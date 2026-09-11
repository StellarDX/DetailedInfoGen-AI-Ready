#include "composite2.h"
#include "composite.h"
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
        if (OrbitTable.contains(i)) {CurrentObject["OrbitalCharacteristics"] = OrbitTable.at(i);}
        if (PhysicalTable.contains(i)) {CurrentObject["PhysicalCharacteristics"] = PhysicalTable.at(i);}
        MinorObjectsTable[IDENT.at(i).front().c_str()] = CurrentObject;
    }
    (*Result)["MinorObjects"] = MinorObjectsTable;

    spdlog::info("完成");
}