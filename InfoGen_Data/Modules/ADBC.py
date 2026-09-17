from InfoGen_Data import InfoGenHelpFormatter

from adbc_driver_manager import dbapi
from pathlib import Path
from copy import deepcopy

import tomllib
import string
import sqlparse
import re

_ADBCConf = None
_ADBCROConf = None
_Connection = None
_ROConnection = None

ConnectionTemplate = "./InfoGen_Data/Config/ADBC.toml.def"
ConnectionConf = "./InfoGen_Data/Config/ADBC.toml"
SchemaFile = "./InfoGen_Data/Resources/Schema-ADBC.sql"

def ADBCConfig():
    global _ADBCConf
    if _ADBCConf == None:
        with Path(ConnectionConf).open("rb") as ADBCConfFile:
            _ADBCConf = tomllib.load(ADBCConfFile)
    return _ADBCConf

def ADBCROConfig():
    global _ADBCROConf
    if _ADBCROConf == None:
        _ADBCROConf = deepcopy(ADBCConfig())
        _ADBCROConf["Connection"]["conn_kwargs"]["adbc.connection.read_only"] = "true"
    return _ADBCROConf

def RunInit(Connection):
    for Statement in ADBCConfig()["InitStatements"]["init_statements"]:
        with Connection.cursor() as Cursor:
            Cursor.execute(Statement)

def ADBCClient():
    global _Connection
    if _Connection is None:
        _Connection = dbapi.connect(**(ADBCConfig()["Connection"]))
        RunInit(_Connection)
    return _Connection

def ADBCROClient():
    global _ROConnection
    if _ROConnection is None:
        _ROConnection = dbapi.connect(**(ADBCROConfig()["Connection"]))
        RunInit(_ROConnection)
    return _ROConnection

def TomlQuote(Value: str) -> str:
    return '"' + Value.replace('\\', '\\\\').replace('"', '\\"') + '"'

def ExecuteScript(Connection, Text: str):
    Statements = [i for i in sqlparse.split(sqlparse.format(Text, strip_comments = True)) if i.strip()]
    for Statement in Statements:
        with Connection.cursor() as Cursor:
            Cursor.execute(Statement)
    Connection.commit()

def ListTables(Connection):
    Reader = Connection.adbc_get_objects(depth = "tables")
    try:
        Tables = []
        for Catalog in Reader.read_all().to_pylist():
            for DbSchema in Catalog["catalog_db_schemas"] or []:
                for Table in DbSchema["db_schema_tables"] or []:
                    Tables.append(Table["table_name"])
        return Tables
    finally:
        Reader.close()

def TableExists(Connection, TableName: str) -> bool:
    try:
        with Connection.cursor() as Cursor:
            Cursor.execute(f'SELECT 1 FROM "{TableName}" WHERE 1 = 0')
            Cursor.fetchall()          # 有些驱动把错误推迟到 fetch 才报
        return True
    except dbapi.ProgrammingError:     # 42S02/42P01 → no such table
        return False

def Init(args):
    Template = Path(ConnectionTemplate).read_text(encoding = "utf-8")
    Rendered = string.Template(Template).substitute(
        DRIVER = TomlQuote(args.driver),
        URI    = TomlQuote(args.uri)
    )

    Conf = tomllib.loads(Rendered) # 产物自检，坏TOML直接在这里炸
    Conn = dbapi.connect(**Conf["Connection"])
    for Statement in Conf["InitStatements"]["init_statements"]:
        with Conn.cursor() as Cursor:
            Cursor.execute(Statement)
    print("连接测试通过，开始初始化数据库")

    # 执行./InfoGen_Data/Resources/Schema-ADBC.sql
    # 原本这里应该先检查数据库是不是空的（这里要一个空数据库来存放，防止数据丢失）。
    # 但是因为数据库产品没有统一的标准，搞得要用通用指令难上加难

    SchemaText = Path(SchemaFile).read_text(encoding = "utf-8")
    SchemaTables = [T.lower() for T in re.findall(
        r"CREATE\s+TABLE\s+(?:IF\s+NOT\s+EXISTS\s+)?([A-Za-z_][A-Za-z0-9_]*)", SchemaText, re.IGNORECASE)]

    # 通用空库检查：优先走ADBC元数据接口，驱动不支持就退回SQL探针
    try:
        Existing = [T for T in ListTables(Conn) if T]
    except dbapi.NotSupportedError:
        print("目标数据库未实现获取表数据方法，正在检查自身表是否存在")
        Existing = [T for T in SchemaTables if TableExists(Conn, T)]
        Continue = False
        Confirm = ''
        while Confirm not in ["y", "n", "Y", "N"]:
            Confirm = input("继续执行可能污染原有表结构，是否继续？（y/n)：")
            if (Confirm == 'y' or Confirm == 'Y'):
                Continue = True
            elif (Confirm == 'n' or Confirm == 'N'):
                Continue = False
        if Continue == False:
            raise InterruptedError("用户取消执行")

    if Existing:
        Conn.close()
        print(f"目标数据库非空，本次init已中止，避免把表结构和已有数据混在一起")
        raise RuntimeError("Init指定的数据库必须是空的")

    ExecuteScript(Conn, SchemaText)
    print(f"已执行：{SchemaFile}")
    Conn.close()

    # --------------------------------------------------

    Out = Path(ConnectionConf)
    if Out.exists() and not args.force:
        print(f"{Out} 已存在，如需覆盖请加 --force（原文件不会被备份）")
        return
    Out.parent.mkdir(parents = True, exist_ok = True)
    Out.write_text(Rendered, encoding = "utf-8")
    print(f"已生成：{Out}")

Modules = {"init": Init}

def Register(MainArgParser):
    ParserADBC = MainArgParser.add_parser("adbc", help = "ADBC数据库管理系统", formatter_class = InfoGenHelpFormatter)
    SubParsers = ParserADBC.add_subparsers(dest = 'adbcmodule', help = '可用选项')
    InitParser = SubParsers.add_parser("init", help = f"从原初模板渲染出副本配置（{ConnectionConf}）")
    InitParser.add_argument("--driver", type = str, required = True, help = "ADBC驱动路径")
    InitParser.add_argument("--uri", type = str, required = True, help = "连接目标：文件路径或DSN")
    InitParser.add_argument("-F", "--force", action = 'store_true', help = "覆盖已存在的副本配置")
    Modules[".Parser"] = ParserADBC

def ADBCAdmin(args):
    if args.adbcmodule is None:
        Modules[".Parser"].print_help()
        return
    if args.adbcmodule in Modules:
        Modules[args.adbcmodule](args)
    else:
        print(f"无效的选项：{args.adbcmodule}\n")
        Modules[".Parser"].print_help()