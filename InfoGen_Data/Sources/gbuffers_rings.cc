#define _USE_MATH_DEFINES
#include "gbuffers_rings.h"
#include "gbuffers_basic.h"
#include "gbuffers_orbit.h"
#include "CONSTANTS.INC"
#include "MathFuncs/Trigonometry.h"

#include <spdlog/spdlog.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

RingsPrevTableType RingsPrevTable;

double MaxOrbitalHeight(double SemiMajorAxis, double Inclination, double Eccentricity, double ArgOfPercenter)
{
    // 参考文献：Rodet L , Beust H , Bonnefoy M ,et al.ODEA: Orbital Dynamics in a complex 
    // Evolving Architecture - Application to the planetary system HD 106906[J].Astronomy 
    // and Astrophysics, 2019, 631.DOI:10.1051/0004-6361/201935728.
    return SemiMajorAxis * sind(Inclination) * (sqrt(1. - pow(Eccentricity, 2) * pow(cosd(ArgOfPercenter), 2)) + Eccentricity * std::abs(sind(ArgOfPercenter)));
}

void LoadRings(const BasicTableType& BasicTable, const SystemType& System, OIDType CurrentID, const PhysicalTableType& PhysTable, const OrbitTableType& OrbTable, RingsPrevTableType* Table)
{
    Rings Result;
    bool HasValue = 0;

    std::vector<OIDType> HasAsteroids;
    std::ranges::set_intersection(MinorPlanetList, System.at(CurrentID), std::back_inserter(HasAsteroids));
    if (!HasAsteroids.empty())
    {
        HasValue = 1;

        OIDType LargestObj = HasAsteroids[0], ClosestObj = HasAsteroids[0], FarthestObj = HasAsteroids[0], HighestOrb = HasAsteroids[0];
        for (auto i : HasAsteroids)
        {
            if (PhysTable.at(LargestObj).Dimensions.maxCoeff() < PhysTable.at(i).Dimensions.maxCoeff())
            {
                LargestObj = i;
            }
            if (OrbTable.at(ClosestObj).PericenterDist > OrbTable.at(i).PericenterDist)
            {
                ClosestObj = i;
            }
            if (OrbTable.at(FarthestObj).AphelionDist < OrbTable.at(i).AphelionDist)
            {
                FarthestObj = i;
            }
            auto BighestOrbitCompare = [&OrbTable](OIDType A, OIDType B)
            {
                auto AOrbit = OrbTable.at(A), BOrbit = OrbTable.at(B);
                double AHeight = MaxOrbitalHeight(AOrbit.SemiMajorAxis, AOrbit.Inclination, AOrbit.Eccentricity, AOrbit.ArgOfPericenter);
                double BHeight = MaxOrbitalHeight(BOrbit.SemiMajorAxis, BOrbit.Inclination, BOrbit.Eccentricity, BOrbit.ArgOfPericenter);
                return AHeight < BHeight;
            };
            if (BighestOrbitCompare(HighestOrb, i)) {HighestOrb = i;}
        }

        // 星周盘，由一堆小行星组成
        RingsDetails CircumplanetaryDisc;
        CircumplanetaryDisc.Type = "CircumplanetaryDisc";
        CircumplanetaryDisc.InnerRadius = OrbTable.at(ClosestObj).PericenterDist;
        CircumplanetaryDisc.Width = OrbTable.at(FarthestObj).AphelionDist - CircumplanetaryDisc.InnerRadius;
        auto HighestOrbit = OrbTable.at(HighestOrb);
        CircumplanetaryDisc.Thickness = 2. * MaxOrbitalHeight(HighestOrbit.SemiMajorAxis, HighestOrbit.Inclination, HighestOrbit.Eccentricity, HighestOrbit.ArgOfPericenter);
        CircumplanetaryDisc.RockMaxSize = PhysTable.at(LargestObj).Dimensions.maxCoeff();
        CircumplanetaryDisc.ObjectCount = HasAsteroids.size();
        Result.Details.push_back(CircumplanetaryDisc);
    }

    SETable RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    if (!GetObjectS(RawData, "NoRings", 0, false))
    {
        auto it = RawData.find("Rings");
        bool HasTable = it != RawData.end();
        SETable RingsData;
        try {if (HasTable) {RingsData = it->second[0].As<SETable>();}}
        catch (...) {HasTable = 0;}
        if (HasTable)
        {
            HasValue = 1;
            // 类似木星主环和土星DCBA环的部分，可见
            RingsDetails MainRing;
            MainRing.Type = "MainRing";
            MainRing.InnerRadius = GetObjectS<SEReal>(RingsData, "InnerRadius", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;
            MainRing.Width = GetObjectS<SEReal>(RingsData, "EdgeRadius", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km - MainRing.InnerRadius;
            MainRing.Thickness = GetObjectS<SEReal>(RingsData, "Thickness", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;
            MainRing.RockMaxSize = GetObjectS<SEReal>(RingsData, "RocksMaxSize", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;

            // 类似木星内侧环和土星E环的部分，背光可见
            RingsDetails DuskyRing;
            DuskyRing.Type = "Halo";
            DuskyRing.InnerRadius = GetObjectS<SEReal>(RingsData, "EdgeRadius", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;
            DuskyRing.Width = GetObjectS<SEReal>(RingsData, "OuterRadius", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km - DuskyRing.InnerRadius;
            DuskyRing.Thickness = GetObjectS<SEReal>(RingsData, "Thickness", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;
            DuskyRing.RockMaxSize = GetObjectS<SEReal>(RingsData, "RocksMaxSize", 0, std::numeric_limits<SEReal>::quiet_NaN()) * Km;

            // SE控制星环长相的几个参数似乎都是正态分布生成的，这里也只能拟合个大概了
            // TODO

            if (MainRing.Width) {Result.Details.push_back(MainRing);}
            if (DuskyRing.Width) {Result.Details.push_back(DuskyRing);}
        }
    }

    if (HasValue) {Table->insert({CurrentID, Result});}
}

RingsTableType gbuffers_rings()
{
    spdlog::info("整理星环数据...");

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList, SatelliteList))
    {
        LoadRings(BASIC, SystemTable, i, PhysicalTable, OrbitTable, &RingsPrevTable);
    }

    RingsTableType Result;
    for (auto [OID, Data] : RingsPrevTable)
    {
        RingsTableType::mapped_type Dst;
        namespace py = pybind11;
        std::vector<py::dict> Details;
        for (auto i : Data.Details)
        {
            py::dict Data;
            Data["Type"] = i.Type;
            Data["InnerRadius"] = i.InnerRadius;
            Data["Width"] = i.Width;
            Data["Thickness"] = i.Thickness;
            Data["RockMaxSize"] = i.RockMaxSize;
            if (i.Type == "CircumplanetaryDisc") {Data["ObjectCount"] = i.ObjectCount;}
            Details.push_back(Data);
        }
        Dst["Details"] = Details;
        Result.insert({OID, Dst});
    }
    
    spdlog::info("完成");

    return Result;
}