#include "gbuffers_atmosphere.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "CONSTANTS.INC"
#include "composite.h"

#include <limits>
#include <ranges>

#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

AtmospherePrevTableType AtmospherePrevTable;

void LoadAtmosphere(const BasicTableType& BasicTable, OIDType CurrentID, AtmospherePrevTableType* Table)
{
    SETable RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    if (!GetObjectS(RawData, "NoAtmosphere", 0, false))
    {
        auto it = RawData.find("Atmosphere");
        bool HasTable = it != RawData.end();
        SETable AtmData;
        try {if (HasTable) {AtmData = it->second[0].As<SETable>();}}
        catch (...) {HasTable = 0;}
        if (HasTable)
        {
            Atmosphere Result;
            Result.SurfacePressure = GetObjectS(AtmData, "Pressure", 0, std::numeric_limits<double>::quiet_NaN()) * StandardAtm;
            Result.ScaleHeight = GetObjectS(AtmData, "Height", 0, std::numeric_limits<double>::quiet_NaN()) * Km;
            SETable Compositions = AtmData.find("Composition")->second[0].As<SETable>();
            for (auto [Material, Percents] : Compositions)
            {
                Result.CompositionByVolume.insert({Material, Percents[0].As<double>()});
            }
            Table->insert({CurrentID, Result});
        }
    }
}

AtmosphereTableType gbuffer_atmosphere()
{
    spdlog::info("整理大气数据...");

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList, SatelliteList))
    {
        LoadAtmosphere(BASIC, i, &AtmospherePrevTable);
    }

    AtmosphereTableType Result;
    for (auto [OID, Data] : AtmospherePrevTable)
    {
        AtmosphereTableType::mapped_type Dst;
        Dst["SurfacePressure"] = Data.SurfacePressure;
        Dst["ScaleHeight"] = Data.ScaleHeight;
        Dst["CompositionByVolume"] = Data.CompositionByVolume;
        Result.insert({OID, Dst});
    }

    spdlog::info("完成");

    return Result;
}