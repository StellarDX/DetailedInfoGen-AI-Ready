from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import ADBCClient, CurrentSQLVariation
from InfoGen_Data import CheckNamespace

import yaml

from sqlalchemy import Table, MetaData, Column, select, or_
from sqlalchemy import String, Double, Integer, Boolean, DateTime, BigInteger, Text
from pandas import read_sql
from tabulate import tabulate
from langchain.tools import tool

_NamespaceBuffer = ""

def GetNamespace():
    return _NamespaceBuffer

def SetNamespace(NewNamespace):
    global _NamespaceBuffer
    _NamespaceBuffer = NewNamespace

def _PreCheck(Namespace, SystemName):
    if Namespace != "-A":
        if Namespace == None:
            raise ValueError("必须指定一个命名空间")
        CheckNamespace(Namespace)

def _QuerySystem_Unchecked(Namespace, SystemName):
    Cols = [Column(i, j) for i, j in [
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["source_file",            String(1024)],
        ["source_hash",            String(512)],
        ["code_page",              Integer],
        ["esi_estimator",          String(32)],
        ["absolute_orbit",         Boolean],
        ["common_plane_threshold", Integer],
        ["namespace",              String(255)],
        ["n_stars",                Integer],
        ["n_planets",              Integer],
        ["n_dwarf_planets",        Integer],
        ["n_satellites",           Integer],
        ["n_minor_planets",        Integer],
        ["n_comets",               Integer],
        ["spectral_types",         Text],
        ["create_date",            DateTime],
        ["modified_date",          DateTime]
    ]]
    Tbl = Table("ig_system", MetaData(), *Cols)

    Statement = ""
    if Namespace == "-A":
        Statement = str(select(Tbl)
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    elif SystemName == None or SystemName == "":
        Statement = str((select(Tbl)
            .where(Tbl.c.namespace == Namespace))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    else:
        Statement = str((select(Tbl)
            .where(Tbl.c.namespace == Namespace)
            .where(or_(Tbl.c.main_id == SystemName, Tbl.c.system_id == SystemName)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    Connection = ADBCClient()
    return read_sql(Statement, Connection)

def _QuerySystem(Namespace, SystemName, OutFmt):
    _PreCheck(Namespace, SystemName)

    SystemDataFrame = _QuerySystem_Unchecked(Namespace, SystemName)

    if len(SystemDataFrame) == 0:
        return "没找到任何资源"

    match OutFmt:
        case "json":
            return SystemDataFrame.to_json(
                orient = 'records',
                indent = 4
            )
        case "yaml":
            return yaml.dump(SystemDataFrame.to_dict(orient = 'records'),
                allow_unicode = True, 
                sort_keys = False, 
                default_flow_style = False, 
                indent = 2
            )
        case "wide":
            return str(tabulate(SystemDataFrame[["namespace", "main_id", "spectral_types", "create_date", "system_id", "source_file"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))
        case _:
            return str(tabulate(SystemDataFrame[["namespace", "main_id", "spectral_types", "create_date"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))

def _QueryObjectBasic_Unchecked(Namespace, ObjectName):
    SysCols = [Column(i, j) for i, j in [
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["namespace",              String(255)]
    ]]
    SysTbl = Table("ig_system", MetaData(), *SysCols)

    Cols = [Column(i, j) for i, j in [
        ["object_id",        String(512)],
        ["system_id",        String(512)],
        ["parent_object_id", String(512)],
        ["primary_name",     String(255)],
        ["otype",            String(32)],
        ["class",            String(64)],
        ["depth",            Integer],
        ["sibling_index",    Integer],
        ["is_minor",         Boolean]
    ]]
    Tbl = Table("ig_object", MetaData(), *Cols)

    Statement = ""
    if Namespace == "-A":
        Statement = str((select(SysTbl.c.namespace, SysTbl.c.main_id, Tbl)
            .select_from(Tbl.join(SysTbl, SysTbl.c.system_id == Tbl.c.system_id)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    elif ObjectName == None or ObjectName == "":
        Statement = str((select(SysTbl.c.namespace, SysTbl.c.main_id, Tbl)
            .select_from(Tbl.join(SysTbl, SysTbl.c.system_id == Tbl.c.system_id))
            .where(SysTbl.c.namespace == Namespace))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    else:
        Statement = str((select(SysTbl.c.namespace, SysTbl.c.main_id, Tbl)
            .select_from(Tbl.join(SysTbl, SysTbl.c.system_id == Tbl.c.system_id))
            .where(SysTbl.c.namespace == Namespace)
            .where(or_(Tbl.c.primary_name == ObjectName, Tbl.c.object_id == ObjectName)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    Connection = ADBCClient()
    return read_sql(Statement, Connection)

def _QueryObject(Namespace, ObjectName, OutFmt):
    _PreCheck(Namespace, ObjectName)
    ObjectDataFrame = _QueryObjectBasic_Unchecked(Namespace, ObjectName)

    if len(ObjectDataFrame) == 0:
        return "没找到任何资源"

    match OutFmt:
        case "json":
            return ObjectDataFrame.to_json(
                orient = 'records',
                indent = 4
            )
        case "yaml":
            return yaml.dump(ObjectDataFrame.to_dict(orient = 'records'),
                allow_unicode = True, 
                sort_keys = False, 
                default_flow_style = False, 
                indent = 2
            )
        case "wide":
            return str(tabulate(ObjectDataFrame[["namespace", "primary_name", "main_id", "otype", "class", "object_id", "parent_object_id", "system_id", "depth", "sibling_index"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))
        case _:
            return str(tabulate(ObjectDataFrame[["namespace", "primary_name", "main_id", "otype", "class"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))

@tool
def QuerySystem(SystemName): # 这个是给AI看的
    """按名称查询信息生成器数据库中某个恒星系（System）的元数据。

    传入恒星系的主标识 SystemName（对应 ig_system.main_id），返回该恒星系
    记录的 JSON 数组，字段包含 system_id、main_id、spectral_types、
    n_stars、n_planets、create_date 等；当命名空间内不存在该名称时返回空数组。
    """
    return _QuerySystem(GetNamespace(), SystemName, "json")

def GetSystem(args):
    print(_QuerySystem("-A" if args.all_namespaces else args.namespace, args.name, args.output))

def GetObject(args):
    print(_QueryObject("-A" if args.all_namespaces else args.namespace, args.name, args.output))

Modules = {
    "system": GetSystem,
    "object": GetObject
}

Modules1 = {
    
}

def GetRegister(MainArgParser):
    ParserGetter = MainArgParser.add_parser("get", help = "打印数据", formatter_class = InfoGenHelpFormatter)
    ParserGetter.add_argument("-n", "--namespace", type = str, help = "命名空间")
    ParserGetter.add_argument("resource", nargs = 1, help = "资源类型")
    ParserGetter.add_argument("-o", "--output", type = str, help = "输出格式[wide|json|yaml]")
    ParserGetter.add_argument("-A", "--all-namespaces", action = 'store_true', help = "输出全部数据")
    ParserGetter.add_argument("name", nargs = "?", help = "资源名称")
    Modules[".Parser"] = ParserGetter

def DescribeRegister(MainArgParser):
    ParserDescriber = MainArgParser.add_parser("describe", help = "打印详细数据", formatter_class = InfoGenHelpFormatter)
    ParserDescriber.add_argument("-n", "--namespace", type = str, help = "命名空间")
    ParserDescriber.add_argument("resource", nargs = 1, help = "资源类型")
    ParserDescriber.add_argument("-A", "--all-namespaces", action = 'store_true', help = "输出全部数据")
    ParserDescriber.add_argument("name", nargs = "?", help = "资源名称")
    Modules1[".Parser"] = ParserDescriber

def Get(args):
    if args.resource is None:
        print("你需要指定一个资源去获取")
        return
    if args.resource[0] in Modules:
        Modules[args.resource[0]](args)
    else:
        print(f"无效的选项：{args.resource}\n")
        Modules[".Parser"].print_help()

def Describe(args):
    if args.resource is None:
        print("你需要指定一个资源去获取")
        return
    if args.resource[0] in Modules1:
        Modules1[args.resource[0]](args)
    else:
        print(f"无效的选项：{args.resource}\n")
        Modules1[".Parser"].print_help()