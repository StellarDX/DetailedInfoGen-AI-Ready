#define _USE_MATH_DEFINES
#include "gbuffers_orbit.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "MathFuncs/Trigonometry.h"
#include "Keplerian/Keplerian.h"
#include "CONSTANTS.INC"
#include "composite.h"
#include "final.h"
#include "fmt/format.h"

#include <limits>
#include <queue>

#include <pybind11/pytypes.h>
#include <pybind11/eigen.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

OrbitTableType OrbitTable;

void LoadOrbitParamsFromRawData(const BasicTableType& BasicTable, const SystemType& System, OIDType Barycenter, OrbitTableType* Output)
{
    spdlog::info("提取轨道数据（此步骤预计耗时较长，请耐心等待）...");

    auto LoadOrbitParamsFromRawData_Start = std::chrono::high_resolution_clock::now();

    int WaitSecond = 3;
    std::jthread LoadOrbitParamsFromRawData_Timer([WaitSecond, BasicTable, Output](std::stop_token Stoken) 
    {
        while (!Stoken.stop_requested()) 
        {
            // 使用 stop_token 实现可中断的睡眠
            std::this_thread::sleep_for(std::chrono::seconds(WaitSecond));
            if (Stoken.stop_requested()) break;
            spdlog::info("提取轨道数据（{}%）...", (100 * Output->size()) / BasicTable.size());
        }
    });

    for (auto [OID, Data] : BasicTable)
    {
        SETable MainTable = Data.second[1].As<SETable>();
        SETable OrbitRawData = MainTable.find("Orbit")->second[0].As<SETable>();
        OrbitCharacteristics Table;
        Table.RefPlane = GetObjectS<SEString>(OrbitRawData, "RefPlane", 0, "Extrasolar");
        Table.GravParam = GetObjectS<SEReal>(OrbitRawData, "GravParams", 0, std::numeric_limits<SEReal>::quiet_NaN());

        Table.Eccentricity = GetObjectS<SEReal>(OrbitRawData, "Eccentricity", 0, 0);

        enum{TYears, TDays} PeriodUnit;
        static const double PeriodScales[](TropicalYear, SynodicDay);
        auto it = OrbitRawData.find("Period");
        PeriodUnit = TYears;
        if (it == OrbitRawData.end())
        {
            it = OrbitRawData.find("PeriodDays");
            PeriodUnit = TDays;
        }
        if (it != OrbitRawData.end())
        {
            Table.Period = it->second[0].As<SEReal>() * PeriodScales[PeriodUnit];
        }
        else {Table.Period = std::numeric_limits<SEReal>::quiet_NaN();}

        enum{AAU, AKm} SemiMajorAxisUnit;
        static const double SemiMajorAxisScales[](AU, Km);
        Table.SemiMajorAxis = std::numeric_limits<SEReal>::quiet_NaN();
        it = OrbitRawData.find("SemiMajorAxis");
        SemiMajorAxisUnit = AAU;
        if (it == OrbitRawData.end())
        {
            it = OrbitRawData.find("SemiMajorAxisKm");
            SemiMajorAxisUnit = AKm;
        }
        if (it != OrbitRawData.end())
        {
            Table.SemiMajorAxis = it->second[0].As<SEReal>() * SemiMajorAxisScales[SemiMajorAxisUnit];
        }
        Table.PericenterDist = Table.SemiMajorAxis - Table.SemiMajorAxis * Table.Eccentricity;
        if (Table.Eccentricity < 1)
        {
            Table.AphelionDist = 2. * Table.SemiMajorAxis - Table.PericenterDist;
        }

        if (std::isnan(Table.PericenterDist))
        {
            it = OrbitRawData.find("PericenterDist");
            SemiMajorAxisUnit = AAU;
            if (it == OrbitRawData.end())
            {
                it = OrbitRawData.find("PericenterDistKm");
                SemiMajorAxisUnit = AKm;
            }
            if (it != OrbitRawData.end())
            {
                Table.PericenterDist = it->second[0].As<SEReal>() * SemiMajorAxisScales[SemiMajorAxisUnit];
            }
            Table.SemiMajorAxis = Table.PericenterDist / (1. - Table.Eccentricity);
            if (Table.Eccentricity < 1)
            {
                Table.AphelionDist = 2. * Table.SemiMajorAxis - Table.PericenterDist;
            }
        }

        Table.Inclination = GetObjectS<SEReal>(OrbitRawData, "Inclination", 0, 0);
        if ((Table.Inclination > 90 && Table.Inclination < 270) || 
            (Table.Inclination < -90 && Table.Inclination > -270))
        {
            Table.Period = -Table.Period; // 逆行轨道周期为负
        }

        Table.AscendingNode = GetObjectS<SEReal>(OrbitRawData, "AscendingNode", 0, 0);
        Table.ArgOfPericenter = GetObjectS<SEReal>(OrbitRawData, "ArgOfPericenter", 0, 0);

        Table.MeanAnomaly = GetObjectS<SEReal>(OrbitRawData, "MeanAnomaly", 0, 0);

        if (GetObjectS<SEBoolean>(MainTable, "TidalLocked", 0, false))
        {
            Table.AxialTilt = 0;
            Table.Equinox = 0;
        }
        else
        {
            Table.AxialTilt = GetObjectS<SEReal>(MainTable, "Obliquity", 0, 0);
            Table.Equinox = GetObjectS<SEReal>(MainTable, "EqAscendNode", 0, 0);
        }

        Output->insert({OID, Table});
    }

    auto LoadOrbitParamsFromRawData_End = std::chrono::high_resolution_clock::now();
    auto LoadOrbitParamsFromRawData_Delay = std::chrono::duration_cast<std::chrono::seconds>(LoadOrbitParamsFromRawData_End - LoadOrbitParamsFromRawData_Start);
    
    spdlog::info("完成（已处理 {} 个物体，延迟：{} s）", Output->size(), LoadOrbitParamsFromRawData_Delay.count());
}

SEMat3 Rz(double Yaw)
{
    double c = cosd(Yaw), s = sind(Yaw);
    SEMat3 m;
    m << c, -s, 0, s, c, 0, 0, 0, 1;
    return m;
}

SEMat3 Rx(double Roll)
{
    double c = cosd(Roll), s=sind(Roll);
    SEMat3 m;
    m << 1, 0, 0, 0, c, -s, 0, s, c;
    return m;
}

void MakeEclipticalReferencedOrbit(OrbitTableType* OrbitTable, const SystemType& SysTable, OIDType Barycenter)
{
    spdlog::info("计算黄道轨道根数（此步骤预计耗时较长，请耐心等待）...");

    auto MakeEclipticalReferencedOrbit_Start = std::chrono::high_resolution_clock::now();

    int WaitSecond = 3, Processed = 0;
    std::jthread MakeEclipticalReferencedOrbit_Timer([WaitSecond, OrbitTable, Processed](std::stop_token Stoken) 
    {
        while (!Stoken.stop_requested()) 
        {
            // 使用 stop_token 实现可中断的睡眠
            std::this_thread::sleep_for(std::chrono::seconds(WaitSecond));
            if (Stoken.stop_requested()) break;
            spdlog::info("计算黄道轨道根数（{}%）...", (100 * Processed) / OrbitTable->size());
        }
    });
    
    std::queue<OIDType> OIDQueue;
    std::flat_map<OIDType, SEMat3> RotationMatTable;
    RotationMatTable[Barycenter] = SEMat3::Identity();
    OIDQueue.push(Barycenter);

    while(!OIDQueue.empty())
    {
        OIDType CurrentOID = OIDQueue.front();
        OIDQueue.pop();
        auto it = SysTable.find(CurrentOID);
        if(it == SysTable.end()) {continue;}

        const auto& Pdata = OrbitTable->at(CurrentOID);
        SEMat3 Rp = Rz(Pdata.Equinox) * Rx(Pdata.AxialTilt);

        for(OIDType i : it->second)
        {
            auto CIt = OrbitTable->find(i);
            if(CIt == OrbitTable->end()) continue;
            OrbitCharacteristics& Inc = CIt->second;

            SEMat3 Mchild = (Inc.RefPlane == "Equator")
                ? SEMat3(RotationMatTable[CurrentOID] * Rp)
                : SEMat3::Identity();
            RotationMatTable[i] = Mchild;

            SEMat3 Q  = Rz(Inc.AscendingNode) * Rx(Inc.Inclination) * Rz(Inc.ArgOfPericenter);
            SEMat3 Qr = Mchild * Q;
            double sinI = ::sqrt(Qr(0,2)*Qr(0,2) + Qr(1,2)*Qr(1,2));
            if(sinI > ::pow(10, -CommonPlaneThreshold))
            {
                Inc.InclinationEcliptic = arctan2d(sinI, Qr(2,2));
                Inc.AscNodeEcliptic = arctan2d(Qr(0,2), -Qr(1,2));
                Inc.ArgOfPeriEcliptic = arctan2d(Qr(2,0), Qr(2,1));
            } 
            else
            {
                // 接近共面时：Ω接近0，保留ω
                Inc.InclinationEcliptic = 0;
                Inc.AscNodeEcliptic = 0;
                Inc.ArgOfPeriEcliptic = arctan2d(Qr(2,0), Qr(2,1));
            }
            OIDQueue.push(i);
        }

        ++Processed;
    }

    auto MakeEclipticalReferencedOrbit_End = std::chrono::high_resolution_clock::now();
    auto MakeEclipticalReferencedOrbit_Delay = std::chrono::duration_cast<std::chrono::seconds>(MakeEclipticalReferencedOrbit_End - MakeEclipticalReferencedOrbit_Start);
    
    spdlog::info("完成（已处理 {} 个物体，延迟：{} s）", Processed, MakeEclipticalReferencedOrbit_Delay.count());
}

void OrbitParamsToStateVectors(OrbitTableType* Table, const SystemType& SysTable, OIDType Barycenter)
{
    spdlog::info("计算位置...");

    for (auto [OID, Table] : *Table)
    {
        double Inclin, AscNode, ArgOfPeri;
        if (IsAbsoluteOrbitParams)
        {
            Inclin = Table.Inclination;
            AscNode = Table.AscendingNode;
            ArgOfPeri = Table.ArgOfPericenter;
        }
        else
        {
            Inclin = Table.InclinationEcliptic;
            AscNode = Table.AscNodeEcliptic;
            ArgOfPeri = Table.ArgOfPeriEcliptic;
        }

        double EccentricAnomaly = InverseKeplerianEquation(
            Table.Eccentricity, Table.MeanAnomaly);
        double TrueAnomaly = GetTrueAnomalyFromEccentricAnomaly(
            Table.Eccentricity, EccentricAnomaly);
        double SemiLatusRectum = Table.PericenterDist * (1. + Table.Eccentricity);
        double CurrentDist = SemiLatusRectum /
            (1. + Table.Eccentricity * cosd(TrueAnomaly));
        double ArgOfLatitude = ArgOfPeri + TrueAnomaly;

        double SinAscNode = sind(AscNode);
        double CosAscNode = cosd(AscNode);
        double SinArgOfLatitude = sind(ArgOfLatitude);
        double CosArgOfLatitude = cosd(ArgOfLatitude);
        double SinInclination = sind(Inclin);
        double CosInclination = cosd(Inclin);

        SEVec3 CurrentPos = CurrentDist * SEVec3(
            CosAscNode * CosArgOfLatitude -
            SinAscNode * SinArgOfLatitude * CosInclination,
            SinAscNode * CosArgOfLatitude +
            CosAscNode * SinArgOfLatitude * CosInclination,
            SinArgOfLatitude * SinInclination);
        Table.Position = CurrentPos;
    }

    spdlog::info("完成");
}

void TransferBarycenter(OrbitTableType* Table, const SystemType& SysTable, OIDType CurrentID, const BasicTableType& BasicTable)
{
    if (!SysTable.at(CurrentID).empty())
    {
        for (auto OID : SysTable.at(CurrentID))
        {
            TransferBarycenter(Table, SysTable, OID, BasicTable);
        }
    }

    if (BasicTable.at(CurrentID).first == "Barycenter")
    {
        auto SubSystems = SysTable.at(CurrentID);
        std::sort(SubSystems.begin(), SubSystems.end(), 
            [Table](OIDType L, OIDType R)
        {
            // 这三个条件比对下来，多星系统里面必然会出现一组三个值全部相等的物体且位于相邻位置
            // 到时只需要检验两个近日点幅角是否相差180就行
            return (*Table)[L].PericenterDist < (*Table)[R].PericenterDist &&
                (*Table)[L].Inclination < (*Table)[R].Inclination &&
                (*Table)[L].AscendingNode < (*Table)[R].AscendingNode &&
                (*Table)[L].MeanAnomaly < (*Table)[R].MeanAnomaly;
        });

        uint64_t i = 1;
        for (; i < SubSystems.size(); ++i)
        {
            if (((*Table)[SubSystems[i]].Inclination - (*Table)[SubSystems[i - 1]].Inclination < 1E-12) &&
                ((*Table)[SubSystems[i]].AscendingNode - (*Table)[SubSystems[i - 1]].AscendingNode < 1E-12) &&
                ((*Table)[SubSystems[i]].MeanAnomaly - (*Table)[SubSystems[i - 1]].MeanAnomaly < 1E-12))
            {
                break;
            }
        }
        if (i == SubSystems.size())
        {
            spdlog::warn("质心 {}:{} 下一级没有找到两个互相绕行的物体",
                BasicTable.at(CurrentID).second[0].As<SEString>(), CurrentID);
            return;
        }

        auto& Barycen = (*Table)[CurrentID], 
            &Primary = (*Table)[SubSystems[i - 1]], &Companion = (*Table)[SubSystems[i]];
        if (std::abs(Primary.ArgOfPericenter - Companion.ArgOfPericenter) - 180 < 1E-5) // 文件->数字这一过程发生了3次精度丢失，因此不能设的太高
        {
            Companion.BinaryOrbit = 1;
            Companion.IsPrimary = 0;
            Companion.Primary = BasicTable.at(SubSystems[i - 1]).second[0].As<SEString>();
            Companion.Companion = BasicTable.at(SubSystems[i]).second[0].As<SEString>();
            Companion.BPeriod = Primary.Period;
            Companion.BPericenterDist = Primary.PericenterDist + Companion.PericenterDist;
            Companion.BAphelionDist = Primary.AphelionDist + Companion.AphelionDist;
            Companion.BSemiMajorAxis = Primary.SemiMajorAxis + Companion.SemiMajorAxis;
            Companion.BEccentricity = Companion.Eccentricity;
            Companion.BInclination = Companion.Inclination;
            Companion.BInclinationEcliptic = Companion.InclinationEcliptic;
            Companion.BAscendingNode = Companion.AscendingNode;
            Companion.BAscNodeEcliptic = Companion.AscNodeEcliptic;
            Companion.BArgOfPericenter = Companion.ArgOfPericenter;
            Companion.BArgOfPeriEcliptic = Companion.ArgOfPeriEcliptic;
            Companion.BMeanAnomaly = Companion.MeanAnomaly;

            Primary.BinaryOrbit = 1;
            Primary.IsPrimary = 1;
            Primary.BPeriod = Barycen.Period;
            Primary.BPericenterDist = Barycen.PericenterDist;
            Primary.BAphelionDist = Barycen.AphelionDist;
            Primary.BSemiMajorAxis = Barycen.SemiMajorAxis;
            Primary.BEccentricity = Barycen.Eccentricity;
            Primary.BInclination = Barycen.Inclination;
            Primary.BInclinationEcliptic = Barycen.InclinationEcliptic;
            Primary.BAscendingNode = Barycen.AscendingNode;
            Primary.BAscNodeEcliptic = Barycen.AscNodeEcliptic;
            Primary.BArgOfPericenter = Barycen.ArgOfPericenter;
            Primary.BArgOfPeriEcliptic = Barycen.ArgOfPeriEcliptic;
            Primary.BMeanAnomaly = Barycen.MeanAnomaly;
        }
        else
        {
            spdlog::warn("质心 {}:{} 下一级没有找到两个互相绕行的物体",
                BasicTable.at(CurrentID).second[0].As<SEString>(), CurrentID);
        }
    }
}

void ComputeSynodicMonth(OrbitTableType* Table, const SystemType& SysTable, OIDType ParentID, OIDType CurrentID, const BasicTableType& BasicTable)
{
    if (!SysTable.at(CurrentID).empty())
    {
        for (auto OID : SysTable.at(CurrentID))
        {
            ComputeSynodicMonth(Table, SysTable, CurrentID, OID, BasicTable);
        }
    }

    if (BasicTable.at(CurrentID).first == "Moon")
    {
        double m = (*Table)[CurrentID].Period;
        double y = (*Table)[ParentID].Period;
        (*Table)[CurrentID].SynodicOrbitalPeriod = (m * y) / std::abs(m - y);
    }
}

OrbitCharTableType gbuffer_orbit()
{
    LoadOrbitParamsFromRawData(BASIC, SystemTable, BarycenterID, &OrbitTable);

    if (!IsAbsoluteOrbitParams)
    {
        MakeEclipticalReferencedOrbit(&OrbitTable, SystemTable, BarycenterID);
    }

    OrbitParamsToStateVectors(&OrbitTable, SystemTable, BarycenterID);

    spdlog::info("迁移多星系统轨道...");
    TransferBarycenter(&OrbitTable, SystemTable, BarycenterID, BASIC);
    spdlog::info("完成");

    spdlog::info("计算会合周期...");
    ComputeSynodicMonth(&OrbitTable, SystemTable, BarycenterID, BarycenterID, BASIC);
    spdlog::info("完成");

    spdlog::info("生成轨道数据表...");

    OrbitCharTableType Value;
    for (auto [OID, Table] : OrbitTable)
    {
        OrbitCharTableType::mapped_type Dst;
        Dst["RefPlane"] = Table.RefPlane;
        Dst["Position"] = Table.Position; // Eigen向量扔到py::dict里，Python侧会得到一个numpy数组
        Dst["Period"] = Table.Period;
        Dst["PericenterDist"] = Table.PericenterDist;
        Dst["AphelionDist"] = Table.AphelionDist;
        Dst["SemiMajorAxis"] = Table.SemiMajorAxis;
        Dst["Eccentricity"] = Table.Eccentricity;
        Dst["Inclination"] = Table.Inclination;
        if (!IsAbsoluteOrbitParams) {Dst["InclEcliptic"] = Table.InclinationEcliptic;}
        Dst["AscendingNode"] = Table.AscendingNode;
        if (!IsAbsoluteOrbitParams) {Dst["AscNodeEcliptic"] = Table.AscNodeEcliptic;}
        Dst["ArgOfPericenter"] = Table.ArgOfPericenter;
        if (!IsAbsoluteOrbitParams) {Dst["ArgOfPeriEcliptic"] = Table.ArgOfPeriEcliptic;}
        Dst["MeanAnomaly"] = Table.MeanAnomaly;

        if (BASIC.at(OID).first == "Moon")
        {
            Dst["SynodicMonth"] = Table.SynodicOrbitalPeriod;
        }

        Dst["BinaryOrbit"] = Table.BinaryOrbit;
        if (Table.BinaryOrbit)
        {
            Dst["IsPrimary"] = Table.IsPrimary;
            if (!Table.IsPrimary)
            {
                Dst["Primary"] = Table.Primary;
                Dst["Companion"] = Table.Companion;
            }
            Dst["BPeriod"] = Table.BPeriod;
            Dst["BPericenterDist"] = Table.BPericenterDist;
            Dst["BAphelionDist"] = Table.BAphelionDist;
            Dst["BSemiMajorAxis"] = Table.BSemiMajorAxis;
            Dst["BEccentricity"] = Table.BEccentricity;
            Dst["BInclination"] = Table.BInclination;
            Dst["BInclinationEcliptic"] = Table.BInclinationEcliptic;
            Dst["BAscendingNode"] = Table.BAscendingNode;
            Dst["BAscNodeEcliptic"] = Table.BAscNodeEcliptic;
            Dst["BArgOfPericenter"] = Table.BArgOfPericenter;
            Dst["BArgOfPeriEcliptic"] = Table.BArgOfPeriEcliptic;
            Dst["BMeanAnomaly"] = Table.BMeanAnomaly;
        }

        Value.insert({OID, Dst});
    }

    spdlog::info("完成");

    return Value;
}