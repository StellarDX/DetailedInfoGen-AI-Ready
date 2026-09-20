from InfoGen_Data import InfoGen
from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import LoadLocale
from InfoGen_Data import Classifications
from InfoGen_Data import ADBCClient
from InfoGen_Data import UserConfirm
from InfoGen_Data import CurrentSQLVariation

import argparse

from abc import ABC, abstractmethod
from pandas import DataFrame, read_sql, Timestamp
from pathlib import Path
from jinja2 import Environment, FileSystemLoader, select_autoescape
from uuid import uuid5, NAMESPACE_URL
from hashlib import sha256
from sqlalchemy import select, delete, insert, MetaData, Table, Column
from sqlalchemy import String, Double, Integer, Boolean, DateTime, BigInteger

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
    print(f"--namespace:                   {args.namespace}")
    print(f"--store-mode:                  {args.store_mode}")

class Uploader():
    @staticmethod
    def StrToRootNamespace(Namespace: str):
        return uuid5(NAMESPACE_URL, Namespace.strip())

    def __init__(self, System:dict, args):
        self._Src = System
        self._Args = args
        if self._Args.namespace == None or len(self._Args.namespace) == 0:
            raise ValueError("命名空间未填写或无效")
        self._TableSchema = {}
        self._SystemDataFrame = None
        self._ObjectsDataFrame = None
        self._IdentifiersDataFrame = None
        self._PhysicalDataFrame = None
        self._OrbitDataFrame = None
        self._AtmosphereDataFrame = None
        self._HydrosphereDataFrame = None
        self._BiosphereDataFrame = None
        self._BioBiomeDataFrame = None
        self._CompositionsDataFrame = None
        self._RootID = self.StrToRootNamespace(self._Args.namespace)

    def SystemID(self, main_id: str) -> str:
        return str(uuid5(self._RootID, f"{main_id}"))

    def ObjectID(self, main_id: str, path: list[str]) -> str:
        # path是从根到自己的Identifiers[0]链，例如["Solar System", "Sun", "Earth", "Moon"]
        return str(uuid5(self._RootID, f"{main_id}://{'/'.join(path)}")) # SE的物体名里不会出现'/'

    def FileSHA256(self, Source:str):
        return sha256(Path(Source).read_bytes()).hexdigest()

    def _Load_Table(self):
        Connection = ADBCClient()
        # 此处使用select * from table where 1 = 0获取表数据，多数数据库都支持且这种方法是多数ORM都在用的方法
        self._SystemDataFrame = read_sql("select * from ig_system where 1 = 0;", Connection)
        self._TableSchema["ig_system"] = self._SystemDataFrame.dtypes.to_dict()
        self._SystemDataFrame.set_index("system_id", inplace=True)
        self._ObjectsDataFrame = read_sql("select * from ig_object where 1 = 0;", Connection)
        self._TableSchema["ig_object"] = self._ObjectsDataFrame.dtypes.to_dict()
        self._ObjectsDataFrame.set_index("object_id", inplace=True)
        self._IdentifiersDataFrame = read_sql("select * from ig_identifiers where 1 = 0;", Connection)
        self._TableSchema["ig_identifiers"] = self._IdentifiersDataFrame.dtypes.to_dict()
        self._IdentifiersDataFrame.set_index(["object_id", "alias"], inplace=True)
        self._PhysicalDataFrame = read_sql("select * from ig_physical where 1 = 0;", Connection)
        self._TableSchema["ig_physical"] = self._PhysicalDataFrame.dtypes.to_dict()
        self._PhysicalDataFrame.set_index("object_id", inplace=True)
        self._OrbitDataFrame = read_sql("select * from ig_orbit where 1 = 0;", Connection)
        self._TableSchema["ig_orbit"] = self._OrbitDataFrame.dtypes.to_dict()
        self._OrbitDataFrame.set_index("object_id", inplace=True)
        self._AtmosphereDataFrame = read_sql("select * from ig_atmosphere where 1 = 0;", Connection)
        self._TableSchema["ig_atmosphere"] = self._AtmosphereDataFrame.dtypes.to_dict()
        self._AtmosphereDataFrame.set_index("object_id", inplace=True)
        self._HydrosphereDataFrame = read_sql("select * from ig_hydrosphere where 1 = 0;", Connection)
        self._TableSchema["ig_hydrosphere"] = self._HydrosphereDataFrame.dtypes.to_dict()
        self._HydrosphereDataFrame.set_index("object_id", inplace=True)
        self._BiosphereDataFrame = read_sql("select * from ig_biosphere where 1 = 0;", Connection)
        self._TableSchema["ig_biosphere"] = self._BiosphereDataFrame.dtypes.to_dict()
        self._BiosphereDataFrame.set_index("object_id", inplace=True)
        self._BioBiomeDataFrame = read_sql("select * from ig_biosphere_biome where 1 = 0;", Connection)
        self._TableSchema["ig_biosphere_biome"] = self._BioBiomeDataFrame.dtypes.to_dict()
        self._BioBiomeDataFrame.set_index(["object_id", "biome"], inplace=True)
        self._CompositionsDataFrame = read_sql("select * from ig_composition where 1 = 0;", Connection)
        self._TableSchema["ig_composition"] = self._CompositionsDataFrame.dtypes.to_dict()
        self._CompositionsDataFrame.set_index(["object_id", "kind", "component"], inplace=True)

    def _System_To_DataFrame(self):
        SystemID = self.SystemID(self._Src["MainID"])
        CurrentTime = Timestamp.now()
        self._SystemDataFrame.loc[SystemID] = [
            self._Src["MainID"],
            self._Args.input,
            self.FileSHA256(self._Args.input),
            self._Args.code_page,
            self._Args.esi_estimator,
            self._Args.absolute_orbit,
            self._Args.common_plane_threshold,
            self._Args.namespace,
            self._Src["NStars"],
            self._Src["NPlanets"],
            self._Src["NDwarfPlanets"],
            self._Src["NSatellites"],
            self._Src["NMinorPlanets"],
            self._Src["NComets"],
            " + ".join(self._Src["StarSpectralType"]),
            CurrentTime,
            CurrentTime
        ]
        return SystemID

    def _DFS_Iterate(self, Ident:str, ParentBody:str, SystemHash:str, ParentHash:str, CurrentStack:list, SubSysIndex:int):
        CurrentObject = self._Src["Objects"][Ident]
        CurrentHash = self.ObjectID(self._Src["MainID"], CurrentStack)
        ObjType = CurrentObject["OType"]

        self._ObjectsDataFrame.loc[CurrentHash] = [
            SystemHash,
            ParentHash,
            Ident,
            ObjType,
            CurrentObject["PhysicalCharacteristics"]["Class"] if "PhysicalCharacteristics" in CurrentObject else None,
            len(CurrentStack) - 1,
            SubSysIndex,
            ObjType in ["DwarfMoon", "Asteroid", "Comet"]
        ]

        for i in CurrentObject["Identifiers"]:
            self._IdentifiersDataFrame.loc[(CurrentHash, i), :] = None

        if "PhysicalCharacteristics" in CurrentObject:
            PhysicalCharacteristics = CurrentObject["PhysicalCharacteristics"]
            self._PhysicalDataFrame.loc[CurrentHash] = [
                PhysicalCharacteristics["AbsMagnBol"] if ObjType == "Star" else None,
                PhysicalCharacteristics["MeanRadius"],
                PhysicalCharacteristics["Dimensions"][0],
                PhysicalCharacteristics["Dimensions"][1],
                PhysicalCharacteristics["Dimensions"][2],
                PhysicalCharacteristics["Flattening"][0],
                PhysicalCharacteristics["Flattening"][1],
                PhysicalCharacteristics["Flattening"][2],
                PhysicalCharacteristics["Circumference"][0],
                PhysicalCharacteristics["Circumference"][1],
                PhysicalCharacteristics["SurfaceArea"],
                PhysicalCharacteristics["Volume"],
                PhysicalCharacteristics["Mass"],
                PhysicalCharacteristics["MeanDensity"],
                PhysicalCharacteristics["Age"] if ObjType == "Star" else None,
                PhysicalCharacteristics["SurfaceGravity"],
                PhysicalCharacteristics["MomentOfInertiaFactor"],
                PhysicalCharacteristics["EscapeVelocity"],
                PhysicalCharacteristics["SynodicRotationPeriod"] if ObjType not in ["Barycenter", "Star"] else None,
                PhysicalCharacteristics["SiderealRotationPeriod"],
                PhysicalCharacteristics["EquatorialRotationVelocity"],
                PhysicalCharacteristics["AxialTilt"],
                PhysicalCharacteristics["Albedo"][0] if ObjType not in ["Barycenter", "Star"] else None,
                PhysicalCharacteristics["Albedo"][1] if ObjType not in ["Barycenter", "Star"] else None,
                PhysicalCharacteristics["Luminosity"] if ObjType == "Star" else None,
                PhysicalCharacteristics["Temperature"],
                PhysicalCharacteristics["RadiantFlux"] if ObjType not in ["Barycenter", "Star"] else None,
                PhysicalCharacteristics["KerrSpin"] if ObjType == "Star" and (PhysicalCharacteristics["Class"] == "X" or PhysicalCharacteristics["Class"] == "BlackHole") else None,
                PhysicalCharacteristics["KerrCharge"] if ObjType == "Star" and (PhysicalCharacteristics["Class"] == "X" or PhysicalCharacteristics["Class"] == "BlackHole") else None,
                PhysicalCharacteristics["CometTotalMagn"] if ObjType == "Comet" else None,
                PhysicalCharacteristics["CometTotalMagnSlope"] if ObjType == "Comet" else None,
                PhysicalCharacteristics["ESI"] if ObjType in ["Planet", "Moon"] else None,
                ""
            ]

        if "OrbitalCharacteristics" in CurrentObject:
            OrbitalCharacteristics = CurrentObject["OrbitalCharacteristics"]
            OutputBinaryOrbit = (OrbitalCharacteristics["BinaryOrbit"] and not OrbitalCharacteristics["IsPrimary"]) if ObjType in ["Barycenter", "Star"] else (OrbitalCharacteristics["BinaryOrbit"])
            self._OrbitDataFrame.loc[CurrentHash] = [
                OrbitalCharacteristics["RefPlane"],
                OrbitalCharacteristics["Position"][0],
                OrbitalCharacteristics["Position"][1],
                OrbitalCharacteristics["Position"][2],
                OrbitalCharacteristics["Period"],
                OrbitalCharacteristics["PericenterDist"],
                OrbitalCharacteristics["AphelionDist"],
                OrbitalCharacteristics["SemiMajorAxis"],
                OrbitalCharacteristics["Eccentricity"],
                OrbitalCharacteristics["Inclination"],
                OrbitalCharacteristics["InclEcliptic"],
                OrbitalCharacteristics["AscendingNode"],
                OrbitalCharacteristics["AscNodeEcliptic"],
                OrbitalCharacteristics["Epoch"],
                OrbitalCharacteristics["ArgOfPericenter"],
                OrbitalCharacteristics["ArgOfPeriEcliptic"],
                OrbitalCharacteristics["MeanAnomaly"],
                OrbitalCharacteristics["SynodicMonth"] if ObjType == "Moon" or (ObjType == "Planet" and OrbitalCharacteristics["BinaryOrbit"] and not OrbitalCharacteristics["IsPrimary"]) else None,
                OrbitalCharacteristics["BinaryOrbit"],
                OrbitalCharacteristics["IsPrimary"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["Primary"] if OutputBinaryOrbit and not OrbitalCharacteristics["IsPrimary"] else None,
                OrbitalCharacteristics["Companion"] if OutputBinaryOrbit and not OrbitalCharacteristics["IsPrimary"] else None,
                OrbitalCharacteristics["BPeriod"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BPericenterDist"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BAphelionDist"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BSemiMajorAxis"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BEccentricity"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BInclination"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BInclinationEcliptic"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BAscendingNode"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BAscNodeEcliptic"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BEpoch"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BArgOfPericenter"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BArgOfPeriEcliptic"] if OutputBinaryOrbit else None,
                OrbitalCharacteristics["BMeanAnomaly"] if OutputBinaryOrbit else None,
            ]

        if "Atmosphere" in CurrentObject:
            Atmosphere = CurrentObject["Atmosphere"]
            self._AtmosphereDataFrame.loc[CurrentHash] = [
                Atmosphere["SurfacePressure"],
                Atmosphere["ScaleHeight"],
            ]

            for Material, Percent in Atmosphere["CompositionByVolume"].items():
                self._CompositionsDataFrame.loc[(CurrentHash, "Atmosphere", Material)] = [
                    Percent
                ]
        
        if "Hydrosphere" in CurrentObject:
            Hydrosphere = CurrentObject["Hydrosphere"]
            self._HydrosphereDataFrame.loc[CurrentHash] = [
                Hydrosphere["Height"]
            ]

            for Material, Percent in Hydrosphere["CompositionByVolume"].items():
                self._CompositionsDataFrame.loc[(CurrentHash, "Hydrosphere", Material)] = [
                    Percent
                ]
        
        if "Biosphere" in CurrentObject:
            Biosphere = CurrentObject["Biosphere"]
            self._BiosphereDataFrame.loc[CurrentHash] = [
                Biosphere["Class"],
                Biosphere["Type"]
            ]

            for i in Biosphere["Biome"]:
                self._BioBiomeDataFrame.loc[(CurrentHash, i), :] = None
        
        if "SubSystems" in CurrentObject:
            j = 0
            for i in CurrentObject["SubSystems"]:
                self._DFS_Iterate(i, Ident, SystemHash, CurrentHash, CurrentStack + [i], j)
                j += 1

    @staticmethod
    def SQLAlchemyType( # DataFrame的SQLTable里的类型映射函数（已被我改编）
        col_name: str, 
        table_dtype: pd.DataType, 
        schema_mapping: Optional[dict] = None,
        manual_mapping: Optional[dict] = None,
        tz_info: Optional[str] = None
    ) -> object:
        # 另外，据官方文档所说，DataFrame与PyArrow（ADBC）的对接还是实验性功能，用官方的推送功能无异于在地雷上蹦迪
        # 因此保守的办法还是自己弄一套逻辑
        if manual_mapping and isinstance(manual_mapping, dict):
            if col_name in manual_mapping:
                return manual_mapping[col_name]

        dtype = schema_mapping[col_name] if schema_mapping else table_dtype

        from sqlalchemy.types import (
            TIMESTAMP,
            BigInteger,
            Boolean,
            Date,
            DateTime,
            Float,
            Integer,
            SmallInteger,
            Text,
            Time
        )

        from pandas.api.types import (
            is_datetime64_any_dtype, 
            is_timedelta64_dtype, 
            is_bool_dtype, 
            is_float_dtype, 
            is_integer_dtype, 
            is_complex_dtype
        )

        if is_datetime64_any_dtype(dtype):
            # 如果有显式传入的 tz_info 或者 dtype 本身包含 tz
            has_tz = tz_info is not None
            if not has_tz:
                # 检查 type是否本身就是带时区的 (例如datetime64[ns, UTC])
                if hasattr(dtype, 'tz') and dtype.tz is not None:
                    has_tz = True
            if has_tz:
                return TIMESTAMP(timezone=True)
            else:
                return DateTime

        if is_timedelta64_dtype(dtype):
            print(f"列\"{col_name}\"：大多数数据库产品不原生支持timedelta类型，该类型将被映射为BigInteger（纳秒）。")
            return BigInteger

        if is_float_dtype(dtype):
            # float32 -> Float(precision=23)
            # float64 -> Float(precision=53)
            if dtype == 'float32':
                return Float(precision = 23) 
            else:
                # 默认包括 float64 和其他浮点类型
                return Float(precision = 53)

        if is_integer_dtype(dtype):
            dtype_name = dtype.name.lower()
            # 映射整数类型到最佳SQLAlchemy整数类型
            if dtype_name in ("int8", "uint8", "int16"):
                return SmallInteger
            elif dtype_name in ("uint16", "int32"):
                return Integer
            elif dtype_name == "uint64":
                raise ValueError(f"列\"{col_name}\"：SQLAlchemy整数类型不支持无符号64位整数（uint64）。")
            else:
                # 包括int64, int128(如果存在), uint32等
                return BigInteger

        if is_bool_dtype(dtype):
            return Boolean

        if is_complex_dtype(dtype):
            raise ValueError(f"列\"{col_name}\"：SQLAlchemy不支持复数类型")

        return Text

    @staticmethod
    def DataFrameToSQL(TblName:str, Data:DataFrame, SchemaMapping:dict = None, ManualMapping:dict = None):
        # 导出所有列名和数据类型
        FullTable = Data.reset_index()
        ColNames = FullTable.columns
        DTypes = [Uploader.SQLAlchemyType(i, j, schema_mapping = SchemaMapping, manual_mapping = ManualMapping) 
            for i, j in zip(ColNames, FullTable.dtypes)]
        AlchemyCols = [Column(i, j) for i, j in zip(ColNames, DTypes)]
        AlchemyTable = Table(TblName, MetaData(), *AlchemyCols)
        Statement = insert(AlchemyTable).values(FullTable.to_dict(orient = "records"))
        return str(Statement.compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))

    def DistUpgrade(self, SysID):
        Connection = ADBCClient()

        ExistingSystemTable = Table("ig_system", MetaData(), 
            Column('system_id', String),
            Column('main_id', String),
            Column('namespace', String),
            Column('create_date', DateTime)
        )
        ExistingSystemQuery = str((select(ExistingSystemTable)
            .where(ExistingSystemTable.c.namespace == self._Args.namespace)
            .where(ExistingSystemTable.c.main_id == self._Src["MainID"])
        ).compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        ExistingSystem = read_sql(ExistingSystemQuery, Connection)
        ExistingSystem.set_index("system_id", inplace = True)

        Upload = True if len(ExistingSystem) == 0 else UserConfirm(f"当前命名空间已存在系统\"{self._Src["MainID"]}\"，是否更新？")

        if Upload:
            if len(ExistingSystem) != 0:
                self._SystemDataFrame.loc[SysID, "create_date"] = ExistingSystem.at[SysID, "create_date"]
                with Connection.cursor() as Cursor:
                    DeleteTable = Table("ig_system", MetaData(), Column('system_id', String))
                    DeleteQuery = str(delete(DeleteTable).where(DeleteTable.c.system_id == SysID)
                        .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
                    Cursor.execute(DeleteQuery)
                Connection.commit()
            
            InsertStatements = []
            if (len(self._SystemDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_system", self._SystemDataFrame, self._TableSchema["ig_system"]))
            if (len(self._ObjectsDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_object", self._ObjectsDataFrame, self._TableSchema["ig_object"]))
            if (len(self._IdentifiersDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_identifiers", self._IdentifiersDataFrame, self._TableSchema["ig_identifiers"]))
            if (len(self._PhysicalDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_physical", self._PhysicalDataFrame, self._TableSchema["ig_physical"]))
            if (len(self._OrbitDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_orbit", self._OrbitDataFrame, self._TableSchema["ig_orbit"]))
            if (len(self._AtmosphereDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_atmosphere", self._AtmosphereDataFrame, self._TableSchema["ig_atmosphere"]))
            if (len(self._HydrosphereDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_hydrosphere", self._HydrosphereDataFrame, self._TableSchema["ig_hydrosphere"]))
            if (len(self._BiosphereDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_biosphere", self._BiosphereDataFrame, self._TableSchema["ig_biosphere"]))
            if (len(self._BioBiomeDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_biosphere_biome", self._BioBiomeDataFrame, self._TableSchema["ig_biosphere_biome"]))
            if (len(self._CompositionsDataFrame) != 0):
                InsertStatements.append(self.DataFrameToSQL("ig_composition", self._CompositionsDataFrame, self._TableSchema["ig_composition"]))
        
            with Connection.cursor() as Cursor:
                for i in InsertStatements:
                    Cursor.execute(i)
            Connection.commit()

    def Run(self):
        # 先准备表
        self._Load_Table()
        SystemID = self._System_To_DataFrame()
        self._DFS_Iterate(self._Src["MainID"], None, SystemID, None, [self._Src["MainID"]], None)
        match self._Args.store_mode:
            case "dist-upgrade":
                self.DistUpgrade(SystemID)
            case _:
                raise ValueError("无效的上传模式")

class Generator(ABC):
    def __init__(self, System:dict, kwargs:dict):
        self._Src = System
        self._Texts = {}
        self._Prec:int = int(kwargs["precision"]) if "precision" in kwargs.keys() and int(kwargs["precision"]) > 0 else 12

    def _System_Info_To_Table(self):
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
        return DataFrame([SystemInformationValues])

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
                DimOutput = " × ".join([f"{Dimensions[0]:.{self._Prec}g}", f"{Dimensions[1]:.{self._Prec}g}"])
            elif Dimensions[0] == Dimensions[2] and Dimensions[0] == Dimensions[1]:
                DimOutput = ""
            else:
                DimOutput = " × ".join([f"{i:.{self._Prec}g}" for i in Dimensions])
            DimOutput = " (" + DimOutput + ")" if DimOutput != "" else ""
            Row[LOC("直径")] = f"{MeanDiameter:.{self._Prec}g} m{DimOutput}"
            Row[LOC("质量")] = f"{Table["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
            if ParentBody["OType"] == "Barycenter" and Table["OrbitalCharacteristics"]["BinaryOrbit"] and not Table["OrbitalCharacteristics"]["IsPrimary"]:
                Row[LOC("半长轴 (a)")] = f"{Table["OrbitalCharacteristics"]["BSemiMajorAxis"]:.{self._Prec}g} m"
                Row[LOC("周期 (P)")] = f"{Table["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                Row[LOC("轨道倾角 (i)")] = f"{Table["OrbitalCharacteristics"]["BInclination"]:.{self._Prec}g}"
                Row[LOC("离心率 (e)")] = f"{Table["OrbitalCharacteristics"]["BEccentricity"]:.{self._Prec}g}"
            else:
                Row[LOC("半长轴 (a)")] = f"{Table["OrbitalCharacteristics"]["SemiMajorAxis"]:.{self._Prec}g} m"
                Row[LOC("周期 (P)")] = f"{Table["OrbitalCharacteristics"]["Period"]:.{self._Prec}g} s"
                Row[LOC("轨道倾角 (i)")] = f"{Table["OrbitalCharacteristics"]["Inclination"]:.{self._Prec}g}"
                Row[LOC("离心率 (e)")] = f"{Table["OrbitalCharacteristics"]["Eccentricity"]:.{self._Prec}g}"
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
                    OrbitalCharacteristics[LOC("周期 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["BEccentricity"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["BInclination"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["BAscendingNode"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]:.{self._Prec}g} JD"
                    OrbitalCharacteristics[LOC("近心点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["BArgOfPericenter"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["BMeanAnomaly"]:.{self._Prec}g}"
                    ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
            case "Star":
                OClass = Object["PhysicalCharacteristics"]["Class"]
                if OClass == "X" or OClass == "BlackHole":
                    ObjectInfo[LOC("物体类型")] = LOC(Classifications.RegexStellarClassification(OClass))
                    PhysicalCharacteristics = {}
                    PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
                    PhysicalCharacteristics[LOC("史瓦西半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]:.{self._Prec}g} m"
                    PhysicalCharacteristics[LOC("自旋")] = f"{Object["PhysicalCharacteristics"]["KerrSpin"]:.{self._Prec}g}"
                    PhysicalCharacteristics[LOC("电荷量")] = f"{Object["PhysicalCharacteristics"]["KerrCharge"]:.{self._Prec}g}"
                    ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                else:
                    ObjectInfo[LOC("物体类型")] = LOC(Classifications.RegexStellarClassification(OClass))
                    # 恒星默认情况下不输出轨道，除非它是双星系统的伴星
                    if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"] and not Object["OrbitalCharacteristics"]["IsPrimary"]:
                        OrbitalCharacteristics = {}
                        OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                        OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                        OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                        OrbitalCharacteristics[LOC("周期 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                        OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]:.{self._Prec}g} m"
                        OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["BEccentricity"]:.{self._Prec}g}"
                        OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["BInclination"]:.{self._Prec}g}"
                        OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["BAscendingNode"]:.{self._Prec}g}"
                        OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]:.{self._Prec}g} JD"
                        OrbitalCharacteristics[LOC("近心点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["BArgOfPericenter"]:.{self._Prec}g}"
                        OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["BMeanAnomaly"]:.{self._Prec}g}"
                        ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                    PhysicalCharacteristics = {}
                    PhysicalCharacteristics[LOC("绝对热星等")] = f"{Object["PhysicalCharacteristics"]["AbsMagnBol"]:.{self._Prec}g}"
                    PhysicalCharacteristics[LOC("光谱类型")] = Object["PhysicalCharacteristics"]["Class"]
                    PhysicalCharacteristics[LOC("黄赤交角")] = f"{Object["PhysicalCharacteristics"]["AxialTilt"]:.{self._Prec}g}"
                    PhysicalCharacteristics[LOC("自转周期")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]:.{self._Prec}g} s"
                    PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]:.{self._Prec}g} m/s"
                    Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                    XRadius, ZRadius = Dimensions[0], Dimensions[2]
                    PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius:.{self._Prec}g} m"
                    Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                    PhysicalCharacteristics[LOC("扁率")] = f"{Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]:.{self._Prec}g}"
                    PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]:.{self._Prec}g} m^2"
                    PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]:.{self._Prec}g} m^3"
                    PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
                    PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]:.{self._Prec}g} Kg/m^3"
                    PhysicalCharacteristics[LOC("年龄")] = f"{Object["PhysicalCharacteristics"]["Age"]:.{self._Prec}g} Yr"
                    PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]:.{self._Prec}g} m/s^2"
                    PhysicalCharacteristics[LOC("转动惯量")] = f"{Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]:.{self._Prec}g}"
                    PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]:.{self._Prec}g} m/s"
                    PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]:.{self._Prec}g} °K"
                    PhysicalCharacteristics[LOC("光度")] = f"{Object["PhysicalCharacteristics"]["Luminosity"]:.{self._Prec}g} W"
                    ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
            case "Planet" | "DwarfPlanet":
                ObjectInfo[LOC("物体类型")] = LOC(Classifications.PlanetClassification(
                    Object["PhysicalCharacteristics"]["Class"],
                    Object["PhysicalCharacteristics"],
                    None if "Hydrosphere" not in Object.keys() else Object["Hydrosphere"]
                ))
                OrbitalCharacteristics = {}
                if ParentBody["OType"] == "Barycenter" and Object["OrbitalCharacteristics"]["BinaryOrbit"]:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    if not Object["OrbitalCharacteristics"]["IsPrimary"]:
                        OrbitalCharacteristics[LOC("主星")] = Object["OrbitalCharacteristics"]["Primary"]
                        OrbitalCharacteristics[LOC("伴星")] = Object["OrbitalCharacteristics"]["Companion"]
                        OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["BAphelionDist"]:.{self._Prec}g} m"
                        OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["BPericenterDist"]:.{self._Prec}g} m"
                    else:
                        OrbitalCharacteristics[LOC("远日点")] = f"{Object["OrbitalCharacteristics"]["BAphelionDist"]:.{self._Prec}g} m"
                        OrbitalCharacteristics[LOC("近日点")] = f"{Object["OrbitalCharacteristics"]["BPericenterDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["BEccentricity"]:.{self._Prec}g}"
                    if Object["OrbitalCharacteristics"]["IsPrimary"]:
                        OrbitalCharacteristics[LOC("恒星年 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                    else:
                        OrbitalCharacteristics[LOC("恒星月 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                        OrbitalCharacteristics[LOC("朔望月 (P)")] = f"{Object["OrbitalCharacteristics"]["SynodicMonth"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["BMeanAnomaly"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["BInclination"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["BAscendingNode"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]:.{self._Prec}g} JD"
                    if Object["OrbitalCharacteristics"]["IsPrimary"]:
                        OrbitalCharacteristics[LOC("近日点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["BArgOfPericenter"]:.{self._Prec}g}"
                    else:
                        OrbitalCharacteristics[LOC("近地点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["BArgOfPericenter"]:.{self._Prec}g}"
                else:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("远日点")] = f"{Object["OrbitalCharacteristics"]["AphelionDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("近日点")] = f"{Object["OrbitalCharacteristics"]["PericenterDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["SemiMajorAxis"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["Eccentricity"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("恒星年 (P)")] = f"{Object["OrbitalCharacteristics"]["Period"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["MeanAnomaly"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["Inclination"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["AscendingNode"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["Epoch"]:.{self._Prec}g} JD"
                    OrbitalCharacteristics[LOC("近日点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["ArgOfPericenter"]:.{self._Prec}g}"
                ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                PhysicalCharacteristics = {}
                PhysicalCharacteristics[LOC("平均半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]:.{self._Prec}g} m"
                Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                XRadius, ZRadius = Dimensions[0], Dimensions[2]
                PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius:.{self._Prec}g} m"
                PhysicalCharacteristics[LOC("极半径")] = f"{Dimensions[1]:.{self._Prec}g} m"
                Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                PhysicalCharacteristics[LOC("扁率")] = f"{Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("赤道周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][0]:.{self._Prec}g} m"
                PhysicalCharacteristics[LOC("极周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][1]:.{self._Prec}g} m"
                PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]:.{self._Prec}g} m^2"
                PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]:.{self._Prec}g} m^3"
                PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
                PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]:.{self._Prec}g} Kg/m^3"
                PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]:.{self._Prec}g} m/s^2"
                PhysicalCharacteristics[LOC("转动惯量")] = f"{Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]:.{self._Prec}g} m/s"
                PhysicalCharacteristics[LOC("会合日")] = f"{Object["PhysicalCharacteristics"]["SynodicRotationPeriod"]:.{self._Prec}g} s"
                PhysicalCharacteristics[LOC("恒星日")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]:.{self._Prec}g} s"
                PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]:.{self._Prec}g} m/s"
                PhysicalCharacteristics[LOC("黄赤交角")] = f"{Object["PhysicalCharacteristics"]["AxialTilt"]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("球面反照率")] = f"{Object["PhysicalCharacteristics"]["Albedo"][0]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("几何反照率")] = f"{Object["PhysicalCharacteristics"]["Albedo"][1]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]:.{self._Prec}g} °K"
                ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                if "Atmosphere" in Object.keys():
                    Atmosphere = {}
                    Atmosphere[LOC("大气压强")] = f"{Object["Atmosphere"]["SurfacePressure"]:.{self._Prec}g} Pa"
                    Compositions = Object["Atmosphere"]["CompositionByVolume"]
                    Atmosphere[LOC("大气成分")] = "\n".join([f"{v:.{self._Prec}g}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("大气")] = DataFrame([Atmosphere])
                if "Hydrosphere" in Object.keys():
                    Ocean = {}
                    Ocean[LOC("海洋深度")] = f"{Object["Hydrosphere"]["Height"]:.{self._Prec}g} m"
                    Compositions = Object["Hydrosphere"]["CompositionByVolume"]
                    Ocean[LOC("海洋成分")] = "\n".join([f"{v:.{self._Prec}g}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("海洋")] = DataFrame([Ocean])
                if "Biosphere" in Object.keys():
                    Life = {}
                    Life[LOC("生物形式")] = LOC(Object["Biosphere"]["Class"])
                    Life[LOC("生物种类")] = LOC(Object["Biosphere"]["Type"])
                    Life[LOC("生物群系")] = ", ".join([LOC(i) for i in Object["Biosphere"]["Biome"]])
                    ObjectInfo[LOC("生物圈")] = DataFrame([Life])
                if "SubSystems" in Object.keys():
                    ObjectInfo[LOC("卫星列表")] = self._Subsystem_To_DataFrame(Object["SubSystems"], ParentBody)
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
                    OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["BAphelionDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["BPericenterDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["BSemiMajorAxis"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["BEccentricity"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("恒星月 (P)")] = f"{Object["OrbitalCharacteristics"]["BPeriod"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("朔望月 (P)")] = f"{Object["OrbitalCharacteristics"]["SynodicMonth"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["BMeanAnomaly"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["BInclination"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["BAscendingNode"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["BEpoch"]:.{self._Prec}g} JD"
                    OrbitalCharacteristics[LOC("近地点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["BArgOfPericenter"]:.{self._Prec}g}"
                else:
                    OrbitalCharacteristics[LOC("参考系统")] = LOC(Object["OrbitalCharacteristics"]["RefPlane"])
                    OrbitalCharacteristics[LOC("远地点")] = f"{Object["OrbitalCharacteristics"]["AphelionDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("近地点")] = f"{Object["OrbitalCharacteristics"]["PericenterDist"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("半长轴 (a)")] = f"{Object["OrbitalCharacteristics"]["SemiMajorAxis"]:.{self._Prec}g} m"
                    OrbitalCharacteristics[LOC("离心率 (e)")] = f"{Object["OrbitalCharacteristics"]["Eccentricity"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("恒星月 (P)")] = f"{Object["OrbitalCharacteristics"]["Period"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("朔望月 (P)")] = f"{Object["OrbitalCharacteristics"]["SynodicMonth"]:.{self._Prec}g} s"
                    OrbitalCharacteristics[LOC("平近点角 (M0)")] = f"{Object["OrbitalCharacteristics"]["MeanAnomaly"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("轨道倾角 (i)")] = f"{Object["OrbitalCharacteristics"]["Inclination"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("升交点经度 (Ω)")] = f"{Object["OrbitalCharacteristics"]["AscendingNode"]:.{self._Prec}g}"
                    OrbitalCharacteristics[LOC("历元 (T)")] = f"{Object["OrbitalCharacteristics"]["Epoch"]:.{self._Prec}g} JD"
                    OrbitalCharacteristics[LOC("近地点辐角 (ω)")] = f"{Object["OrbitalCharacteristics"]["ArgOfPericenter"]:.{self._Prec}g}"
                ObjectInfo[LOC("轨道数据")] = DataFrame([OrbitalCharacteristics])
                PhysicalCharacteristics = {}
                PhysicalCharacteristics[LOC("平均半径")] = f"{Object["PhysicalCharacteristics"]["MeanRadius"]:.{self._Prec}g} m"
                Dimensions = Object["PhysicalCharacteristics"]["Dimensions"] / 2.
                XRadius, ZRadius = Dimensions[0], Dimensions[2]
                PhysicalCharacteristics[LOC("赤道半径")] = f"{XRadius if max(XRadius, ZRadius) == XRadius else ZRadius:.{self._Prec}g} m"
                PhysicalCharacteristics[LOC("极半径")] = f"{Dimensions[1]:.{self._Prec}g} m"
                Flattening = Object["PhysicalCharacteristics"]["Flattening"]
                PhysicalCharacteristics[LOC("扁率")] = Flattening if Flattening[0] != 0 or Flattening[2] != 0 else Flattening[1]
                PhysicalCharacteristics[LOC("赤道周长")] = f"{Object["PhysicalCharacteristics"]["Circumference"][0]:.{self._Prec}g} m"
                PhysicalCharacteristics[LOC("表面积")] = f"{Object["PhysicalCharacteristics"]["SurfaceArea"]:.{self._Prec}g} m^2"
                PhysicalCharacteristics[LOC("体积")] = f"{Object["PhysicalCharacteristics"]["Volume"]:.{self._Prec}g} m^3"
                PhysicalCharacteristics[LOC("质量")] = f"{Object["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
                PhysicalCharacteristics[LOC("平均密度")] = f"{Object["PhysicalCharacteristics"]["MeanDensity"]:.{self._Prec}g} Kg/m^3"
                PhysicalCharacteristics[LOC("表面重力")] = f"{Object["PhysicalCharacteristics"]["SurfaceGravity"]:.{self._Prec}g} m/s^2"
                PhysicalCharacteristics[LOC("转动惯量")] = f"{Object["PhysicalCharacteristics"]["MomentOfInertiaFactor"]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("逃逸速度")] = f"{Object["PhysicalCharacteristics"]["EscapeVelocity"]:.{self._Prec}g} m/s"
                PhysicalCharacteristics[LOC("会合日")] = f"{Object["PhysicalCharacteristics"]["SynodicRotationPeriod"]:.{self._Prec}g} s"
                PhysicalCharacteristics[LOC("恒星日")] = f"{Object["PhysicalCharacteristics"]["SiderealRotationPeriod"]:.{self._Prec}g} s"
                PhysicalCharacteristics[LOC("自转线速度")] = f"{Object["PhysicalCharacteristics"]["EquatorialRotationVelocity"]:.{self._Prec}g} m/s"
                PhysicalCharacteristics[LOC("黄赤交角")] = f"{Object["PhysicalCharacteristics"]["AxialTilt"]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("球面反照率")] = f"{Object["PhysicalCharacteristics"]["Albedo"][0]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("几何反照率")] = f"{Object["PhysicalCharacteristics"]["Albedo"][1]:.{self._Prec}g}"
                PhysicalCharacteristics[LOC("温度")] = f"{Object["PhysicalCharacteristics"]["Temperature"]:.{self._Prec}g} °K"
                ObjectInfo[LOC("物理数据")] = DataFrame([PhysicalCharacteristics])
                if "Atmosphere" in Object.keys():
                    Atmosphere = {}
                    Atmosphere[LOC("大气压强")] = f"{Object["Atmosphere"]["SurfacePressure"]:.{self._Prec}g} Pa"
                    Compositions = Object["Atmosphere"]["CompositionByVolume"]
                    Atmosphere[LOC("大气成分")] = "\n".join([f"{v:.{self._Prec}g}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("大气")] = DataFrame([Atmosphere])
                if "Hydrosphere" in Object.keys():
                    Ocean = {}
                    Ocean[LOC("海洋深度")] = f"{Object["Hydrosphere"]["Height"]:.{self._Prec}g} m"
                    Compositions = Object["Hydrosphere"]["CompositionByVolume"]
                    Ocean[LOC("海洋成分")] = "\n".join([f"{v:.{self._Prec}g}% {k}" for k, v in Compositions.items()])
                    ObjectInfo[LOC("海洋")] = DataFrame([Ocean])
                if "Biosphere" in Object.keys():
                    Life = {}
                    Life[LOC("生物形式")] = LOC(Object["Biosphere"]["Class"])
                    Life[LOC("生物种类")] = LOC(Object["Biosphere"]["Type"])
                    Life[LOC("生物群系")] = ", ".join([LOC(i) for i in Object["Biosphere"]["Biome"]])
                    ObjectInfo[LOC("生物圈")] = DataFrame([Life])
        return ObjectInfo

    def _Minor_Object_List_Has_Comet(self):
        for Dummy, Data in self._Src["MinorObjects"].items():
            if Data["OType"] == "Comet":
                return True
        return False

    def _Minor_Objects_To_Table(self):
        MinorObjects = []
        HasComet = self._Minor_Object_List_Has_Comet()
        for Ident, Data in self._Src["MinorObjects"].items():
            Row = {}
            Row[LOC("小行星/彗星")] = Ident
            Row[LOC("平均直径")] = f"{Data["PhysicalCharacteristics"]["MeanRadius"] * 2:.{self._Prec}g} m"
            Dimensions = Data["PhysicalCharacteristics"]["Dimensions"]
            DimOutput = ""
            if Dimensions[0] == Dimensions[2]:
                DimOutput = " × ".join([f"{Dimensions[0]:.{self._Prec}g}", f"{Dimensions[1]:.{self._Prec}g}"])
            elif Dimensions[0] == Dimensions[2] and Dimensions[0] == Dimensions[1]:
                DimOutput = ""
            else:
                DimOutput = " × ".join([f"{i:.{self._Prec}g}" for i in Dimensions])
            DimOutput = "(" + DimOutput + ")" if DimOutput != "" else ""
            Row[LOC("直径")] = f"{DimOutput}"
            Row[LOC("质量")] = f"{Data["PhysicalCharacteristics"]["Mass"]:.{self._Prec}g} Kg"
            Row[LOC("恒星日")] = f"{Data["PhysicalCharacteristics"]["SiderealRotationPeriod"]:.{self._Prec}g} s"
            if HasComet:
                if Data["OType"] == "Comet":
                    Row[LOC("Comet total magnitude slope (K1)")] = f"{Data["PhysicalCharacteristics"]["CometTotalMagnSlope"]:.{self._Prec}g}"
                    Row[LOC("彗星绝对星等 (M1)")] = f"{Data["PhysicalCharacteristics"]["CometTotalMagn"]}"
                else:
                    Row[LOC("Comet total magnitude slope (K1)")] = ""
                    Row[LOC("彗星绝对星等 (M1)")] = ""
            Row[LOC("半长轴 (a)")] = f"{Data["OrbitalCharacteristics"]["SemiMajorAxis"]:.{self._Prec}g} m"
            Row[LOC("轨道倾角 (i)")] = f"{Data["OrbitalCharacteristics"]["Inclination"]:.{self._Prec}g}"
            Row[LOC("物体类型")] = LOC(Data["OType"])
            MinorObjects.append(Row)
        return DataFrame(MinorObjects)

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

    @abstractmethod
    def FileSuffix(self):
        pass

    @abstractmethod
    def ExportTextToString(self):
        pass

class MarkdownGenerator(Generator): # 大模型推荐使用，因为模型训练数据中Markdown格式占比很高，而且推理时相比JSON还有Html能省大概30%的Token
    def __init__(self, System:dict, kwargs:dict):
        super().__init__(System, kwargs)

    def SystemInfo(self):
        SystemInformationFrame = self._System_Info_To_Table()
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
        return self._Minor_Objects_To_Table().to_markdown()

    def FileSuffix(self):
        return "md"

    def ExportTextToString(self):
        Text = ""
        Text += f"# {self._Src["MainID"]}\n\n"
        Contents = self._Texts[self._Src["MainID"]]
        Text += f"## {LOC("行星系统信息")}\n\n"
        Text += f"{Contents[LOC("行星系统信息")]}\n\n"
        Text += f"## {LOC("物体列表")}\n\n"
        for Ident, Data in Contents[LOC("物体列表")].items():
            Text += f"### {Ident} - {Data["Type"]}\n\n"
            Text += f"{Data["Content"]}\n\n"
        Text += f"## {LOC("小行星列表")}\n\n"
        Text += f"{Contents[LOC("小行星列表")]}"
        return Text

class Jinja2Generator(Generator):
    def __init__(self, System:dict, kwargs:dict):
        super().__init__(System, kwargs)
        self.TemplatePath = self.CanonicalTemplatePath(kwargs["template-path"] if "template-path" in kwargs.keys() else "Default")

    @staticmethod
    def CanonicalTemplatePath(TPath: str) -> Path:
        Root = Path("./InfoGen_Data/StreamingAssets/Templates")
        Dir = Path(TPath) if ("/" in TPath or "\\" in TPath) else Root / TPath
        if not (Dir.exists() and Dir.is_dir()):
            raise FileNotFoundError(f"未能获取到有效模板目录：{Dir}，没有那个文件或目录或权限不够")
        return Dir

    @staticmethod
    def _Fields(Frame: DataFrame) -> dict:
        if len(Frame) == 0:
            return {}
        return {str(k): v for k, v in Frame.iloc[0].to_dict().items()}

    @staticmethod
    def _Rows(Frame: DataFrame):
        Columns = [str(c) for c in Frame.columns]
        Rows = [{str(k): v for k, v in Row.items()} for Row in Frame.to_dict(orient = "records")]
        return Columns, Rows

    def SystemInfo(self):
        return self._Fields(self._System_Info_To_Table())

    def _Object_Sections(self, ObjData: dict) -> list:
        Sections = []
        for Title, Data in ObjData.items():
            if Title == LOC("物体类型") or not isinstance(Data, DataFrame):
                continue
            if len(Data) == 1:
                Sections.append({"Title": Title, "Fields": self._Fields(Data)})
            else:
                Columns, Rows = self._Rows(Data)
                Sections.append({"Title": Title, "Columns": Columns, "Rows": Rows})
        return Sections

    def _DFS_Iterate(self, Objs: list, Ident: str, ParentBody: str, Depth: int = 0):
        Sub = self._Src["Objects"][Ident]
        Store = not (Ident == ParentBody and Sub["OType"] == "Barycenter")
        Store = Store and Sub["OType"] != "DwarfMoon"   # 与 Markdown 侧过滤规则一致
        if Store:
            ObjData = self._Object_To_Table(Sub, self._Src["Objects"][ParentBody])
            Objs.append({
                "Ident": Ident,
                "Type": ObjData.get(LOC("物体类型"), ""),
                "Depth": Depth,
                "Sections": self._Object_Sections(ObjData),
            })
        for i in Sub.get("SubSystems", []):
            self._DFS_Iterate(Objs, i, Ident, Depth + 1)

    def ObjectList(self):
        Objs = []
        self._DFS_Iterate(Objs, self._Src["MainID"], self._Src["MainID"])
        return Objs

    def MinorObjectList(self):
        Columns, Rows = self._Rows(self._Minor_Objects_To_Table())
        return {"Columns": Columns, "Rows": Rows}

    def FileSuffix(self):
        return "Html"

    def ExportTextToString(self):
        Contents = self._Texts[self._Src["MainID"]]
        Env = Environment(
            loader = FileSystemLoader(str(self.TemplatePath.parent)),
            autoescape = select_autoescape(["html", "xml"]),
            trim_blocks = True, lstrip_blocks = True, keep_trailing_newline = True,
        )
        Env.globals["LOC"] = LOC   # 让模板也能走 i18n
        Template = Env.get_template(f"{self.TemplatePath.name}/index.html")
        return Template.render(
            MainID       = self._Src["MainID"],
            Title        = self._Src["MainID"],
            SystemInfo   = Contents[LOC("行星系统信息")],
            Objects      = Contents[LOC("物体列表")],
            MinorObjects = Contents[LOC("小行星列表")],
        )

def InitGenerator(fmt:str, args:dict, Src:dict):
    if fmt == "Markdown":
        return MarkdownGenerator(Src, args)
    elif fmt in ("Jinja2", "HTML"):
        return Jinja2Generator(Src, args)
    else:
        raise ValueError(f"无效的格式：{fmt}")

def LoadObjectsFromSC(args):
    PrintArgs(args)

    global I18N, LOC # 我也没想到更好的办法了
    I18N = LoadLocale("SystemCreator", args.lcid);
    LOC = I18N.gettext

    Objects = InfoGen.InfoGen_Main(**vars(args))

    if args.store == True:
        print("上传行星系统到数据库...")
        Upl = Uploader(Objects, args)
        Upl.Run()

    print("生成文件...")

    Gen = InitGenerator(args.format, dict(args.format_args), Objects)
    Gen.Run()

    Path(args.output).mkdir(parents = True, exist_ok = True)
    OutputFileName = f"{args.output}/{Objects["MainID"]}.{Gen.FileSuffix()}"
    with open(OutputFileName, "w", encoding = "utf-8") as fout:
        fout.write(Gen.ExportTextToString())

    print(f"文件已写入：{OutputFileName}")