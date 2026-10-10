from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import LoadObjectsFromSC
from InfoGen_Data import SystemLoaderRegister
from InfoGen_Data import UserConfirm
from InfoGen_Data import ADBCClient, CurrentSQLVariation, ReadSQLToDataFrame
from InfoGen_Data import CheckNamespace

from tabulate import tabulate
from sqlalchemy import Table, MetaData, Column, String
from sqlalchemy import Select, Delete, or_
from pandas import DataFrame
from hashlib import sha256
from pathlib import Path

CreateModules = {
    "system": LoadObjectsFromSC
}

def CreateRegister(MainArgParser):
    CreateModules[".ParserC"] = MainArgParser.add_parser("create", help = "导入资源", formatter_class = InfoGenHelpFormatter)
    RCParser = CreateModules[".ParserC"].add_subparsers(dest='restype', help='可用选项')
    # SystemLoader.py
    SystemLoaderRegister(RCParser)

def CreateResource(args):
    # 这里原本想着如果有第三级子命令则走第三级子命令的参数，然后没有第三级子命令的情况下走默认。
    # 后来经实验和全网搜索证实了这种结构只有Go的Cobra才能做到，Python的argparse和Click和C++的CLI11都无法在不硬编码子命令名且参数隔离的前提下实现
    # 而我已经不想在这个项目里引入第三种语言了（x
    # if args.restype is None:
    #     LoadObjectsFromSC(args)
    if args.restype in CreateModules: 
        CreateModules[args.restype](args)
    else:
        print(f"无效的选项：{args.restype}\n")
        CreateModules[".ParserC"].print_help()

def DeleteSystemConfirmString(Data:DataFrame):
    CStr = "将移除以下系统：\n"
    CStr += str(tabulate(Data, 
        headers = "keys", 
        tablefmt = "plain", 
        showindex = False
    )) + "\n"
    return CStr + "是否继续？（y/n）："

def DeleteSystem(args):
    SysCols = [Column(i, j) for i, j in [
        ["namespace",              String(255)],
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["source_file",            String(1024)],
        ["source_hash",            String(512)]
    ]]
    SysTbl = Table("ig_system", MetaData(), *SysCols)
    if args.namespace == None and args.all:
        # SelectStatement = str(Select(SysTbl)
        #     .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        # Connection = ADBCClient()
        # Result = ReadSQLToDataFrame(Statement, Connection)
        # Connection.close()
        Confirm = UserConfirm("此操作将清除全部数据，是否继续？（y/n）：")
        if Confirm:
            DeleteStatement = str(Delete(SysTbl)
                .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            Connection = ADBCClient()
            with Connection.cursor() as Cursor:
                Cursor.execute(DeleteStatement)
            Connection.commit()
            print("数据已清除")
            Connection.close()
    elif args.namespace:
        CheckNamespace(args.namespace)
        if args.all:
            SelectStatement = str(Select(SysTbl)
                .where(SysTbl.c.namespace == args.namespace)
                .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            Connection = ADBCClient()
            Result = ReadSQLToDataFrame(SelectStatement, Connection)
            if len(Result) == 0:
                print("没找到任何资源")
                return
            Confirm = UserConfirm(DeleteSystemConfirmString(Result))
            if Confirm:
                IDList = Result["system_id"].to_list()
                DeleteStatement = str(Delete(SysTbl)
                    .where(SysTbl.c.system_id.in_(IDList))
                    .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
                with Connection.cursor() as Cursor:
                    Cursor.execute(DeleteStatement)
                Connection.commit()
                print(f"已移除{len(IDList)}个系统")
            Connection.close()
        elif args.name:
            SelectStatement = str(Select(SysTbl)
                .where(SysTbl.c.namespace == args.namespace)
                .where(or_(SysTbl.c.system_id == args.name, SysTbl.c.main_id == args.name))
                .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            Connection = ADBCClient()
            Result = ReadSQLToDataFrame(SelectStatement, Connection)
            if len(Result) == 0:
                print("没找到任何资源")
                return
            Confirm = UserConfirm(DeleteSystemConfirmString(Result))
            if Confirm:
                IDList = Result["system_id"].to_list()
                DeleteStatement = str(Delete(SysTbl)
                    .where(SysTbl.c.system_id.in_(IDList))
                    .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
                with Connection.cursor() as Cursor:
                    Cursor.execute(DeleteStatement)
                Connection.commit()
                print(f"已移除{len(IDList)}个系统")
            Connection.close()
        else:
            raise ValueError("必须指定一个名称")
    elif args.namespace == None and args.name:
        raise ValueError("必须指定一个命名空间")
    elif args.input:
        FileHash = sha256(Path(args.input).read_bytes()).hexdigest()
        SelectStatement = str(Select(SysTbl)
            .where(SysTbl.c.source_hash == FileHash)
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        Connection = ADBCClient()
        Result = ReadSQLToDataFrame(SelectStatement, Connection)
        if len(Result) == 0:
            print("没找到任何资源")
            return
        Confirm = UserConfirm(DeleteSystemConfirmString(Result))
        if Confirm:
            IDList = Result["system_id"].to_list()
            DeleteStatement = str(Delete(SysTbl)
                .where(SysTbl.c.system_id.in_(IDList))
                .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
            with Connection.cursor() as Cursor:
                Cursor.execute(DeleteStatement)
            Connection.commit()
            print(f"已移除{len(IDList)}个系统")
        Connection.close()
    else:
        raise ValueError("必须指定一个命名空间或文件")

DeleteModules = {
    "system": DeleteSystem
}

def DeleteSystemRegister(RCParser):
    SysDParser = RCParser.add_parser("system", help = "删除数据库中已保存的系统")
    SysDParser.add_argument("-S", "--input", type = str, help = "按文件删除")
    SysDParser.add_argument("-n", "--namespace", type = str, help = "命名空间")
    SysDParser.add_argument("name", nargs = "?", help = "资源名称")
    SysDParser.add_argument("--all", action = 'store_true', help = "删除所有系统")

def DeleteRegister(MainArgParser):
    DeleteModules[".ParserD"] = MainArgParser.add_parser("delete", help = "删除资源", formatter_class = InfoGenHelpFormatter)
    RCParser = DeleteModules[".ParserD"].add_subparsers(dest='restype', help='可用选项')
    DeleteSystemRegister(RCParser)

def DeleteResource(args):
    if args.restype in DeleteModules: 
        DeleteModules[args.restype](args)
    else:
        print(f"无效的选项：{args.restype}\n")
        DeleteModules[".ParserD"].print_help()
