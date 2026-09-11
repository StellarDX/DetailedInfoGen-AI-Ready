#include "composite2.h"
#include "composite.h"
#include "composite1.h"
#include "gbuffers_basic.h"
#include "gbuffers_orbit.h"
#include "minobjs.h"

#include "spdlog/spdlog.h"

void Composite2(ReturnType* Result)
{   
    spdlog::info("整理小型物体...");

    std::vector<OIDType> ListedMinorObjects = GetListedMinorObjects();
    ReturnType MinorObjectsTable;
    for (auto i : ListedMinorObjects)
    {
        ReturnType CurrentObject;
        CurrentObject["Identifiers"] = IDENT.at(i);
        if (OrbitalCharacteristicsTable.contains(i)) {CurrentObject["OrbitalCharacteristics"] = OrbitalCharacteristicsTable.at(i);}
        if (PhysicalCharacteristicsTable.contains(i)) {CurrentObject["PhysicalCharacteristics"] = PhysicalCharacteristicsTable.at(i);}
        MinorObjectsTable[IDENT.at(i).front().c_str()] = CurrentObject;
    }
    (*Result)["MinorObjects"] = MinorObjectsTable;

    spdlog::info("完成");
}