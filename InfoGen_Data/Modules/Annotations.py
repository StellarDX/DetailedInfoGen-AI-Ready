from InfoGen_Data import InfoGenHelpFormatter

def Register(RCParser):
    AnnotateParser = RCParser.add_parser("annotate", help = "管理注解", formatter_class = InfoGenHelpFormatter)
    AnnotateParser.add_argument("resource", nargs = 1, choices = ["system", "object"])
    AnnotateParser.add_argument("ident", help = "系统ID/主ID，或物体ID/名称/别名")
    AnnotateParser.add_argument("-n", "--namespace", type = str, required = True)
    AnnotateParser.add_argument("pairs", nargs = "+", metavar = "KEY=VALUE|KEY-")
    AnnotateParser.add_argument("--overwrite", action = "store_true", help = "覆盖已存在的注解")

def Annotate(args): # 占位符
    print(args.pairs)