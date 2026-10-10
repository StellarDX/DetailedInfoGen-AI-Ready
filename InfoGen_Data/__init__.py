import sys

if sys.platform == "win32":
    # C/C++ 侧(spdlog)向真实控制台写 UTF-8 字节时，把控制台输出代码页切到 UTF-8，
    # 避免默认 ANSI 代码页(936/GBK)把 UTF-8 中文解码成乱码。
    try:
        import ctypes
        ctypes.windll.kernel32.SetConsoleOutputCP(65001)  # CP_UTF8
    except Exception:
        pass

# 重定向/管道场景(IDE 输出窗格)下，让 Python print 也以 UTF-8 编码输出，
# 与 C++ 扩展(spdlog)写出的 UTF-8 字节保持一致。
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError, OSError):
        pass

def ArgToDict(s):
    k, v = s.split('=', 1)
    return k, v

from InfoGen_Data import InfoGen

from InfoGen_Data.Modules.ChineseHelpFormatter import InfoGenHelpFormatter

from InfoGen_Data.Modules import Classifications

from InfoGen_Data.Modules.I18NAdmin import LoadLocale
from InfoGen_Data.Modules.I18NAdmin import Register as LCAdminRegister
from InfoGen_Data.Modules.I18NAdmin import LocaleAdmin

from InfoGen_Data.Modules.ADBC import UserConfirm
from InfoGen_Data.Modules.ADBC import ADBCClient, ADBCRawClient, ReadSQLToDataFrame, LoadTableSchemaToDataFrame
from InfoGen_Data.Modules.ADBC import Register as ADBCRegister
from InfoGen_Data.Modules.ADBC import ADBCAdmin
from InfoGen_Data.Modules.ADBC import CurrentSQLVariation
from InfoGen_Data.Modules.ADBC import RegisterSQLVariation

def CheckNamespace(Namespace):
    from re import fullmatch
    if fullmatch("^[a-z0-9]([-a-z0-9]*[a-z0-9])?$", Namespace) is not None and len(Namespace) <= 63:
        return;
    else:
        raise ValueError("命名空间只能以只能包含小写字母（a-z）、数字（0-9）以及连字符/中划线（-），必须以字母或数字开头和结尾，不能以连字符（-）开头或结尾，且最大长度不能超过63个字符。")

from InfoGen_Data.Modules.SystemLoader import LoadObjectsFromSC
from InfoGen_Data.Modules.SystemLoader import Register as SystemLoaderRegister
    
from InfoGen_Data.Modules.ResourceManager import CreateRegister as ResourceManagerCRegister
from InfoGen_Data.Modules.ResourceManager import CreateResource
from InfoGen_Data.Modules.ResourceManager import DeleteRegister as ResourceManagerDRegister
from InfoGen_Data.Modules.ResourceManager import DeleteResource

from InfoGen_Data.Modules.SQLQuery import GetRegister as GetterRegister, DescribeRegister as DescribeRegister
from InfoGen_Data.Modules.SQLQuery import Get, Describe

from InfoGen_Data.Modules.SQLQuery import SetNamespace
from InfoGen_Data.Modules.SQLQuery import QuerySystem, QueryAllObjectsInSystem, QueryObject

from InfoGen_Data.Modules.LangGraph import Register as LangGraphRegister
from InfoGen_Data.Modules.LangGraph import Generate

__import__("InfoGen_Data.Plug-ins")