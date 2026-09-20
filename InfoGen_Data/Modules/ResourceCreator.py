from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import LoadObjectsFromSC

Modules = {
    "system": LoadObjectsFromSC
}

def ArgToDict(s):
    k, v = s.split('=', 1)
    return k, v

def Register(MainArgParser):
    Modules[".Parser"] = MainArgParser.add_parser("create", help = "从SpaceEngine导出的SC文件导入物体", formatter_class = InfoGenHelpFormatter)
    RCParser = Modules[".Parser"].add_subparsers(dest='restype', help='可用选项')
    # SystemLoader.py
    SysParser = RCParser.add_parser("system", help = "从SpaceEngine导出的SC文件导入物体")
    SysParser.add_argument("-S", "--input", type = str, required = True, help = "输入的文件")
    SysParser.add_argument("-B", "--output", type = str, default = "./Export", help = "输出目录")
    SysParser.add_argument("--code-page", type = int, default = 65001, help = "输入的文件的编码（防止烫屯锟斤拷）[TODO]")
    SysParser.add_argument("--absolute-orbit", action='store_true', help = "轨道是否为绝对参照系（如黄道，天球这些，默认为相对参照系，如赤道）")
    SysParser.add_argument("--common-plane-threshold", type = float, default = 9, help = "10^(-x)，当计算的黄道轨道面与上一级物体的黄道轨道面相差少于这个值时视为共面，仅当--absolute-orbit=False时生效")
    SysParser.add_argument("--esi-estimator", type = str, default = "SolarSys", choices = ["SolarSys", "Extrasolar"], help = "地球相似指数算法，当前可用：\n * SolarSys - 使用半径，密度，逃逸速度和温度计算。\n * Extrasolar - 使用恒星辐射通量和行星半径计算")
    SysParser.add_argument("-t", "--sort-system", action='store_true', help = "对输出的内容按半长轴的顺序排序")
    SysParser.add_argument("--exceptional-asteroids-pred", type = str, default = "LargestByDiameter", choices = ["LargestByDiameter", "MostMassive", "SlowestRotators", "FastestRotators", "Retrograde", "HighlyInclined"], help = "小行星列表排序依据，默认半径降序，可用：\n * LargestByDiameter - 半径降序\n * MostMassive - 质量降序\n * SlowestRotators - 自转周期降序\n * FastestRotators - 自转周期升序\n * Retrograde - 筛选轨道倾角大于90小于270的小行星并按倾角升序排序\n * HighlyInclined - 轨道面与黄道面夹角降序")
    SysParser.add_argument("--exceptional-asteroids-limit", type = int, default = 50, help = "小行星列表最大数量")
    SysParser.add_argument("-L", "--lcid", type = str, default = '2052', help = "输出文档的语言ID")
    SysParser.add_argument("-f", "--format", type = str, default = "Markdown", help = "输出文档格式")
    SysParser.add_argument("-D", "--format-args", action = 'append', type = ArgToDict, default = [], help = "输出文档格式的额外参数")
    SysParser.add_argument("-i", "--store", action='store_true', help = "将生成的原始数据保存到数据库")
    SysParser.add_argument("-n", "--namespace", type = str, help = "保存数据的命名空间")
    SysParser.add_argument("--store-mode", type = str, default = "dist-upgrade", choices = ["dist-upgrade"], help = "插入数据的模式（暂时只支持dist-upgrade，即卸载后重新导入）")
    
def CreateResource(args):
    # 这里原本想着如果有第三级子命令则走第三级子命令的参数，然后没有第三级子命令的情况下走默认。
    # 后来经实验和全网搜索证实了这种结构只有Go的Cobra才能做到，Python的argparse和Click和C++的CLI11都无法在不硬编码子命令名且参数隔离的前提下实现
    # 而我已经不想在这个项目里引入第三种语言了（x
    # if args.restype is None:
    #     LoadObjectsFromSC(args)
    if args.restype in Modules: 
        Modules[args.restype](args)
    else:
        print(f"无效的选项：{args.restype}\n")
        Modules[".Parser"].print_help()