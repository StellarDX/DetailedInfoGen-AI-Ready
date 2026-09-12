from InfoGen_Data import LCAdminRegister, LocaleAdmin
from InfoGen_Data import CreateRegister, LoadObjectsFromSC
from InfoGen_Data import InfoGenHelpFormatter
import sys
import os
from argparse import ArgumentParser

ModuleList = {
    "lcadmin": {"Register": LCAdminRegister, "Invoke": LocaleAdmin},
    "create": {"Register": CreateRegister, "Invoke": LoadObjectsFromSC}
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