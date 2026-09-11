from InfoGen_Data import Create
from InfoGen_Data import InfoGenHelpFormatter
import sys
import os
from argparse import ArgumentParser

ModuleList = {
    "create": {"Register": Create.Register, "Invoke": Create.LoadObjectsFromSC}
}

MainArgParser = ArgumentParser(prog = 'InfoGen', description='SpaceEngine详细信息生成器 (AI-Ready)', formatter_class = InfoGenHelpFormatter)
ModuleParsers = MainArgParser.add_subparsers(dest='command', help='可用模块')

def main():
    for i in ModuleList:
        ModuleList[i]["Register"](ModuleParsers)

    if len(sys.argv) == 1:
        MainArgParser.print_help()
        return

    args = MainArgParser.parse_args()
    print(f"当前调用模块: {args.command}")

    if args.command not in ModuleList:
        print(f"模块{args.command}不存在")
        print()
        MainArgParser.print_help()
        return

    ModuleList[args.command]["Invoke"](args)

if __name__ == '__main__':
    print(os.getpid())
    input("Press Enter to attach...")
    main()