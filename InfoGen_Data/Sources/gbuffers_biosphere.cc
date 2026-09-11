#include "gbuffers_biosphere.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "CONSTANTS.INC"
#include "composite.h"
#include "composite1.h"

#include <limits>
#include <ranges>

#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

LifePrevTableType LifePrevTable;

void LoadLife(const BasicTableType& BasicTable, OIDType CurrentID, LifePrevTableType* Table)
{
    SETable RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    if (!GetObjectS(RawData, "NoLife", 0, false))
    {
        auto it = RawData.find("Life");
        bool HasTable = it != RawData.end();
        SETable LifeData;
        try {if (HasTable) {LifeData = it->second[0].As<SETable>();}}
        catch (...) {HasTable = 0;}
        if (HasTable)
        {
            Life Result;
            Result.Class = GetObjectS(LifeData, "Class", 0, std::string("None"));
            Result.Type = GetObjectS(LifeData, "Type", 0, std::string("None"));
            std::string BiomeText = GetObjectS(LifeData, "Biome", 0, std::string("None"));
            auto BiomeView = BiomeText | std::views::split('/');
            for (auto i : BiomeView)
            {
                Result.Biome.push_back(std::string(i.begin(), i.end()));
            }
            Table->insert({CurrentID, Result});
        }
    }
}

BiosphereTableType gbuffers_biosphere()
{
    spdlog::info("整理生物圈数据...");

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList, SatelliteList))
    {
        LoadLife(BASIC, i, &LifePrevTable);
    }

    BiosphereTableType Result;
    for (auto [OID, Data] : LifePrevTable)
    {
        BiosphereTableType::mapped_type Dst;
        Dst["Class"] = Data.Class;
        Dst["Type"] = Data.Type;
        Dst["Biome"] = Data.Biome;
        Result.insert({OID, Dst});
    }

    spdlog::info("完成");

    return Result;
}