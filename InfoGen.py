from InfoGen_Data import ResourceManagerCRegister, CreateResource
from InfoGen_Data import ResourceManagerDRegister, DeleteResource
from InfoGen_Data import GetterRegister, Get
from InfoGen_Data import DescribeRegister, Describe
from InfoGen_Data import LCAdminRegister, LocaleAdmin
from InfoGen_Data import ADBCRegister, ADBCAdmin
from InfoGen_Data import LangGraphRegister, Generate
from InfoGen_Data import InfoGenHelpFormatter
import sys
import os
from argparse import ArgumentParser, SUPPRESS

def Hello(args):
    print(r"""++-----------------------------------------------------------------------------------++
|| 回环的涟漪，守候在昨日的岁月。                                                    ||
++-----------------------------------------------------------------------------------++
||   _    _                                                                          ||
||  | |  | |         _   _                                           _       _   _   ||
||  | |__| |  ____  | | | |  _____       __        __  _____   ___  | |  ___| | | |  ||
||  |  __  | / __ \ | | | | /  _  \      \ \  /\  / / /  _  \ |  _| | | /  _  | | |  ||
||  | |  | | | ___/ | | | | | |_| |  _    \ \/  \/ /  | |_| | | |   | | | |_| | |_|  ||
||  |_|  |_| \____| |_| |_| \_____/ ( )    \__/\__/   \_____/ |_|   |_| \___,_| (_)  ||
||                                  |/                                               ||
++-----------------------------------------------------------------------------------++
|| 开拓的罗盘，指引明天滚滚向前。                                                    ||
++-----------------------------------------------------------------------------------++""")

def Egg(MainArgParser):
    ParserLocale = MainArgParser.add_parser("hello", help = "浪漫一如初见♪", formatter_class = InfoGenHelpFormatter)

ModuleList = {
    "create": {"Register": ResourceManagerCRegister, "Invoke": CreateResource},
    "delete": {"Register": ResourceManagerDRegister, "Invoke": DeleteResource},
    "get": {"Register": GetterRegister, "Invoke": Get},
    "describe": {"Register": DescribeRegister, "Invoke": Describe},
    "generate": {"Register": LangGraphRegister, "Invoke": Generate},
    "adbc": {"Register": ADBCRegister, "Invoke": ADBCAdmin},
    "lcadmin": {"Register": LCAdminRegister, "Invoke": LocaleAdmin},
    "hello": {"Register": Egg, "Invoke": Hello}
}

MainArgParser = ArgumentParser(prog = 'InfoGen', description='SpaceEngine详细信息生成器 (AI-Ready)', formatter_class = InfoGenHelpFormatter)
ModuleParsers = MainArgParser.add_subparsers(dest='command', help='可用选项')

def main():
    for i in ModuleList:
        ModuleList[i]["Register"](ModuleParsers)

    if len(sys.argv) == 1:
        MainArgParser.print_help()
        return

    args = MainArgParser.parse_args()
    # print(f"当前调用选项: {args.command}")

    if args.command not in ModuleList:
        print(f"无效的选项：{args.command}")
        print()
        MainArgParser.print_help()
        return

    ModuleList[args.command]["Invoke"](args)

if __name__ == '__main__':
    # print(os.getpid())
    # input("Press Enter to attach...")
    main()