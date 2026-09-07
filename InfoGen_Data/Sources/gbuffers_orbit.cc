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
#include <spdlog/spdlog.h>
#include <stdexcept>

OrbitTableType OrbitTable;

void __CompleteOrbitalParams_Backtrack(OIDType OID, OrbitCharacteristics* Orbit, const SystemType& System, const BasicTableType& BasicTable)
{
    auto Parent = std::find_if(System.begin(), System.end(), [OID](SystemType::value_type v)
    {
        return std::find(v.second.begin(), v.second.end(), OID) != v.second.end();
    });
    if (Parent == System.end())[[unlikely]] {return;}

    if (BasicTable.at(Parent->first).first == "Barycenter")
    {
        // 如果上级物体是Barycenter，说明这个物体位于一个多星系统中，
        // 此时需要在同级物体中找到两个相互绕行的主物体
        // TODO...
    }
}

// 这个功能目前依然有一点问题，就是物体位于一个多星系统时，直接用相对于质心的轨道不能得到正确的结果
void CompleteOrbitalParams(OIDType OID, OrbitCharacteristics* Orbit, const SystemType& System, const BasicTableType& BasicTable)
{
    int ProvidedParams = 0;
    bool Has_a = 0, Has_T = 0, Has_Mu = 0;
    if (!isnan(Orbit->PericenterDist))
    {
        ProvidedParams++;
        Has_a = 1;
    }
    if (!isnan(Orbit->Period))
    {
        ProvidedParams++;
        Has_T = 1;
    }
    if (!isnan(Orbit->GravParam))
    {
        ProvidedParams++;
        Has_Mu = 1;
    }

    if (ProvidedParams < 2)
    {
        // if (Has_Mu)
        // {
            throw std::invalid_argument("有物体提供了不完整的轨道参数");
        // }
        // 如果仅知道一个半长轴或周期，可以试着通过上级物体的质量来反推
        // __CompleteOrbitalParams_Backtrack(OID, Orbit, System, BasicTable);
    }

    double a = Orbit->PericenterDist, e = Orbit->Eccentricity;
    if (Has_a) { a /= (1.0 - e); }

    if (!Has_Mu && Has_T && Has_a)[[likely]]
    {
        Orbit->GravParam = (4.0 * M_PI * M_PI * a * a * a) / (Orbit->Period * Orbit->Period);
    }

    else if (!Has_T && Has_Mu && Has_a)
    {
        Orbit->Period = 2.0 * M_PI * sqrt(a * a * a / Orbit->GravParam);
    }

    else if (!Has_a && Has_T && Has_Mu)
    {
        // 先计算半长轴
        a = cbrt(Orbit->GravParam * Orbit->Period * Orbit->Period / (4.0 * M_PI * M_PI));
        // 然后计算近地点距离
        Orbit->PericenterDist = a - a * e;
    }

    if (!(isfinite(Orbit->PericenterDist) &&
        isfinite(Orbit->Period) &&
        isfinite(Orbit->GravParam)))
    {
        throw std::invalid_argument("无法补全轨道参数，有物体提供了不完整的轨道参数？");
    }
}

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

        enum{AAU, AKm} SemuMajorAxisUnit;
        static const double SemiMajorAxisScales[](AU, Km);
        SEReal SemiMajorAxis = std::numeric_limits<SEReal>::quiet_NaN();
        it = OrbitRawData.find("SemiMajorAxis");
        SemuMajorAxisUnit = AAU;
        if (it == OrbitRawData.end())
        {
            it = OrbitRawData.find("SemiMajorAxisKm");
            SemuMajorAxisUnit = AKm;
        }
        if (it != OrbitRawData.end())
        {
            SemiMajorAxis = it->second[0].As<SEReal>() * SemiMajorAxisScales[SemuMajorAxisUnit];
        }
        Table.PericenterDist = SemiMajorAxis - SemiMajorAxis * Table.Eccentricity;

        if (std::isnan(Table.PericenterDist))
        {
            it = OrbitRawData.find("PericenterDist");
            SemuMajorAxisUnit = AAU;
            if (it == OrbitRawData.end())
            {
                it = OrbitRawData.find("PericenterDistKm");
                SemuMajorAxisUnit = AKm;
            }
            if (it != OrbitRawData.end())
            {
                Table.PericenterDist = it->second[0].As<SEReal>() * SemiMajorAxisScales[SemuMajorAxisUnit];
            }
        }
        // if (OID != Barycenter) {CompleteOrbitalParams(OID, &Table, System, BasicTable);} // FIXME

        Table.Inclination = GetObjectS<SEReal>(OrbitRawData, "Inclination", 0, 0);
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
}

OrbitCharTableType gbuffer_orbit()
{
    LoadOrbitParamsFromRawData(BASIC, SystemTable, BarycenterID, &OrbitTable);

    if (!IsAbsoluteOrbitParams)
    {
        MakeEclipticalReferencedOrbit(&OrbitTable, SystemTable, BarycenterID);
    }

    OrbitParamsToStateVectors(&OrbitTable, SystemTable, BarycenterID);

    OrbitCharTableType Value;
    for (auto [OID, Table] : OrbitTable)
    {
        OrbitCharTableType::mapped_type Dst;
        Dst["RefPlane"] = Table.RefPlane;
        Dst["Position"] = Table.Position; // Eigen向量扔到py::dict里，Python侧会得到一个numpy数组
        Dst["Period"] = Table.Period;
        Dst["PericenterDist"] = Table.PericenterDist;
        Dst["Eccentricity"] = Table.Eccentricity;
        Dst["Inclination"] = Table.Inclination;
        if (!IsAbsoluteOrbitParams) {Dst["InclEcliptic"] = Table.InclinationEcliptic;}
        Dst["AscendingNode"] = Table.AscendingNode;
        if (!IsAbsoluteOrbitParams) {Dst["AscNodeEcliptic"] = Table.AscNodeEcliptic;}
        Dst["ArgOfPericenter"] = Table.ArgOfPericenter;
        if (!IsAbsoluteOrbitParams) {Dst["ArgOfPeriEcliptic"] = Table.ArgOfPeriEcliptic;}
        Dst["MeanAnomaly"] = Table.MeanAnomaly;
        Value.insert({OID, Dst});
    }

    return Value;
}