#include "composite1.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"
#include "gbuffers_atmosphere.h"
#include "gbuffers_basic.h"
#include "gbuffers_biosphere.h"
#include "gbuffers_hydrosphere.h"
#include "gbuffers_orbit.h"

#include <queue>

#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

StaticPosTableType    StaticPosTable;

std::unordered_set<OIDType> MinorObjectList;

void ComputeStaticPos(const SystemType& System, OIDType Barycenter, const OrbitTableType& Orbit, StaticPosTableType* Output)
{
    std::queue<OIDType> OIDQueue;
    OIDQueue.push(Barycenter);
    (*Output)[Barycenter] = SEVec3::Zero();
    while (!OIDQueue.empty())
    {
        OIDType CurrentOID = OIDQueue.front();
        OIDQueue.pop();
        for (auto i : System.at(CurrentOID))
        {
            (*Output)[i] = SEVec3((*Output)[CurrentOID] + Orbit.at(i).Position);
            OIDQueue.push(i);
        }
    }
}

void __DFS_Iterate(ReturnType* Result, const SystemType* System, const IdentTableType* Idt, const OrbitCharTableType* Obt, const PhysicalCharTableType* Phy, const AtmosphereTableType* Atm, const HydrosphereTableType* Hyd, const BiosphereTableType* Bio, const std::unordered_set<OIDType>* MinObj, OIDType CurrentID)
{
    py::dict CurrentObject;
    CurrentObject["Identifiers"] = Idt->at(CurrentID);
    if (Obt->contains(CurrentID)) {CurrentObject["OrbitalCharacteristics"] = Obt->at(CurrentID);}
    if (Phy->contains(CurrentID)) {CurrentObject["PhysicalCharacteristics"] = Phy->at(CurrentID);}
    if (Atm->contains(CurrentID)) {CurrentObject["Atmosphere"] = Atm->at(CurrentID);}
    if (Hyd->contains(CurrentID)) {CurrentObject["Hydrosphere"] = Hyd->at(CurrentID);}
    if (Bio->contains(CurrentID)) {CurrentObject["Biosphere"] = Bio->at(CurrentID);}

    std::vector<OIDType> SubSystemsFinalSeq;
    if (!System->at(CurrentID).empty())
    {
        SubSystemsFinalSeq.assign(System->at(CurrentID).begin(), System->at(CurrentID).end());
        std::erase_if(SubSystemsFinalSeq, [MinObj](OIDType v)
        {
            return MinObj->contains(v);
        });
        if (SortSystem)
        {
            std::sort(SubSystemsFinalSeq.begin(), SubSystemsFinalSeq.end(), [](OIDType L, OIDType R)
            {
                return std::abs(OrbitTable.at(L).SemiMajorAxis) < std::abs(OrbitTable.at(R).SemiMajorAxis);
            });
        }
        std::vector<std::string> SubSystemIdents;
        for (auto i : SubSystemsFinalSeq)
        {
            SubSystemIdents.push_back(Idt->at(i).front());
        }
        CurrentObject["SubSystems"] = SubSystemIdents;
    }

    (*Result)[Idt->at(CurrentID).front().c_str()] = CurrentObject;

    if (!System->at(CurrentID).empty())
    {
        for (auto i : SubSystemsFinalSeq)
        {
            __DFS_Iterate(Result, System, Idt, Obt, Phy, Atm, Hyd, Bio, MinObj, i);
        }
    }
}

void Composite1(ReturnType* Result)
{
    OrbitCharTableType OrbitalCharacteristicsTable = gbuffer_orbit();

    ComputeStaticPos(SystemTable, BarycenterID, OrbitTable, &StaticPosTable);

    PhysicalCharTableType PhysicalCharacteristicsTable = gbuffer_basic();
    AtmosphereTableType AtmosphereTable = gbuffer_atmosphere();
    HydrosphereTableType HydrosphereTable = gbuffers_hydrosphere();
    BiosphereTableType BiosphereTable = gbuffers_biosphere();

    spdlog::info("生成物体列表...");

    auto MinorObjs = std::ranges::views::concat(MinorPlanetList, CometList);
    MinorObjectList.insert(MinorObjs.begin(), MinorObjs.end());
    ReturnType ObjectList;
    __DFS_Iterate(&ObjectList, &SystemTable, &IDENT, &OrbitalCharacteristicsTable, &PhysicalCharacteristicsTable, &AtmosphereTable, &HydrosphereTable, &BiosphereTable, &MinorObjectList, BarycenterID);
    (*Result)["Objects"] = ObjectList;

    spdlog::info("完成");
}