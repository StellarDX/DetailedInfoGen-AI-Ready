from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import ADBCClient, CurrentSQLVariation, ReadSQLToDataFrame
from InfoGen_Data import CheckNamespace

lazy from pandas import DataFrame
import re
lazy from sqlalchemy import Column, Table, MetaData
lazy from sqlalchemy import String, Text
lazy from sqlalchemy import select, insert, update, delete
lazy from sqlalchemy import or_
lazy from tabulate import tabulate

PrefixPattern = re.compile(r'^(?!-)[A-Za-z0-9-]{1,63}(?<!-)(\.(?!-)[A-Za-z0-9-]{1,63}(?<!-))*$') # RFC 1035，RFC 1123
KeyPattern = re.compile(r'(^[a-zA-Z0-9][a-zA-Z0-9._-]*[a-zA-Z0-9]$)|(^[a-zA-Z0-9]$)')

def CheckKey(Prefix, Key):
    if Prefix and not (len(Prefix) <= 253 and PrefixPattern.fullmatch(Prefix)):
        raise ValueError(f"无效的注解前缀\"{Prefix}\"\n前缀必须是一系列由点（.）分隔的 DNS 标签， 总计不超过 253 个字符，后跟斜杠（/）。")
    if not(len(Key) <= 63 and KeyPattern.fullmatch(Key)):
        raise ValueError(f"无效的注解键\"{Key}\"\n名称段必须在 63 个字符以内，以字母数字字符（[a-z0-9A-Z]）开头和结尾， 并允许使用破折号（-），下划线（_），点（.）和字母数字。")

def ParseAnnotations(Input:list):
    UpdateList = {"key_prefix": [], "key_name": [], "value": []}
    DeleteList = {"key_prefix": [], "key_name": []}
    for i in Input:
        Prefix, Key, Value = None, None, None
        if '=' not in i and i[-1] == '-':
            Key = i[:-1]
            if '/' in Key:
                Prefix, Dummy, Key = Key.partition("/")
            CheckKey(Prefix, Key)
            DeleteList["key_prefix"].append(Prefix)
            DeleteList["key_name"].append(Key)
        else:
            Key, Dummy, Value = i.partition("=")
            if '/' in Key:
                Prefix, Dummy, Key = Key.partition("/")
            CheckKey(Prefix, Key)
            UpdateList["key_prefix"].append(Prefix)
            UpdateList["key_name"].append(Key)
            UpdateList["value"].append(Value)
    return DataFrame(UpdateList), DataFrame(DeleteList)

def LocateResourceID(Namespace:str, ResourceType:str, ID:str):
    ExistingSystemTable = Table("ig_system", MetaData(), 
        Column('system_id', String),
        Column('main_id', String),
        Column('namespace', String)
    )
    ExistingObjectTable = Table("ig_object", MetaData(), 
        Column('system_id', String),
        Column('object_id', String),
        Column('primary_name', String),
        Column('otype', String)
    )
    ObjectAliasTable = Table("ig_identifiers", MetaData(), 
        Column('object_id', String),
        Column('alias', String)
    )

    Connection = ADBCClient()
    QueryName = ID
    match ResourceType:
        case "system":
            ExistingRsrcQuery = str((select(ExistingSystemTable)
                .where(ExistingSystemTable.c.namespace == Namespace)
                .where(or_(ExistingSystemTable.c.main_id == QueryName, ExistingSystemTable.c.system_id == QueryName))
            ).compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            ExistingSystem = ReadSQLToDataFrame(ExistingRsrcQuery, Connection)
            if len(ExistingSystem) == 0:
                print("没找到任何资源")
                return
            elif len(ExistingSystem) == 1:
                QueryName = ExistingSystem.at[0, "system_id"]
            elif len(ExistingSystem) >= 2:
                print("找到多个匹配的资源：")
                print(str(tabulate(ExistingSystem, 
                    headers = "keys", 
                    tablefmt = "plain", 
                    showindex = True
                )))
                Index = None
                while Index == None or not(Index >= 0 and Index < len(ExistingSystem)):
                    Index = int(input("请输入序号选择一个目标物体，按Ctrl+C取消操作："))
                QueryName = ExistingSystem.at[Index, "system_id"]
        case "object":
            ExistingRsrcQuery = str((select(ExistingSystemTable.c.namespace, ExistingObjectTable.c.object_id, ExistingSystemTable.c.main_id, ExistingObjectTable.c.primary_name, ExistingObjectTable.c.otype).select_from(ExistingObjectTable
                    .join(ExistingSystemTable, ExistingSystemTable.c.system_id == ExistingObjectTable.c.system_id)
                    .join(ObjectAliasTable, ObjectAliasTable.c.object_id == ExistingObjectTable.c.object_id))
                .where(ExistingSystemTable.c.namespace == Namespace)
                .where(or_(ExistingObjectTable.c.object_id == QueryName, ExistingObjectTable.c.primary_name == QueryName, ObjectAliasTable.c.alias == QueryName))
            ).compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            ExistingObject = ReadSQLToDataFrame(ExistingRsrcQuery, Connection)
            if len(ExistingObject) == 0:
                print("没找到任何资源")
                return
            elif len(ExistingObject) == 1:
                QueryName = ExistingObject.at[0, "object_id"]
            elif len(ExistingObject) >= 2:
                print("找到多个匹配的资源：")
                print(str(tabulate(ExistingObject, 
                    headers = "keys", 
                    tablefmt = "plain", 
                    showindex = True
                )))
                Index = None
                while Index == None or not(Index >= 0 and Index < len(ExistingObject)):
                    Index = int(input("请输入序号选择一个目标物体，按Ctrl+C取消操作："))
                QueryName = ExistingObject.at[Index, "object_id"]
        case _:
            raise ValueError("无效的资源类型")

    Connection.close()
    return QueryName

def AddAnnotations(Namespace:str, ResourceType:str, ID:str, UpdateList:DataFrame, Overwrite = False):
    Cols = [Column(i, j) for i, j in [
        ["rsrc_type",              String(16)],
        ["rsrc_id",                String(256)],
        ["key_prefix",             String(253)],
        ["key_name",               String(63)],
        ["value",                  Text]
    ]]
    Tbl = Table("ig_annotation", MetaData(), *Cols)

    QueryName = LocateResourceID(Namespace, ResourceType, ID)
    Connection = ADBCClient()

    ExistingAnnotationsQuery = str((select(Tbl.c.key_prefix, Tbl.c.key_name, Tbl.c.value)
        .where(Tbl.c.rsrc_type == ResourceType)
        .where(Tbl.c.rsrc_id == QueryName)
    ).compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    PrimaryKeys = ["key_prefix", "key_name"]
    UpdateListWithIndex = UpdateList
    ExistingAnnotations = ReadSQLToDataFrame(ExistingAnnotationsQuery, Connection)
    Intersection = UpdateListWithIndex.merge(ExistingAnnotations[PrimaryKeys], on = PrimaryKeys, how = "inner")
    Difference = (UpdateListWithIndex.merge(ExistingAnnotations[PrimaryKeys], on = PrimaryKeys, how = "left", indicator = True)
        .query("_merge == 'left_only'")
        .drop(columns = '_merge')
        .reset_index(drop = True))

    if not Overwrite and len(Intersection) != 0:
        raise ValueError("重复注解，覆盖已有注解请使用--overwrite")

    Statements = []
    if len(Difference) != 0:
        InsertDiff = insert(Tbl).values([{"rsrc_type": ResourceType, "rsrc_id": QueryName, **i} 
            for i in Difference.to_dict(orient = "records")])
        Statements.append(str(InsertDiff.compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True})))

    if Overwrite and len(Intersection) != 0:
        UpdateDiff = [update(Tbl)
            .where(Tbl.c.rsrc_type == ResourceType)
            .where(Tbl.c.rsrc_id == QueryName)
            .where(Tbl.c.key_prefix == Intersection.at[i, "key_prefix"])
            .where(Tbl.c.key_name == Intersection.at[i, "key_name"])
            .values(value = Intersection.at[i, "value"])
            for i in range(len(Intersection))
        ]
        UpdateDiff = [str(i.compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            for i in UpdateDiff]
        Statements += UpdateDiff

    with Connection.cursor() as Cursor:
        for i in Statements:
            Cursor.execute(i)
    Connection.commit()

    Connection.close()

def DeleteAnnotations(Namespace:str, ResourceType:str, ID:str, DeleteList:DataFrame):
    Cols = [Column(i, j) for i, j in [
        ["rsrc_type",              String(16)],
        ["rsrc_id",                String(256)],
        ["key_prefix",             String(253)],
        ["key_name",               String(63)],
        ["value",                  Text]
    ]]
    Tbl = Table("ig_annotation", MetaData(), *Cols)

    QueryName = LocateResourceID(Namespace, ResourceType, ID)
    Connection = ADBCClient()

    ExistingAnnotationsQuery = str((select(Tbl.c.key_prefix, Tbl.c.key_name)
        .where(Tbl.c.rsrc_type == ResourceType)
        .where(Tbl.c.rsrc_id == QueryName)
    ).compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    PrimaryKeys = ["key_prefix", "key_name"]
    UpdateListWithIndex = DeleteList
    ExistingAnnotations = ReadSQLToDataFrame(ExistingAnnotationsQuery, Connection)
    Intersection = UpdateListWithIndex.merge(ExistingAnnotations[PrimaryKeys], on = PrimaryKeys, how = "inner")
    Difference = (UpdateListWithIndex.merge(ExistingAnnotations[PrimaryKeys], on = PrimaryKeys, how = "left", indicator = True)
        .query("_merge == 'left_only'")
        .drop(columns = '_merge')
        .reset_index(drop = True))

    if len(Difference) != 0:
        for i in Difference[["key_prefix", "key_name"]].to_dict(orient = "records"):
            print(f"\"{i["key_prefix"]}/{i["key_name"]}\"：没有那个注解")
    if len(Intersection) == 0:
        print("没有进行任何修改")
        return

    Statements = [delete(Tbl)
        .where(Tbl.c.rsrc_type == ResourceType)
        .where(Tbl.c.rsrc_id == QueryName)
        .where(Tbl.c.key_prefix == Intersection.at[i, "key_prefix"])
        .where(Tbl.c.key_name == Intersection.at[i, "key_name"])
        for i in range(len(Intersection))
    ]
    Statements = [str(i.compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        for i in Statements]

    with Connection.cursor() as Cursor:
        for i in Statements:
            Cursor.execute(i)
    Connection.commit()

    Connection.close()

def Register(RCParser):
    AnnotateParser = RCParser.add_parser("annotate", help = "管理注解", formatter_class = InfoGenHelpFormatter)
    AnnotateParser.add_argument("resource", nargs = 1, choices = ["system", "object"])
    AnnotateParser.add_argument("ident", help = "系统ID/主ID，或物体ID/名称/别名")
    AnnotateParser.add_argument("-n", "--namespace", type = str, required = True)
    AnnotateParser.add_argument("pairs", nargs = "+", metavar = "KEY=VALUE|KEY-")
    AnnotateParser.add_argument("--overwrite", action = "store_true", help = "覆盖已存在的注解")

def Annotate(args):
    CheckNamespace(args.namespace)

    UpdateList, DeleteList = ParseAnnotations(args.pairs)

    if len(UpdateList) != 0:
        AddAnnotations(args.namespace, args.resource[0], args.ident, UpdateList, args.overwrite)
    if len(DeleteList) != 0:
        DeleteAnnotations(args.namespace, args.resource[0], args.ident, DeleteList)