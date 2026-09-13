from InfoGen_Data import InfoGen
from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import LoadLocale
from InfoGen_Data import Classifications

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

class Generator(ABC):
    def __init__(self, System:dict):
        self._Src = System
        self._Texts = {}

    def _Subsystem_To_DataFrame(self, SubSystem:list, ParentBody:dict):
        RawData = []
        for i in SubSystem:
            Table = self._Src["Objects"][i]
            Row = {}
            Row[LOC("卫星名")] = i
            MeanDiameter = Table["PhysicalCharacteristics"]["MeanRadius"] * 2
            Dimensions = Table["PhysicalCharacteristics"]["Dimensions"]
            DimOutput = ""
            if Dimensions[0] == Dimensions[2]:
                DimOutput = " × ".join([f"{Dimensions[0]}", f"{Dimensions[1]}"])
            elif Dimensions[0] == Dimensions[2] and Dimensions[0] == Dimensions[1]:
                DimOutput = ""
            else:
                DimOutput = " × ".join([f"{i}" for i in Dimensions])
            DimOutput = " (" + DimOutput + ")" if DimOutput != "" else ""
            Row[LOC("直径")] = f"{MeanDiameter} m{DimOutput}"
            Row[LOC("质量")] = f"{Table["PhysicalCharacteristics"]["Mass"]} Kg"
            if ParentBody["OType"] == "Barycenter" and Table["OrbitalCharacteristics"]["BinaryOrbit"] and not Table["OrbitalCharacteristics"]["IsPrimary"]:
                Row[LOC("半长轴 (a)")] = f"{Table["OrbitalCharacteristics"]["BSemiMajorAxis"]} m"
                Row[LOC("周期 (P)")] = f"{Table["OrbitalCharacteristics"]["BPeriod"]} s"
                Row[LOC("轨道倾角 (i)")] = Table["OrbitalCharacteristics"]["BInclination"]
                Row[LOC("离心率 (e)")] = Table["OrbitalCharacteristics"]["BEccentricity"]
            else:
                Row[LOC("半长轴 (a)")] = f"{Table["OrbitalCharacteristics"]["SemiMajorAxis"]} m"
                Row[LOC("周期 (P)")] = f"{Table["OrbitalCharacteristics"]["Period"]} s"
                Row[LOC("轨道倾角 (i)")] = Table["OrbitalCharacteristics"]["Inclination"]
                Row[LOC("离心率 (e)")] = Table["OrbitalCharacteristics"]["Eccentricity"]
            RawData.append(Row)
        return DataFrame(RawData)

    def _Object_To_Table(self, Object:dict, ParentBody:dict):
        ObjectInfo = {}
        match Object["OType"] :
            case "Barycenter":
                ObjectInfo[LOC("物体类型")] = LOC("质心")
                # 质心默认情况下不输出任何数据，除非它是双星系统的伴星才会输出轨道
                if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"] and not Object["OrbitalCharacteristics"]["IsPrimary"]:
                    OrbitalCharacteristics = {}
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                    OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                    OrbitalCharacteristics[LOC("周期 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]} s"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["BEccentricity"]
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["BInclination"]
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["BAscendingNode"]
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]} JD"
                    OrbitalCharacteristics[LOC("近心点辐角 (ω)")] = Object["OrbitalCharacteristics"]["BArgOfPericenter"]
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["BMeanAnomaly"]
                    ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
            case "Star":
                OClass = Object["PhysicalCharacteristics"]["Class"]
                if OClass == "X" or OClass == "BlackHole":
                    ObjectInfo[LOC("物体类型")] = LOC(Classifications.RegexStellarClassification(OClass))
                    PhysicalCharacteristics = {}
                    PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]} Kg"
                    PhysicalCharacteristics[LOC("史瓦西半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]} m"
                    PhysicalCharacteristics[LOC("自旋")] = Object["PhysicalCharacteristics"]["KerrSpin"]
                    PhysicalCharacteristics[LOC("电荷量")] = Object["PhysicalCharacteristics"]["KerrCharge"]
                    ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                else:
                    ObjectInfo[LOC("物体类型")] = LOC(Classifications.RegexStellarClassification(OClass))
                    # 恒星默认情况下不输出轨道，除非它是双星系统的伴星
                    if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"] and not Object["OrbitalCharacteristics"]["IsPrimary"]:
                        OrbitalCharacteristics = {}
                        OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                        OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                        OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                        OrbitalCharacteristics[LOC("周期 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]} s"
                        OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]} m"
                        OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["BEccentricity"]
                        OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["BInclination"]
                        OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["BAscendingNode"]
                        OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]} JD"
                        OrbitalCharacteristics[LOC("近心点辐角 (ω)")] = Object["OrbitalCharacteristics"]["BArgOfPericenter"]
                        OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["BMeanAnomaly"]
                        ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                    PhysicalCharacteristics = {}
                    PhysicalCharacteristics[LOC("绝对热星等")] = Object["PhysicalCharacteristics"]["AbsMagnBol"]
                    PhysicalCharacteristics[LOC("光谱类型")] = Object["PhysicalCharacteristics"]["Class"]
                    PhysicalCharacteristics[LOC("黄赤交角")] = Object["PhysicalCharacteristics"]["AxialTilt"]
                    PhysicalCharacteristics[LOC("自转周期")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]} s"
                    PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]} m/s"
                    Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                    XRadius, ZRadius = Dimensions[0], Dimensions[2]
                    PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius} m"
                    Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                    PhysicalCharacteristics[LOC("扁率")] = Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]
                    PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]} m^2"
                    PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]} m^3"
                    PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]} Kg"
                    PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]} Kg/m^3"
                    PhysicalCharacteristics[LOC("年龄")] = f"{Object["PhysicalCharacteristics"]["Age"]} Yr"
                    PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]} m/s^2"
                    PhysicalCharacteristics[LOC("转动惯量")] = Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]
                    PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]} m/s"
                    PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]} °K"
                    PhysicalCharacteristics[LOC("光度")] = f"{Object["PhysicalCharacteristics"]["Luminosity"]} W"
                    ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
            case "Planet" | "DwarfPlanet":
                ObjectInfo[LOC("物体类型")] = LOC(Classifications.PlanetClassification(
                    Object["PhysicalCharacteristics"]["Class"],
                    Object["PhysicalCharacteristics"],
                    None if "Hydrosphere" not in Object.keys() else Object["Hydrosphere"]
                ))
                OrbitalCharacteristics = {}
                if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"] and not Object["OrbitalCharacteristics"]["IsPrimary"]:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                    OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                    OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["BAphelionDist"]} m"
                    OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["BPericenterDist"]} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["BEccentricity"]
                    OrbitalCharacteristics[LOC("恒星年 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["BMeanAnomaly"]
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["BInclination"]
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["BAscendingNode"]
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]} JD"
                    OrbitalCharacteristics[LOC("近心点辐角 (ω)")] = Object["OrbitalCharacteristics"]["BArgOfPericenter"]
                else:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("远日点")] = f"{Object["OrbitalCharacteristics"]["AphelionDist"]} m"
                    OrbitalCharacteristics[LOC("近日点")] = f"{Object["OrbitalCharacteristics"]["PericenterDist"]} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["SemiMajorAxis"]} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["Eccentricity"]
                    OrbitalCharacteristics[LOC("恒星年 (P)")] = f"{Object["OrbitalCharacteristics"]["Period"]} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["MeanAnomaly"]
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["Inclination"]
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["AscendingNode"]
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["Epoch"]} JD"
                    OrbitalCharacteristics[LOC("近日点辐角 (ω)")] = Object["OrbitalCharacteristics"]["ArgOfPericenter"]
                ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                PhysicalCharacteristics = {}
                PhysicalCharacteristics[LOC("平均半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]} m"
                Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                XRadius, ZRadius = Dimensions[0], Dimensions[2]
                PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius} m"
                PhysicalCharacteristics[LOC("极半径")] = f"{Dimensions[1]} m"
                Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                PhysicalCharacteristics[LOC("扁率")] = Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]
                PhysicalCharacteristics[LOC("赤道周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][0]} m"
                PhysicalCharacteristics[LOC("极周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][1]} m"
                PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]} m^2"
                PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]} m^3"
                PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]} Kg"
                PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]} Kg/m^3"
                PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]} m/s^2"
                PhysicalCharacteristics[LOC("转动惯量")] = Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]
                PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]} m/s"
                PhysicalCharacteristics[LOC("会合日")] = f"{Object["PhysicalCharacteristics"]["SynodicRotationPeriod"]} s"
                PhysicalCharacteristics[LOC("恒星日")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]} s"
                PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]} m/s"
                PhysicalCharacteristics[LOC("黄赤交角")] = Object["PhysicalCharacteristics"]["AxialTilt"]
                PhysicalCharacteristics[LOC("球面反照率")] = Object["PhysicalCharacteristics"]["Albedo"][0]
                PhysicalCharacteristics[LOC("几何反照率")] = Object["PhysicalCharacteristics"]["Albedo"][1]
                PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]} °K"
                ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                if "Atmosphere" in Object.keys():
                    Atmosphere = {}
                    Atmosphere[LOC("大气压强")] = f"{Object["Atmosphere"]["SurfacePressure"]} Pa"
                    Compositions = Object["Atmosphere"]["CompositionByVolume"]
                    Atmosphere[LOC("大气成分")] = "\n".join([f"{v}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("大气")] = DataFrame([Atmosphere])
                if "Hydrosphere" in Object.keys():
                    Ocean = {}
                    Ocean[LOC("海洋深度")] = f"{Object["Hydrosphere"]["Height"]} m"
                    Compositions = Object["Hydrosphere"]["CompositionByVolume"]
                    Ocean[LOC("海洋成分")] = "\n".join([f"{v}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("海洋")] = DataFrame([Ocean])
                if "Biosphere" in Object.keys():
                    Life = {}
                    Life[LOC("生物形式")] = LOC(Object["Biosphere"]["Class"])
                    Life[LOC("生物种类")] = LOC(Object["Biosphere"]["Type"])
                    Life[LOC("生物群系")] = ", ".join([LOC(i) for i in Object["Biosphere"]["Biome"]])
                    Life[LOC("生物圈")] = DataFrame([Life])
                if "SubSystems" in Object.keys():
                    ObjectInfo["卫星列表"] = self._Subsystem_To_DataFrame(Object["SubSystems"], ParentBody)
            case "Moon":
                ObjectInfo[LOC("物体类型")] = LOC(Classifications.PlanetClassification(
                    Object["PhysicalCharacteristics"]["Class"],
                    Object["PhysicalCharacteristics"],
                    None if "Hydrosphere" not in Object.keys() else Object["Hydrosphere"]
                ))
                OrbitalCharacteristics = {}
                if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"] and not Object["OrbitalCharacteristics"]["IsPrimary"]:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                    OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                    OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["BAphelionDist"]} m"
                    OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["BPericenterDist"]} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["BEccentricity"]
                    OrbitalCharacteristics[LOC("恒星月 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]} s"
                    OrbitalCharacteristics[LOC("朔望月 (P)")] = f"{Object["OrbitalCharacteristics"]["SynodicMonth"]} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["BMeanAnomaly"]
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["BInclination"]
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["BAscendingNode"]
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]} JD"
                    OrbitalCharacteristics[LOC("近地点辐角 (ω)")] = Object["OrbitalCharacteristics"]["BArgOfPericenter"]
                else:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["AphelionDist"]} m"
                    OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["PericenterDist"]} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["SemiMajorAxis"]} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = Object["OrbitalCharacteristics"]["Eccentricity"]
                    OrbitalCharacteristics[LOC("恒星月 (P)")] = f"{Object["OrbitalCharacteristics"]["Period"]} s"
                    OrbitalCharacteristics[LOC("朔望月 (P)")] = f"{Object["OrbitalCharacteristics"]["SynodicMonth"]} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = Object["OrbitalCharacteristics"]["MeanAnomaly"]
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = Object["OrbitalCharacteristics"]["Inclination"]
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = Object["OrbitalCharacteristics"]["AscendingNode"]
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["Epoch"]} JD"
                    OrbitalCharacteristics[LOC("近地点辐角 (ω)")] = Object["OrbitalCharacteristics"]["ArgOfPericenter"]
                ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                PhysicalCharacteristics = {}
                PhysicalCharacteristics[LOC("平均半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]} m"
                Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                XRadius, ZRadius = Dimensions[0], Dimensions[2]
                PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius} m"
                PhysicalCharacteristics[LOC("极半径")] = f"{Dimensions[1]} m"
                Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                PhysicalCharacteristics[LOC("扁率")] = Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]
                PhysicalCharacteristics[LOC("赤道周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][0]} m"
                PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]} m^2"
                PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]} m^3"
                PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]} Kg"
                PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]} Kg/m^3"
                PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]} m/s^2"
                PhysicalCharacteristics[LOC("转动惯量")] = Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]
                PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]} m/s"
                PhysicalCharacteristics[LOC("会合日")] = f"{Object["PhysicalCharacteristics"]["SynodicRotationPeriod"]} s"
                PhysicalCharacteristics[LOC("恒星日")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]} s"
                PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]} m/s"
                PhysicalCharacteristics[LOC("黄赤交角")] = Object["PhysicalCharacteristics"]["AxialTilt"]
                PhysicalCharacteristics[LOC("球面反照率")] = Object["PhysicalCharacteristics"]["Albedo"][0]
                PhysicalCharacteristics[LOC("几何反照率")] = Object["PhysicalCharacteristics"]["Albedo"][1]
                PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]} °K"
                ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                if "Atmosphere" in Object.keys():
                    Atmosphere = {}
                    Atmosphere[LOC("大气压强")] = f"{Object["Atmosphere"]["SurfacePressure"]} Pa"
                    Compositions = Object["Atmosphere"]["CompositionByVolume"]
                    Atmosphere[LOC("大气成分")] = "\n".join([f"{v}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("大气")] = DataFrame([Atmosphere])
                if "Hydrosphere" in Object.keys():
                    Ocean = {}
                    Ocean[LOC("海洋深度")] = f"{Object["Hydrosphere"]["Height"]} m"
                    Compositions = Object["Hydrosphere"]["CompositionByVolume"]
                    Ocean[LOC("海洋成分")] = "\n".join([f"{v}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("海洋")] = DataFrame([Ocean])
                if "Biosphere" in Object.keys():
                    Life = {}
                    Life[LOC("生物形式")] = LOC(Object["Biosphere"]["Class"])
                    Life[LOC("生物种类")] = LOC(Object["Biosphere"]["Type"])
                    Life[LOC("生物群系")] = ", ".join([LOC(i) for i in Object["Biosphere"]["Biome"]])
                    Life[LOC("生物圈")] = DataFrame([Life])
        return ObjectInfo

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

class MarkdownGenerator(Generator): # 大模型推荐使用，因为模型训练数据中Markdown格式占比很高，而且推理时相比JSON还有Html能省大概30%的Token
    def __init__(self, System:dict):
        super().__init__(System)

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
            ObjData = self._Object_To_Table(self._Src["Objects"][Ident], self._Src["Objects"][ParentBody])
            OrbitTable, PhysicalTable, AtmosphereTable, OceanTable, LifeTable, SubSystemTable = "", "", "", "", "", ""
            if LOC("轨道数据") in ObjData.keys():
                OrbitTable = f"#### {LOC("轨道数据")}\n\n"
                OrbitVerticalLayout = ObjData[LOC("轨道数据")].iloc[0].to_frame(name=" ")
                OrbitTable += OrbitVerticalLayout.to_markdown() + "\n\n"
            if LOC("物理数据") in ObjData.keys():
                PhysicalTable = f"#### {LOC("物理数据")}\n\n"
                PhysicalVerticalLayout = ObjData[LOC("物理数据")].iloc[0].to_frame(name=" ")
                PhysicalTable += PhysicalVerticalLayout.to_markdown() + "\n\n"
            if LOC("大气") in ObjData.keys():
                AtmosphereTable = f"#### {LOC("大气")}\n\n"
                AtmosphereVerticalLayout = ObjData[LOC("大气")].iloc[0].to_frame(name=" ")
                AtmosphereTable += AtmosphereVerticalLayout.to_markdown() + "\n\n"
            if LOC("海洋") in ObjData.keys():
                OceanTable = f"#### {LOC("海洋")}\n\n"
                OceanVerticalLayout = ObjData[LOC("海洋")].iloc[0].to_frame(name=" ")
                OceanTable += OceanVerticalLayout.to_markdown() + "\n\n"
            if LOC("生物圈") in ObjData.keys():
                LifeTable = f"#### {LOC("生物圈")}\n\n"
                LifeVerticalLayout = ObjData[LOC("生物圈")].iloc[0].to_frame(name=" ")
                LifeTable += LifeVerticalLayout.to_markdown() + "\n\n"
            if LOC("卫星列表") in ObjData.keys():
                SubSystemTable = f"#### {LOC("卫星列表")}\n\n"
                SubSystemTable += ObjData[LOC("卫星列表")].to_markdown() + "\n\n"
            Objs[Ident] = {"Type": ObjData[LOC("物体类型")], "Content": OrbitTable + PhysicalTable + AtmosphereTable + OceanTable + LifeTable + SubSystemTable}

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