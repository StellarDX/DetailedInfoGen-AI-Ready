#define _USE_MATH_DEFINES
#include "gbuffers_hydrosphere.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "CONSTANTS.INC"
#include "composite.h"

#include <limits>
#include <ranges>

#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

OceanPrevTableType OceanPrevTable;

void LoadOcean(const BasicTableType& BasicTable, OIDType CurrentID, OceanPrevTableType* Table)
{
    SETable RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    if (!GetObjectS(RawData, "NoOcean", 0, false))
    {
        auto it = RawData.find("Ocean");
        bool HasTable = it != RawData.end();
        SETable OceanData;
        try {if (HasTable) {OceanData = it->second[0].As<SETable>();}}
        catch (...) {HasTable = 0;}
        if (HasTable)
        {
            Ocean Result;
            Result.Height = GetObjectS(OceanData, "Depth", 0, std::numeric_limits<double>::quiet_NaN()) * Km;
            if (isnan(Result.Height))
            {
                Result.Height = GetObjectS(OceanData, "Height", 0, std::numeric_limits<double>::quiet_NaN()) * Km;
            }
            SETable Compositions = OceanData.find("Composition")->second[0].As<SETable>();
            for (auto [Material, Percents] : Compositions)
            {
                Result.CompositionByVolume.insert({Material, Percents[0].As<double>()});
            }
            Table->insert({CurrentID, Result});
        }
    }
}

HydrosphereTableType gbuffers_hydrosphere()
{
    spdlog::info("整理海洋数据...");

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList, SatelliteList))
    {
        LoadOcean(BASIC, i, &OceanPrevTable);
    }

    HydrosphereTableType Result;
    for (auto [OID, Data] : OceanPrevTable)
    {
        HydrosphereTableType::mapped_type Dst;
        Dst["Height"] = Data.Height * Km;
        Dst["CompositionByVolume"] = Data.CompositionByVolume;
        Result.insert({OID, Dst});
    }

    spdlog::info("完成");

    return Result;
}
