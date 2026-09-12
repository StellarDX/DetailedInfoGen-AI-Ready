'''
国际化管理系统
'''

from InfoGen_Data import InfoGenHelpFormatter

import sys
import argparse
import gettext
import polib

from pathlib import Path

LocaleDir      = "Locale"
LocaleSrcPath  = "./InfoGen_Data/StreamingAssets"
LocaleFullPath = LocaleSrcPath + '/' + LocaleDir
LocalePath     = "./IGAppData"
LocaleLoadPath = LocalePath + '/' + LocaleDir

def CompileLangFile(PoFile: str, MoFile: str):
    Path(MoFile).parent.mkdir(parents = True, exist_ok = True)
    Po = polib.pofile(PoFile)
    Po.save_as_mofile(MoFile)

def InitLocale():
    print(f"在{LocaleFullPath}中查找语言文件...")
    LangFiles = list(Path(LocaleFullPath).rglob("*.lang"))
    print(f"将编译{len(LangFiles)}个文件")
    for Src in LangFiles:
        Dst = Path(LocalePath) / Src.relative_to(LocaleSrcPath).with_suffix(".mo")
        print(f"正在编译：{Src} -> {Dst}");
        CompileLangFile(Src, Dst)
    print(f"完成")
    
Modules = {
    "init": InitLocale
}

def LoadLocale(Domain:str, LCID = '2052'):
    return gettext.translation(Domain, LocaleLoadPath, [f"{LCID}"], fallback = True)

def Register(MainArgParser):
    ParserLocale = MainArgParser.add_parser("lcadmin", help = "国际化管理系统", formatter_class = InfoGenHelpFormatter)
    SubParsers = ParserLocale.add_subparsers(dest='lcmodule', help='可用选项')

    InitParser = SubParsers.add_parser("init", help = f"编译所有的本地化文件到默认输出目录（{LocaleLoadPath}）")

    Modules[".Parser"] = ParserLocale

def LocaleAdmin(args):
    if len(sys.argv) == 2:
        Modules[".Parser"].print_help()
        return

    if args.lcmodule in Modules:
        Modules[args.lcmodule]()
    else:
        print(f"无效的选项：{args.lcmodule}\n")
        Modules[".Parser"].print_help()
    