from InfoGen_Data import InfoGen
import argparse

def Register(MainArgParser):
    ParserCreate = MainArgParser.add_parser("create", help = "从SpaceEngine导出的SC文件导入物体")
    ParserCreate.add_argument("-S", "--file", type = str, required = True, help = "输入的文件")
    ParserCreate.add_argument("-B", "--output", type = str, default = "./Export", help = "输出目录")
    ParserCreate.add_argument("--code-page", type = int, default = 65001, help = "输入的文件的编码（防止烫屯锟斤拷）")
    ParserCreate.add_argument("--store", action='store_true', help = "将输出的文档向量化后保存到ChromaDB（需要模型支持）")
    ParserCreate.add_argument("--model-config", type = str, help = "向量化文本模型的配置文件")

def PrintArgs(args):
    print(f"配置: ")
    print(f"--file:         {args.file}")
    print(f"--output:       {args.output}")
    print(f"--code-page:    {args.code_page}")
    print(f"--store:        {args.store}")
    print(f"--model-config: {args.model_config}")

def LoadObjectsFromSC(args):
    PrintArgs(args)
    InfoGen.InfoGen_Main(**vars(args))