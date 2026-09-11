#include "minobjs.h"
#include "MathFuncs/Trigonometry.h"
#include "final.h"
#include "composite.h"
#include "gbuffers_basic.h"
#include "gbuffers_orbit.h"
#include <functional>
#include <ranges>

using PredType = std::function<bool(OIDType, OIDType)>;
using FilterType = std::function<bool(OIDType)>;

struct Condition
{
    PredType Pred;
    FilterType Filter;
};

bool LargestByDiameterPred(OIDType L, OIDType R)
{
    return PhysicalTable.at(L).MeanRadius > PhysicalTable.at(R).MeanRadius;
}

bool LargestByDiameterFilter(OIDType V) {return 1;}

bool MostMassivePred(OIDType L, OIDType R)
{
    return PhysicalTable.at(L).Mass > PhysicalTable.at(R).Mass;
}

bool MostMassiveFilter(OIDType V) {return 1;}

bool SlowestRotatorsPred(OIDType L, OIDType R)
{
    return PhysicalTable.at(L).SiderealRotationPeriod > PhysicalTable.at(R).SiderealRotationPeriod;
}

bool SlowestRotatorsFilter(OIDType V) {return 1;}

bool FastestRotatorsPred(OIDType L, OIDType R)
{
    return PhysicalTable.at(L).SiderealRotationPeriod < PhysicalTable.at(R).SiderealRotationPeriod;
}

bool FastestRotatorsFilter(OIDType V) {return 1;}

bool RetrogradePred(OIDType L, OIDType R)
{
    return abs(OrbitTable.at(L).Inclination) < abs(OrbitTable.at(R).Inclination);
}

bool RetrogradeFilter(OIDType V)
{
    return abs(OrbitTable.at(V).Inclination) > 90 && abs(OrbitTable.at(V).Inclination) < 270;
}

bool HighlyInclinedPred(OIDType L, OIDType R)
{
    return abs(sind(OrbitTable.at(L).Inclination)) < abs(sind(OrbitTable.at(R).Inclination));
}

bool HighlyInclinedFilter(OIDType V) {return 1;}

static const std::flat_map<std::string, Condition> ConditionList
{
    {"LargestByDiameter", {.Pred = LargestByDiameterPred, .Filter = LargestByDiameterFilter}},
    {"MostMassive", {.Pred = MostMassivePred, .Filter = MostMassiveFilter}},
    {"SlowestRotators", {.Pred = SlowestRotatorsPred, .Filter = SlowestRotatorsFilter}},
    {"FastestRotators", {.Pred = FastestRotatorsPred, .Filter = FastestRotatorsFilter}},
    {"Retrograde", {.Pred = RetrogradePred, .Filter = RetrogradeFilter}},
    {"HighlyInclined", {.Pred = HighlyInclinedPred, .Filter = HighlyInclinedFilter}},
};

std::vector<OIDType> GetListedMinorObjects()
{
    std::vector<OIDType> MinorObjects(MinorObjectList.begin(), MinorObjectList.end());
    auto FiltedObjectsView = std::ranges::views::filter(MinorObjects, ConditionList.at(AsteroidsTablePred).Filter);
    std::vector<OIDType> FiltedObjects(FiltedObjectsView.begin(), FiltedObjectsView.end());
    std::sort(FiltedObjects.begin(), FiltedObjects.end(), ConditionList.at(AsteroidsTablePred).Pred);
    auto LimitedObjectsView = std::ranges::views::take(MinorObjects, AsteroidsTableLimit);
    return std::vector<OIDType>(LimitedObjectsView.begin(), LimitedObjectsView.end());
}