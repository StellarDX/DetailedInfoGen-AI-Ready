from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import ADBCClient, CurrentSQLVariation
from InfoGen_Data import CheckNamespace

import yaml, json

from sqlalchemy import Table, MetaData, Column, select, and_, or_, not_, case
from sqlalchemy import String, Double, Integer, Boolean, DateTime, BigInteger, Text
from sqlalchemy.orm import aliased
from pandas import read_sql, notna
from pandas.api.types import is_dict_like
from tabulate import tabulate
from langchain.tools import tool

_NamespaceBuffer = ""

def GetNamespace():
    return _NamespaceBuffer

def SetNamespace(NewNamespace):
    global _NamespaceBuffer
    _NamespaceBuffer = NewNamespace

def _PreCheck(Namespace, SystemName):
    if Namespace != "-A":
        if Namespace == None:
            raise ValueError("必须指定一个命名空间")
        CheckNamespace(Namespace)

def _QuerySystem_Unchecked(Namespace, SystemName):
    Cols = [Column(i, j) for i, j in [
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["source_file",            String(1024)],
        ["source_hash",            String(512)],
        ["code_page",              Integer],
        ["esi_estimator",          String(32)],
        ["absolute_orbit",         Boolean],
        ["common_plane_threshold", Integer],
        ["namespace",              String(255)],
        ["n_stars",                Integer],
        ["n_planets",              Integer],
        ["n_dwarf_planets",        Integer],
        ["n_satellites",           Integer],
        ["n_minor_planets",        Integer],
        ["n_comets",               Integer],
        ["spectral_types",         Text],
        ["create_date",            DateTime],
        ["modified_date",          DateTime]
    ]]
    Tbl = Table("ig_system", MetaData(), *Cols)

    Statement = ""
    if Namespace == "-A":
        Statement = str(select(Tbl)
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    elif SystemName == None or SystemName == "":
        Statement = str((select(Tbl)
            .where(Tbl.c.namespace == Namespace))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    else:
        Statement = str((select(Tbl)
            .where(Tbl.c.namespace == Namespace)
            .where(or_(Tbl.c.main_id == SystemName, Tbl.c.system_id == SystemName)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    Connection = ADBCClient()
    return read_sql(Statement, Connection)

def _QuerySystem(Namespace, SystemName, OutFmt):
    _PreCheck(Namespace, SystemName)

    SystemDataFrame = _QuerySystem_Unchecked(Namespace, SystemName)

    if len(SystemDataFrame) == 0:
        return "没找到任何资源"

    match OutFmt:
        case "json":
            return SystemDataFrame.to_json(
                orient = 'records',
                indent = 4
            )
        case "yaml":
            return yaml.dump(SystemDataFrame.to_dict(orient = 'records'),
                allow_unicode = True, 
                sort_keys = False, 
                default_flow_style = False, 
                indent = 2
            )
        case "wide":
            return str(tabulate(SystemDataFrame[["namespace", "main_id", "spectral_types", "create_date", "system_id", "source_file"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))
        case _:
            return str(tabulate(SystemDataFrame[["namespace", "main_id", "spectral_types", "create_date"]], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))

def _QueryAllObjectsInSystem_Unchecked(Namespace, SystemName):
    SysCols = [Column(i, j) for i, j in [
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["namespace",              String(255)]
    ]]
    SysTbl = Table("ig_system", MetaData(), *SysCols)

    Cols = [Column(i, j) for i, j in [
        ["object_id",        String(512)],
        ["system_id",        String(512)],
        ["parent_object_id", String(512)],
        ["primary_name",     String(255)],
        ["otype",            String(32)],
        ["class",            String(64)],
        ["depth",            Integer]
    ]]
    Tbl = Table("ig_object", MetaData(), *Cols)
    PTbl = aliased(Tbl, name = "primaries")
    CTbl = aliased(Tbl, name = "companions")

    Query = str((select(SysTbl.c.main_id.label("system"), CTbl.c.object_id, CTbl.c.otype, 
        CTbl.c.primary_name.label("ident"), PTbl.c.primary_name.label("parent_body"),
        CTbl.c["class"], CTbl.c.depth)
        .select_from(CTbl
            .join(SysTbl, SysTbl.c.system_id == CTbl.c.system_id)
            .outerjoin(PTbl, CTbl.c.parent_object_id == PTbl.c.object_id))
        .where(SysTbl.c.namespace == Namespace)
        .where(SysTbl.c.main_id == SystemName))
        .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    Connection = ADBCClient()
    return read_sql(Query, Connection)

def _QueryObject_Unchecked(Namespace, ObjectName, Info = []):
    SysCols = [Column(i, j) for i, j in [
        ["system_id",              String(512)],
        ["main_id",                String(255)],
        ["namespace",              String(255)]
    ]]
    SysTbl = Table("ig_system", MetaData(), *SysCols)

    Cols = [Column(i, j) for i, j in [
        ["object_id",        String(512)],
        ["system_id",        String(512)],
        ["parent_object_id", String(512)],
        ["primary_name",     String(255)],
        ["otype",            String(32)],
        ["class",            String(64)],
        ["depth",            Integer],
        ["sibling_index",    Integer],
        ["is_minor",         Boolean]
    ]]
    Tbl = Table("ig_object", MetaData(), *Cols)
    BasicLabels = [
        "namespace", "system",
        "object_id", "system_id", "parent_object_id",
        "ident", "otype", "class",
        "depth", "sibling_index", "is_minor"
    ]
    SelectList = [
        Tbl.c.object_id, Tbl.c.system_id, Tbl.c.parent_object_id,
        Tbl.c.primary_name.label("ident"),
        Tbl.c.otype, Tbl.c["class"], Tbl.c.depth,
        Tbl.c.sibling_index, Tbl.c.is_minor,
        SysTbl.c.main_id.label("system"), SysTbl.c.namespace
    ]
    JoinQuery = Tbl.join(SysTbl, SysTbl.c.system_id == Tbl.c.system_id)

    OrbitList, OrbitLabels = [], []
    if "orbit" in Info:
        OrbCols = [Column(i, j) for i, j in [
            ["object_id",              String(512)],
            ["ref_plane",              String(32)],
            ["position_x",             Double],
            ["position_y",             Double],
            ["position_z",             Double],
            ["period",                 Double],
            ["pericenter_dist",        Double],
            ["aphelion_dist",          Double],
            ["semi_major_axis",        Double],
            ["eccentricity",           Double],
            ["inclination",            Double],
            ["inclination_ecliptic",   Double],
            ["ascending_node",         Double],
            ["asc_node_ecliptic",      Double],
            ["epoch",                  Double],
            ["arg_of_pericenter",      Double],
            ["arg_of_peri_ecliptic",   Double],
            ["mean_anomaly",           Double],
            ["synodic_month",          Double],
            ["binary_orbit",           Boolean],
            ["is_primary",             Boolean],
            ["primary_name",           String(255)],
            ["companion_name",         String(255)],
            ["b_period",               Double],
            ["b_pericenter_dist",      Double],
            ["b_aphelion_dist",        Double],
            ["b_semi_major_axis",      Double],
            ["b_eccentricity",         Double],
            ["b_inclination",          Double],
            ["b_inclination_ecliptic", Double],
            ["b_ascending_node",       Double],
            ["b_asc_node_ecliptic",    Double],
            ["b_epoch",                Double],
            ["b_arg_of_pericenter",    Double],
            ["b_arg_of_peri_ecliptic", Double],
            ["b_mean_anomaly",         Double]
        ]]
        OrbTbl = Table("ig_orbit", MetaData(), *OrbCols)
        BinaryCondSt = (and_(Tbl.c.otype.in_(["Star", "Barycenter"]), not_(and_(OrbTbl.c.binary_orbit.is_(True), OrbTbl.c.is_primary.is_(False)))))
        BinaryCondPl = (and_(Tbl.c.otype.not_in(["Star", "Barycenter"]), OrbTbl.c.binary_orbit.is_(True)))
        PrimCompCond = (and_(OrbTbl.c.binary_orbit, OrbTbl.c.is_primary.is_(False)))
        OrbitLabels = ["ref_plane", "primary_name", "companion_name", 
            "period", "synodic_month", "pericenter_dist", "aphelion_dist", "semi_major_axis",
            "eccentricity", "inclination", "ascending_node", "arg_of_pericenter", "mean_anomaly",
            "incl_ecliptic", "asc_node_ecliptic", "arg_of_peri_ecliptic"]
        OrbitList = [
            case((BinaryCondSt, None), else_ = OrbTbl.c.ref_plane).label("ref_plane"),
            case((PrimCompCond, OrbTbl.c.primary_name), else_ = None).label("primary_name"),
            case((PrimCompCond, OrbTbl.c.companion_name), else_ = None).label("companion_name"),
            case((BinaryCondPl, OrbTbl.c.b_period), (BinaryCondSt, None), else_ = OrbTbl.c.period).label("period"),
            case((Tbl.c.otype != "Moon", None), else_ = OrbTbl.c.synodic_month).label("synodic_month"),
            case((BinaryCondPl, OrbTbl.c.b_pericenter_dist), (BinaryCondSt, None), else_ = OrbTbl.c.pericenter_dist).label("pericenter_dist"),
            case((BinaryCondPl, OrbTbl.c.b_aphelion_dist), (BinaryCondSt, None), else_ = OrbTbl.c.aphelion_dist).label("aphelion_dist"),
            case((BinaryCondPl, OrbTbl.c.b_semi_major_axis), (BinaryCondSt, None), else_ = OrbTbl.c.semi_major_axis).label("semi_major_axis"),
            case((BinaryCondPl, OrbTbl.c.b_eccentricity), (BinaryCondSt, None), else_ = OrbTbl.c.eccentricity).label("eccentricity"),
            case((BinaryCondPl, OrbTbl.c.b_inclination), (BinaryCondSt, None), else_ = OrbTbl.c.inclination).label("inclination"),
            case((BinaryCondPl, OrbTbl.c.b_inclination_ecliptic), (BinaryCondSt, None), else_ = OrbTbl.c.inclination_ecliptic).label("incl_ecliptic"),
            case((BinaryCondPl, OrbTbl.c.b_ascending_node), (BinaryCondSt, None), else_ = OrbTbl.c.ascending_node).label("ascending_node"),
            case((BinaryCondPl, OrbTbl.c.b_asc_node_ecliptic), (BinaryCondSt, None), else_ = OrbTbl.c.asc_node_ecliptic).label("asc_node_ecliptic"),
            case((BinaryCondPl, OrbTbl.c.b_epoch), (BinaryCondSt, None), else_ = OrbTbl.c.epoch).label("epoch"),
            case((BinaryCondPl, OrbTbl.c.b_arg_of_pericenter), (BinaryCondSt, None), else_ = OrbTbl.c.arg_of_pericenter).label("arg_of_pericenter"),
            case((BinaryCondPl, OrbTbl.c.b_arg_of_peri_ecliptic), (BinaryCondSt, None), else_ = OrbTbl.c.arg_of_peri_ecliptic).label("arg_of_peri_ecliptic"),
            case((BinaryCondPl, OrbTbl.c.b_mean_anomaly), (BinaryCondSt, None), else_ = OrbTbl.c.mean_anomaly).label("mean_anomaly")
        ]
        SelectList += OrbitList
        JoinQuery = JoinQuery.outerjoin(OrbTbl, OrbTbl.c.object_id == Tbl.c.object_id)

    PhysList, PhysLabels = [], []
    if "physic" in Info:
        PhysCols = [Column(i, j) for i, j in [
            ["object_id",                    String(512)],
            ["abs_magn_bol",                 Double],
            ["mean_radius",                  Double],
            ["dimension_x",                  Double],
            ["dimension_y",                  Double],
            ["dimension_z",                  Double],
            ["flattening_x",                 Double],
            ["flattening_y",                 Double],
            ["flattening_z",                 Double],
            ["circumference_equatorial",     Double],
            ["circumference_meridional",     Double],
            ["surface_area",                 Double],
            ["volume",                       Double],
            ["mass",                         Double],
            ["mean_density",                 Double],
            ["age",                          Double],
            ["surface_gravity",              Double],
            ["moment_of_inertia_factor",     Double],
            ["escape_velocity",              Double],
            ["synodic_rotation_period",      Double],
            ["sidereal_rotation_period",     Double],
            ["equatorial_rotation_velocity", Double],
            ["axial_tilt",                   Double],
            ["albedo_bond",                  Double],
            ["albedo_geometric",             Double],
            ["luminosity",                   Double],
            ["temperature",                  Double],
            ["radiant_flux",                 Double],
            ["kerr_spin",                    Double],
            ["kerr_charge",                  Double],
            ["comet_total_magn",             Double],
            ["comet_total_magn_slope",       Double],
            ["esi",                          Double],
            ["extra",                        Text]
        ]]
        PhysTbl = Table("ig_physical", MetaData(), *PhysCols)
        PhysLabels = ["abs_magn_bol", "mean_radius", "dimension_x", "dimension_y", "dimension_z",
            "flattening_x", "flattening_y", "flattening_z", "circumference_equatorial", "circumference_meridional",
            "surface_area", "volume", "mass", "mean_density", "age", "surface_gravity", "moment_of_inertia_factor",
            "escape_velocity", "synodic_rotation_period", "sidereal_rotation_period", "equatorial_rotation_velocity",
            "axial_tilt", "albedo_bond", "albedo_geometric", "luminosity", "temperature", "radiant_flux",
            "kerr_spin", "kerr_charge", "comet_total_magn", "comet_total_magn_slope", "esi", "extra"]
        PhysList = [
            PhysTbl.c.abs_magn_bol, PhysTbl.c.mean_radius, 
            PhysTbl.c.dimension_x, PhysTbl.c.dimension_y, PhysTbl.c.dimension_z,
            PhysTbl.c.flattening_x, PhysTbl.c.flattening_y, PhysTbl.c.flattening_z,
            PhysTbl.c.circumference_equatorial, PhysTbl.c.circumference_meridional,
            PhysTbl.c.surface_area, PhysTbl.c.volume, PhysTbl.c.mass, PhysTbl.c.mean_density,
            PhysTbl.c.age, PhysTbl.c.surface_gravity, PhysTbl.c.moment_of_inertia_factor, PhysTbl.c.escape_velocity, 
            PhysTbl.c.synodic_rotation_period, PhysTbl.c.sidereal_rotation_period, PhysTbl.c.equatorial_rotation_velocity,
            PhysTbl.c.axial_tilt, PhysTbl.c.albedo_bond, PhysTbl.c.albedo_geometric, 
            PhysTbl.c.luminosity, PhysTbl.c.temperature, PhysTbl.c.radiant_flux,
            PhysTbl.c.kerr_spin, PhysTbl.c.kerr_charge, PhysTbl.c.comet_total_magn, PhysTbl.c.comet_total_magn_slope,
            PhysTbl.c.esi, PhysTbl.c.extra
        ]
        SelectList += PhysList
        JoinQuery = JoinQuery.outerjoin(PhysTbl, PhysTbl.c.object_id == Tbl.c.object_id)

    AtmosphereList, AtmosphereLabels = [], []
    if "atmosphere" in Info:
        AtmoCols = [Column(i, j) for i, j in [
            ["object_id",        String(512)],
            ["surface_pressure", Double],
            ["scale_height",     Double]
        ]]
        AtmoTbl = Table("ig_atmosphere", MetaData(), *AtmoCols)
        AtmosphereLabels = ["atmosphere_pressure", "atmosphere_scale_height"]
        AtmosphereList = [
            AtmoTbl.c.surface_pressure.label("atmosphere_pressure"),
            AtmoTbl.c.scale_height.label("atmosphere_scale_height")
        ]
        SelectList += AtmosphereList
        JoinQuery = JoinQuery.outerjoin(AtmoTbl, AtmoTbl.c.object_id == Tbl.c.object_id)

    HydrosphereList, HydrosphereLabels = [], []
    if "hydrosphere" in Info:
        HydCols = [Column(i, j) for i, j in [
            ["object_id", String(512)],
            ["height",    Double],
        ]]
        HydTbl = Table("ig_hydrosphere", MetaData(), *HydCols)
        HydrosphereLabels = ["ocean_height"]
        HydrosphereList = [
            HydTbl.c.height.label("ocean_height"),
        ]
        SelectList += HydrosphereList
        JoinQuery = JoinQuery.outerjoin(HydTbl, HydTbl.c.object_id == Tbl.c.object_id)

    BiosphereList, BiosphereLabels = [], []
    if "biosphere" in Info:
        BioCols = [Column(i, j) for i, j in [
            ["object_id", String(512)],
            ["bio_class", String(64)],
            ["bio_type",  String(64)]
        ]]
        BioTbl = Table("ig_biosphere", MetaData(), *BioCols)
        BiosphereLabels = ["bio_class", "bio_type"]
        BiosphereList = [
            BioTbl.c.bio_class,
            BioTbl.c.bio_type
        ]
        SelectList += BiosphereList
        JoinQuery = JoinQuery.outerjoin(BioTbl, BioTbl.c.object_id == Tbl.c.object_id)

    MainStatement = ""
    if Namespace == "-A":
        MainStatement = str((select(*SelectList).select_from(JoinQuery))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    elif ObjectName == None or ObjectName == "":
        MainStatement = str(((select(*SelectList).select_from(JoinQuery))
            .where(SysTbl.c.namespace == Namespace))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    else:
        MainStatement = str(((select(*SelectList).select_from(JoinQuery))
            .where(SysTbl.c.namespace == Namespace)
            .where(or_(Tbl.c.primary_name == ObjectName, Tbl.c.object_id == ObjectName)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))

    Connection = ADBCClient()
    MainFrame = read_sql(MainStatement, Connection)

    # 主表生成以后处理一对多查询

    ObjList = MainFrame["object_id"].to_list()
    IdentCols = [Column(i, j) for i, j in [
        ["object_id", String(512)],
        ["alias",     String(255)]
    ]]
    IdentTbl = Table("ig_identifiers", MetaData(), *IdentCols)
    IdentQuery = str((select(IdentTbl).where(IdentTbl.c.object_id.in_(ObjList)))
        .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
    IdentFrame = read_sql(IdentQuery, Connection)
    IdentFrame.set_index("object_id", inplace = True)
    IdentDict = IdentFrame.groupby("object_id")["alias"].apply(list).to_dict()

    CompDict = None
    if "atmosphere" in Info or "hydrosphere" in Info:
        CompCols = [Column(i, j) for i, j in [
            ["object_id", String(512)],
            ["kind",      String(16)],
            ["component", String(64)],
            ["fraction",  Double]
        ]]
        CompTbl = Table("ig_composition", MetaData(), *CompCols)
        CompQuery = str((select(CompTbl).where(CompTbl.c.object_id.in_(ObjList)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        CompFrame = read_sql(CompQuery, Connection)
        CompFrame.set_index(["object_id", "kind"], inplace = True)
        CompDict = CompFrame.groupby(["object_id", "kind"])[["component", "fraction"]].apply(lambda x: x.to_dict('records')).to_dict()
    
    BiomeDict = None
    if "biosphere" in Info:
        BiomeCols = [Column(i, j) for i, j in [
            ["object_id", String(512)],
            ["biome",     String(64)]
        ]]
        BiomeTbl = Table("ig_biosphere_biome", MetaData(), *BiomeCols)
        BiomeQuery = str((select(BiomeTbl).where(BiomeTbl.c.object_id.in_(ObjList)))
            .compile(dialect = CurrentSQLVariation(), compile_kwargs = {"literal_binds": True}))
        BiomeFrame = read_sql(BiomeQuery, Connection)
        BiomeFrame.set_index(["object_id"], inplace = True)
        BiomeDict = BiomeFrame.groupby("object_id")["biome"].apply(list).to_dict()

    Result = MainFrame[BasicLabels].to_dict(orient = 'records')

    for i in Result:
        if i["object_id"] in IdentDict.keys():
            i["other_designations"] = IdentDict[i["object_id"]]

    if "orbit" in Info:
        OrbitFrame = [{i: j for i, j in k.items() if notna(j)}
            for k in MainFrame[OrbitLabels].to_dict(orient = 'records')]
        for i, j in zip(range(len(OrbitFrame)), OrbitFrame):
            if len(j) != 0:
                Result[i]["orbital_characteristics"] = j
    if "physic" in Info:
        PhysFrame = [{i: j for i, j in k.items() if notna(j)}
            for k in MainFrame[PhysLabels].to_dict(orient = 'records')]
        for i, j in zip(range(len(PhysFrame)), PhysFrame):
            if len(j) != 0:
                Result[i]["physical_characteristics"] = j
    if "atmosphere" in Info:
        AtmoFrame = [{i: j for i, j in k.items() if notna(j)}
            for k in MainFrame[AtmosphereLabels].to_dict(orient = 'records')]
        for i, j in zip(range(len(AtmoFrame)), AtmoFrame):
            if len(j) != 0:
                if (Result[i]["object_id"], "Atmosphere") in CompDict.keys(): # 直接变JSON了
                    j["compositions_by_volume"] = CompDict[(Result[i]["object_id"], "Atmosphere")]
                Result[i]["atmosphere"] = j
    if "hydrosphere" in Info:
        HydroFrame = [{i: j for i, j in k.items() if notna(j)}
            for k in MainFrame[HydrosphereLabels].to_dict(orient = 'records')]
        for i, j in zip(range(len(HydroFrame)), HydroFrame):
            if len(j) != 0:
                if (Result[i]["object_id"], "Hydrosphere") in CompDict.keys(): # 直接变JSON了
                    j["compositions_by_volume"] = CompDict[(Result[i]["object_id"], "Hydrosphere")]
                Result[i]["hydrosphere"] = j
    if "biosphere" in Info:
        BioFrame = [{i: j for i, j in k.items() if notna(j)}
            for k in MainFrame[BiosphereLabels].to_dict(orient = 'records')]
        for i, j in zip(range(len(BioFrame)), BioFrame):
            if len(j) != 0:
                if Result[i]["object_id"] in BiomeDict.keys(): # 直接变JSON了
                    j["biome"] = BiomeDict[Result[i]["object_id"]]
                Result[i]["bioSphere"] = j

    return Result

def _QueryObject(Namespace, ObjectName, OutFmt):
    _PreCheck(Namespace, ObjectName)
    FullOutputFormats = ["json", "yaml"]
    ObjectList = _QueryObject_Unchecked(Namespace, ObjectName, ["orbit", "physic", "atmosphere", "hydrosphere", "biosphere"] if OutFmt in FullOutputFormats else [])

    if len(ObjectList) == 0:
        return "没找到任何资源"

    match OutFmt:
        case "json":
            return json.dumps(ObjectList,
                ensure_ascii = False,
                indent = 4
            )
        case "yaml":
            return yaml.dump(ObjectList,
                allow_unicode = True, 
                sort_keys = False, 
                default_flow_style = False, 
                indent = 2
            )
        case "wide":
            return str(tabulate([{
                    "namespace": i["namespace"], 
                    "primary_name": i["ident"], 
                    "system": i["system"], 
                    "otype": i["otype"], 
                    "class": i["class"], 
                    "object_id": i["object_id"], 
                    "parent_object_id": i["parent_object_id"], 
                    "system_id": i["system_id"], 
                    "depth": i["depth"], 
                    "sibling_index": i["sibling_index"]
                } for i in ObjectList], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))
        case _:
            return str(tabulate([{
                    "namespace": i["namespace"], 
                    "primary_name": i["ident"], 
                    "system": i["system"], 
                    "otype": i["otype"], 
                    "class": i["class"], 
                } for i in ObjectList], 
                headers = "keys", 
                tablefmt = "plain", 
                showindex = False
            ))

@tool
def QuerySystem(SystemName): # 这个是给AI看的
    """查询指定系统的信息。

    根据系统名称查询系统数据，并以 JSON 格式返回。

    Args:
        SystemName: 要查询的系统名称。

    Returns:
        命中时返回系统信息（JSON 格式的字符串）；
        未命中（系统不存在）时返回提示文本 「没找到任何资源」。
    """

    return _QuerySystem(GetNamespace(), SystemName, "json")

# @tool
# def QueryAllObjectsInSystem(SystemName):
#     SystemFrame = _QueryAllObjectsInSystem_Unchecked(GetNamespace(), SystemName)
#     return SystemFrame.to_csv()

@tool
def QueryObject(SystemName, ObjectName, ObjectType = None):
    """
    查询指定行星系统内某个物体的完整信息。

    用于按「系统 + 物体名」检索该物体的轨道、物理、大气、水圈、生物圈等模块数据，
    返回格式化后的 JSON 字符串，便于直接阅读或继续解析。
    适用场景：需要了解某个已知天体/物体的具体参数与属性时调用。
    注意：SE 约定同一行星系统内的物体不会重名，因此正常情况下最多命中一条数据。

    所有字段的单位均遵循国际单位制，即：
        长度类字段单位为米，即面积为平方米，体积单位为立方米
        质量类字段单位为千克，即密度单位为千克每立方米
        时间类字段单位为秒（age为年，轨道数据的Epoch单位是JD），即重力单位为米每平方秒，速度单位为米每秒
        光度类字段单位为瓦特
        温度类字段单位为开氏度
        压强类字段单位为帕斯卡
        成分类字段为体积分数

    Args:
        SystemName (str): 行星系统名称，用于限定检索范围。
        ObjectName (str): 物体名称，需与系统中登记的名称一致。
        ObjectType (str): 物体类型，用于进一步过滤，可以是：
                          "Barycenter", "Star", "Planet", "DwarfPlanet", 
                          "Moon", "DwarfMoon", "Asteroid", "Comet"

    Returns:
        str: 命中时返回该物体信息的 JSON 字符串（ensure_ascii=False，indent=4）；
             未命中（系统或物体不存在）时返回提示文本 「没有那个系统或物体」。
    """

    ObjectDict = _QueryObject_Unchecked(GetNamespace(), ObjectName, ["orbit", "physic", "atmosphere", "hydrosphere", "biosphere"])
    ObjectDict = [i for i in ObjectDict if i["system"] == SystemName]
    if ObjectType != None:
        ObjectDict = [i for i in ObjectDict if i["otype"] == ObjectType]

    if len(ObjectDict) == 0:
        return "没有那个系统或物体"
    else: # SE约定单个行星系统内的物体不会出现重名，理论上这个列表最多只会得到唯一一条数据
        return json.dumps(ObjectDict, ensure_ascii = False, indent = 4)

def GetSystem(args):
    print(_QuerySystem("-A" if args.all_namespaces else args.namespace, args.name, args.output))

def GetObject(args):
    print(_QueryObject("-A" if args.all_namespaces else args.namespace, args.name, args.output))

def _Print_Dict(Data:dict, Indent = 0, IndentInc = 4):
    MaxLength = len(max(Data.keys(), key = len)) + 3
    for Key, Value in Data.items():
        if is_dict_like(Value):
            print(f"{' ' * Indent}{(Key + ":"):{MaxLength}}")
            _Print_Dict(Value, Indent + IndentInc)
        else:
            print(f"{' ' * Indent}{(Key + ":"):{MaxLength}}{Value}")

def DescribeSystem(args):
    _PreCheck(args.namespace, args.name)
    SystemDict = _QuerySystem_Unchecked(args.namespace, args.name).to_dict(orient = 'records')
    if len(SystemDict) == 0:
        print("没找到任何资源")
        return
    for i in SystemDict:
        _Print_Dict(i)
        print()

def DescribeObject(args):
    _PreCheck(args.namespace, args.name)
    ObjectDict = _QueryObject_Unchecked(args.namespace, args.name, ["orbit", "physic", "atmosphere", "hydrosphere", "biosphere"])
    if len(ObjectDict) == 0:
        print("没找到任何资源")
        return
    for i in ObjectDict:
        _Print_Dict(i)
        print()

Modules = {
    "system": GetSystem,
    "object": GetObject
}

Modules1 = {
    "system": DescribeSystem,
    "object": DescribeObject
}

def GetRegister(MainArgParser):
    ParserGetter = MainArgParser.add_parser("get", help = "打印数据", formatter_class = InfoGenHelpFormatter)
    ParserGetter.add_argument("-n", "--namespace", type = str, help = "命名空间")
    ParserGetter.add_argument("resource", nargs = 1, help = "资源类型")
    ParserGetter.add_argument("-o", "--output", type = str, help = "输出格式[wide|json|yaml]")
    ParserGetter.add_argument("-A", "--all-namespaces", action = 'store_true', help = "输出全部数据")
    ParserGetter.add_argument("name", nargs = "?", help = "资源名称")
    Modules[".Parser"] = ParserGetter

def DescribeRegister(MainArgParser):
    ParserDescriber = MainArgParser.add_parser("describe", help = "打印详细数据", formatter_class = InfoGenHelpFormatter)
    ParserDescriber.add_argument("-n", "--namespace", type = str, help = "命名空间")
    ParserDescriber.add_argument("resource", nargs = 1, help = "资源类型")
    ParserDescriber.add_argument("-A", "--all-namespaces", action = 'store_true', help = "输出全部数据")
    ParserDescriber.add_argument("name", nargs = "?", help = "资源名称")
    Modules1[".Parser"] = ParserDescriber

def Get(args):
    if args.resource is None:
        print("你需要指定一个资源去获取")
        return
    if args.resource[0] in Modules:
        Modules[args.resource[0]](args)
    else:
        print(f"无效的选项：{args.resource}\n")
        Modules[".Parser"].print_help()

def Describe(args):
    if args.resource is None:
        print("你需要指定一个资源去获取")
        return
    if args.resource[0] in Modules1:
        Modules1[args.resource[0]](args)
    else:
        print(f"无效的选项：{args.resource}\n")
        Modules1[".Parser"].print_help()