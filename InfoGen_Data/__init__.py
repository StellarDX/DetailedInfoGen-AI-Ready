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

from InfoGen_Data import InfoGen

from InfoGen_Data.Modules.ChineseHelpFormatter import InfoGenHelpFormatter

from InfoGen_Data.Modules import Classifications

from InfoGen_Data.Modules.I18NAdmin import LoadLocale
from InfoGen_Data.Modules.I18NAdmin import Register as LCAdminRegister
from InfoGen_Data.Modules.I18NAdmin import LocaleAdmin

from InfoGen_Data.Modules.ADBC import UserConfirm
from InfoGen_Data.Modules.ADBC import ADBCClient, ADBCROClient
from InfoGen_Data.Modules.ADBC import Register as ADBCRegister
from InfoGen_Data.Modules.ADBC import ADBCAdmin
from InfoGen_Data.Modules.ADBC import CurrentSQLVariation

from InfoGen_Data.Modules.SystemLoader import LoadObjectsFromSC
from InfoGen_Data.Modules.SystemLoader import CheckNamespace

from InfoGen_Data.Modules.ResourceCreator import Register as ResourceCreatorRegister
from InfoGen_Data.Modules.ResourceCreator import CreateResource

from InfoGen_Data.Modules.LangTools import GetRegister as GetterRegister
from InfoGen_Data.Modules.LangTools import Get