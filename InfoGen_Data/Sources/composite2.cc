#include "composite2.h"
#include "composite.h"
#include "composite1.h"
#include "gbuffers_basic.h"
#include "gbuffers_orbit.h"
#include "minobjs.h"

#include "spdlog/spdlog.h"

std::vector<OIDType> MinorPlanetHosts(const BasicTableType& BasicTable, const SystemType& System)
{
    std::vector<OIDType> Hosts;
    for (auto [k, v] : System)
    {
        if (std::find_if(v.begin(), v.end(), [&BasicTable](OIDType ID)
            {
                return BasicTable.at(ID).first == "Asteroid" || BasicTable.at(ID).first == "Comet";
            }) != v.end())
        {
            Hosts.push_back(k);
        }
    }
    return Hosts;
}

void Composite2(ReturnType* Result)
{   
    spdlog::info("整理小型物体...");

    std::vector<OIDType> ListedMinorObjects = GetListedMinorObjects();
    ReturnType MinorObjectsTable;
    auto MinorObjHosts = MinorPlanetHosts(BASIC, SystemTable);
    for (auto i : ListedMinorObjects)
    {
        ReturnType CurrentObject;
        CurrentObject["OType"] = BASIC.at(i).first;
        CurrentObject["Identifiers"] = IDENT.at(i);

        OIDType ParentID;
        for (auto j : MinorObjHosts)
        {
            if (std::find(SystemTable.at(j).begin(), SystemTable.at(j).end(), i) != SystemTable.at(j).end())
            {
                ParentID = j;
            }
        }
        CurrentObject["ParentBody"] = IDENT.at(ParentID)[0];

        if (OrbitalCharacteristicsTable.contains(i)) {CurrentObject["OrbitalCharacteristics"] = OrbitalCharacteristicsTable.at(i);}
        if (PhysicalCharacteristicsTable.contains(i)) {CurrentObject["PhysicalCharacteristics"] = PhysicalCharacteristicsTable.at(i);}
        MinorObjectsTable[IDENT.at(i).front().c_str()] = CurrentObject;
    }
    (*Result)["MinorObjects"] = MinorObjectsTable;

    spdlog::info("完成");
}