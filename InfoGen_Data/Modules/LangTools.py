from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import ADBCClient, CurrentSQLVariation
from InfoGen_Data import CheckNamespace

import yaml

from sqlalchemy import Table, MetaData, Column, select
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
        if SystemName == None or SystemName == "":
            raise ValueError("必须指定一个名称")
        if Namespace == None:
            raise ValueError("必须指定一个命名空间")
        CheckNamespace(Namespace)

def _QuerySystem(Namespace, SystemName, OutFmt):
    _PreCheck(Namespace, SystemName)

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
        Statement = str((select(Tbl))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    else:
        Statement = str((select(Tbl)
            .where(Tbl.c.namespace == Namespace)
            .where(Tbl.c.main_id == SystemName))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    Connection = ADBCClient()
    SystemDataFrame = read_sql(Statement, Connection)

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
        case _:
            return str(tabulate(SystemDataFrame[["namespace", "main_id", "spectral_types", "create_date"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))

@tool
def QuerySystem(SystemName):
    """按名称查询信息生成器数据库中某个恒星系（System）的元数据。

    传入恒星系的主标识 SystemName（对应 ig_system.main_id），返回该恒星系
    记录的 JSON 数组，字段包含 system_id、main_id、spectral_types、
    n_stars、n_planets、create_date 等；当命名空间内不存在该名称时返回空数组。
    """
    return _QuerySystem(GetNamespace(), SystemName, "json")

def GetSystem(args):
    print(_QuerySystem("-A" if args.all_namespaces else args.namespace, args.name, args.output_format))

Modules = {"system": GetSystem}

def GetRegister(MainArgParser):
    ParserGetter = MainArgParser.add_parser("get", help = "把已有数据打印出来", formatter_class = InfoGenHelpFormatter)
    ParserGetter.add_argument("-n", "--namespace", type = str, help = "命名空间")
    ParserGetter.add_argument("resource", nargs = 1, help = "资源类型")
    ParserGetter.add_argument("-o", "--output", type = str, help = "输出格式")
    ParserGetter.add_argument("-A", "--all-namespaces", action = 'store_true', help = "输出全部数据")
    ParserGetter.add_argument("name", nargs = "?", help = "资源名称")
    Modules[".Parser"] = ParserGetter

def Get(args):
    if args.resource is None:
        print("你需要指定一个资源去获取")
        return
    if args.resource[0] in Modules:
        Modules[args.resource[0]](args)
    else:
        print(f"无效的选项：{args.resource}\n")
        Modules[".Parser"].print_help()