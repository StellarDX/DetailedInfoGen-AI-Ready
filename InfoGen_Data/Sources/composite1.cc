#include "composite1.h"
#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"
#include "gbuffers_basic.h"
#include "gbuffers_orbit.h"

#include <queue>

#include <spdlog/spdlog.h>

StaticPosTableType    StaticPosTable;

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

void Composite1(ReturnType* Result)
{
    OrbitCharTableType OrbitalCharacteristicsTable = gbuffer_orbit();

    ComputeStaticPos(SystemTable, BarycenterID, OrbitTable, &StaticPosTable);

    PhysicalCharTableType PhysicalCharacteristicsTable = gbuffer_basic();
}