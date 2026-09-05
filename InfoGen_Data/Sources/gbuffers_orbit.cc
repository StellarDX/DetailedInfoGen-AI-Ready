#include "gbuffers_orbit.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "Trigonometry/Trigonometry.h"
#include "CONSTANTS.INC"
#include "composite.h"

#include <limits>
#include <queue>

#include <pybind11/pytypes.h>

OrbitTableType OrbitTable;

void LoadOrbitParamsFromRawData(const BasicTableType& BasicTable, OrbitTableType* Output)
{
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
}

void MakeEclipticalReferencedOrbit(OrbitTableType* OrbitTable, const SystemType& SysTable, OIDType Barycenter)
{
    // TODO: 这里所有的物体的轨道面是相对于上一级物体的赤道面的，而物体的赤道面又是相对于上一级物体的轨道面的，如何才能把它们全部统一为相对于黄道面的轨道根数？（为了好计算，这里直接采用真实世界的坐标系定义就行）
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