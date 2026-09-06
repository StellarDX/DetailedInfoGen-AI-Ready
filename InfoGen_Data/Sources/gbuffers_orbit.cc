#include "gbuffers_orbit.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "MathFuncs/Trigonometry.h"
#include "CONSTANTS.INC"
#include "composite.h"

#include <limits>
#include <queue>

#include <pybind11/pytypes.h>
#include <spdlog/spdlog.h>

OrbitTableType OrbitTable;

void LoadOrbitParamsFromRawData(const BasicTableType& BasicTable, OrbitTableType* Output)
{
    spdlog::info("提取轨道数据...");
    for (auto [OID, Data] : BasicTable)
    {
        SETable MainTable = Data.second[1].As<SETable>();
        SETable OrbitRawData = MainTable.find("Orbit")->second[0].As<SETable>();
        OrbitCharacteristics Table;
        Table.RefPlane = GetObjectS<SEString>(OrbitRawData, "RefPlane", 0, "Extrasolar");
        Table.GravParams = GetObjectS<SEReal>(OrbitRawData, "GravParams", 0, std::numeric_limits<SEReal>::quiet_NaN());

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
    spdlog::info("完成");
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

OrbitCharTableType gbuffer_orbit()
{
    LoadOrbitParamsFromRawData(BASIC, &OrbitTable);

    if (!IsAbsoluteOrbitParams)
    {
        MakeEclipticalReferencedOrbit(&OrbitTable, SystemTable, BarycenterID);
    }

    return OrbitCharTableType(); // TODO
}