from InfoGen_Data import InfoGen
from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import LoadLocale

import argparse

from abc import ABC, abstractmethod
from pandas import DataFrame

def Register(MainArgParser):
    ParserCreate = MainArgParser.add_parser("create", help = "从SpaceEngine导出的SC文件导入物体", formatter_class = InfoGenHelpFormatter)
    ParserCreate.add_argument("-S", "--input", type = str, required = True, help = "输入的文件")
    ParserCreate.add_argument("-B", "--output", type = str, default = "./IGOutputData", help = "输出目录")
    ParserCreate.add_argument("--code-page", type = int, default = 65001, help = "输入的文件的编码（防止烫屯锟斤拷）")
    ParserCreate.add_argument("--absolute-orbit", action='store_true', help = "轨道是否为绝对参照系（如黄道，天球这些，默认为相对参照系，如赤道）")
    ParserCreate.add_argument("--common-plane-threshold", type = float, default = 9, help = "10^(-x)，当计算的黄道轨道面与上一级物体的黄道轨道面相差少于这个值时视为共面，仅当--absolute-orbit=False时生效")
    ParserCreate.add_argument("--esi-estimator", type = str, default = "SolarSys", choices = ["SolarSys", "Extrasolar"], help = "地球相似指数算法，当前可用：\n * SolarSys - 使用半径，密度，逃逸速度和温度计算。\n * Extrasolar - 使用恒星辐射通量和行星半径计算")
    ParserCreate.add_argument("--sort-system", action='store_true', help = "对输出的内容按半长轴的顺序排序")
    ParserCreate.add_argument("--exceptional-asteroids-pred", type = str, default = "LargestByDiameter", choices = ["LargestByDiameter", "MostMassive", "SlowestRotators", "FastestRotators", "Retrograde", "HighlyInclined"], help = "小行星列表排序依据，默认半径降序，可用：\n * LargestByDiameter - 半径降序\n * MostMassive - 质量降序\n * SlowestRotators - 自转周期降序\n * FastestRotators - 自转周期升序\n * Retrograde - 筛选轨道倾角大于90小于270的小行星并按倾角升序排序\n * HighlyInclined - 轨道面与黄道面夹角降序")
    ParserCreate.add_argument("--exceptional-asteroids-limit", type = int, default = 50, help = "小行星列表最大数量")
    ParserCreate.add_argument("-L", "--lcid", type = str, default = '2052', help = "输出文档的语言ID")
    ParserCreate.add_argument("-f", "--format", type = str, default = "Markdown", help = "输出文档格式")
    ParserCreate.add_argument("--format-args", nargs = '*', default = [], help = "输出文档格式的额外参数")
    ParserCreate.add_argument("--store", action='store_true', help = "将输出的文档向量化后保存到ChromaDB（需要模型支持）")
    ParserCreate.add_argument("--model-config", type = str, help = "向量化文本模型的配置文件")

def PrintArgs(args):
    print(f"配置: ")
    print(f"--input:                       {args.input}")
    print(f"--output:                      {args.output}")
    print(f"--code-page:                   {args.code_page}")
    print(f"--absolute-orbit:              {args.absolute_orbit}")
    print(f"--common-plane-threshold:      {args.common_plane_threshold}")
    print(f"--esi-estimator:               {args.esi_estimator}")
    print(f"--sort-system:                 {args.sort_system}")
    print(f"--exceptional-asteroids-pred:  {args.exceptional_asteroids_pred}")
    print(f"--exceptional-asteroids-limit: {args.exceptional_asteroids_limit}")
    print(f"--lcid:                        {args.lcid}")
    print(f"--format:                      {args.format}")
    print(f"--format-args:                 {args.format_args}")
    print(f"--store:                       {args.store}")
    print(f"--model-config:                {args.model_config}")

def Classification(OType:str, OClass:str, Mass:float, Radius:float, Temperature:float):
    if OType == "Star":
        return OClass # 恒星光谱字符串非常复杂，因此返回原始类型，后续可以拿AI解析
    

class Generator(ABC):
    def __init__(self, System:dict):
        self._Src = System
        self._Texts = {}

    @abstractmethod
    def SystemInfo(self):
        pass

    @abstractmethod
    def ObjectList(self):
        pass

    @abstractmethod
    def MinorObjectList(self):
        pass

    def Run(self):
        Head = self._Src["MainID"]
        self._Texts[Head] = {}
        self._Texts[Head][LOC("行星系统信息")] = self.SystemInfo()
        self._Texts[Head][LOC("物体列表")] = self.ObjectList()
        self._Texts[Head][LOC("小行星列表")] = self.MinorObjectList()

    def GetTexts(self):
        return self._Texts;

class MarkdownGenerator(Generator):
    def __init__(self, System:dict):
        super().__init__(System)

    def _Object_To_Table(self, Object:dict, ParentBody:str):
        ObjectInfo = {}
        if Object["OType"] == "Star":
            OClass = Object["PhysicalCharacteristics"]["Class"]
            if OClass == "X" or OClass == "BlackHole":
                pass
            else:
                ObjectInfo[LOC("物体类型")] = Classification(
                    Object["OType"], OClass,
                    Object["PhysicalCharacteristics"]["Mass"],
                    Object["PhysicalCharacteristics"]["Radius"],
                    Object["PhysicalCharacteristics"]["Temperature"]
                # TODO
            )
        return ObjectInfo

    def SystemInfo(self):
        System = self._Src

        SystemInformationValues = {
            LOC("恒星数量"): System["NStars"],
            LOC("行星数量"): System["NPlanets"],
            LOC("矮行星数量"): System["NDwarfPlanets"],
            LOC("卫星数量"): System["NSatellites"],
            LOC("小行星数量"): System["NMinorPlanets"],
            LOC("彗星数量"): System["NComets"],
        }
        SystemSpectralType = " + ".join(System["StarSpectralType"])
        SystemInformationValues[LOC("光谱型")] = SystemSpectralType

        SystemInformationFrame = DataFrame([SystemInformationValues])
        VerticalLayout = SystemInformationFrame.iloc[0].to_frame(name=" ")
        return VerticalLayout.to_markdown()

    def _DFS_Iterate(self, Objs:dict, Ident:str, ParentBody:str):
        Store = True
        if Ident == ParentBody and self._Src["Objects"][Ident]["OType"] == "Barycenter":
            Store = False # 过滤掉根节点为质心
        if self._Src["Objects"][Ident]["OType"] == "DwarfMoon":
            Store = False # 过滤小卫星

        if Store:
            Objs[Ident] = self._Object_To_Table(self._Src["Objects"][Ident], ParentBody)

        if "SubSystems" in self._Src["Objects"][Ident]:
            for i in self._Src["Objects"][Ident]["SubSystems"]:
                self._DFS_Iterate(Objs, i, Ident)

    def ObjectList(self):
        Objs = {}
        self._DFS_Iterate(Objs, self._Src["MainID"], self._Src["MainID"])
        return Objs

    def MinorObjectList(self):
        return {}

def InitGenerator(fmt:str, args:dict, Src:dict):
    if fmt == "Markdown":
        return MarkdownGenerator(Src)
    else:
        raise ValueError(f"无效的格式：{fmt}")

def LoadObjectsFromSC(args):
    PrintArgs(args)

    global I18N, LOC # 我也没想到更好的办法了
    I18N = LoadLocale("Create", args.lcid);
    LOC = I18N.gettext

    Objects = InfoGen.InfoGen_Main(**vars(args))

    print("生成文件...")

    Gen = InitGenerator(args.format, args.format_args, Objects)
    Gen.Run()
    print(Gen.GetTexts())

    print("完成")