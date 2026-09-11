#define _USE_MATH_DEFINES
#include "gbuffers_basic.h"
#include "AdvMath/Integral.h"
#include "CONSTANTS.INC"
#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"
#include "composite1.h"
#include "gbuffers_orbit.h"

#include <limits>

#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include "spdlog/spdlog.h"

PhysicalTableType PhysicalTable;

void LoadRadiuses(const SETable& Data, PhysicalCharacteristics* Table)
{
    Table->Dimensions.setConstant(GetObjectS(Data, "Radius", 0, 
        std::numeric_limits<double>::quiet_NaN()) * Km * 2.);
    if (isnan(Table->Dimensions.x()))
    {
        Table->Dimensions.setConstant(GetObjectS(Data, "RadSol", 0, 
            std::numeric_limits<double>::quiet_NaN()) * SolarRadius * 2.);
    }

    Table->Flattening.setZero();
    Table->Flattening.y() = GetObjectS(Data, "Oblateness", 0, 
        std::numeric_limits<double>::quiet_NaN());
    if (isnan(Table->Flattening.y()))
    {
        Table->Flattening = GetObjectS(Data, "Oblateness", 0, SEVec3(SEVec3::Zero()));
    }

    Table->Dimensions = Table->Dimensions.array() - Table->Dimensions.array() * Table->Flattening.array();

    if (isnan(Table->Dimensions.x()))[[unlikely]]
    {
        Table->Dimensions = GetObjectS(Data, "Dimensions", 0, 
            SEVec3(SEVec3::Constant(std::numeric_limits<double>::quiet_NaN()))) * Km;
        if (isnan(Table->Dimensions.x()))
        {
            Table->Dimensions = GetObjectS(Data, "DimensionsSol", 0, 
                SEVec3(SEVec3::Constant(std::numeric_limits<double>::quiet_NaN()))) * SolarRadius;
        }

        Table->Flattening = 1. - (Table->Dimensions / Table->Dimensions.maxCoeff()).array();
    }
}

double EquatorialCircumference(SEVec3 Dimensions)
{
    double a = std::max(Dimensions.x(), Dimensions.z()) / 2.;
    double b = std::min(Dimensions.x(), Dimensions.z()) / 2.;
    if (a == b) {return 2. * M_PI * a;}

    double e2 = 1. - pow(b / a, 2);
    auto E = [e2](double tet)
    {
        return sqrt(1. - e2 * pow(sin(tet), 2));
    };
    return 4. * a * GaussKronrodQuadrature()(E, 0, M_PI_2);
}

double MeridionalCircumference(SEVec3 Dimensions)
{
    double a = std::max(Dimensions.x(), Dimensions.z()) / 2.;
    double b = Dimensions.y() / 2.;
    if (a == b) {return 2. * M_PI * a;}

    double e2 = 1. - pow(b / a, 2);
    auto E = [e2](double tet)
    {
        return sqrt(1. - e2 * pow(sin(tet), 2));
    };
    return 4. * a * GaussKronrodQuadrature()(E, 0, M_PI_2);
}

double SurfaceArea(SEVec3 Dimensions)
{
    SEVec3 Radius = Dimensions / 2.;
    std::sort(Radius.begin(), Radius.end());
    double a = Radius[2], b = Radius[1], c = Radius[0];
    if (a == b && b == c) {return 4. * M_PI * a * a;}

    auto F = [](double phi, double k2)
    {
        auto EllipticalIntegral1st = [k2](double tet)
        {
            return 1. / sqrt(1. - k2 * pow(sin(tet), 2));
        };
        return GaussKronrodQuadrature()(EllipticalIntegral1st, 0, phi);
    };

    auto E = [](double phi, double k2)
    {
        auto EllipticalIntegral2nd = [k2](double tet)
        {
            return sqrt(1. - k2 * pow(sin(tet), 2));
        };
        return GaussKronrodQuadrature()(EllipticalIntegral2nd, 0, phi);
    };

    double phi = acos(c / a);
    double k2 = (a * a * (b * b - c * c)) / (b * b * (a * a - c * c));
    return 2. * M_PI * c * c + (2. * M_PI * a * b) / sin(phi) *
        (E(phi, k2) * pow(sin(phi), 2) + F(phi, k2) * pow(cos(phi), 2));
}

void ComputeRadiusesRelativeParams(PhysicalCharacteristics* Table)
{
    Table->MeanRadius = cbrt((Table->Dimensions.x() / 2.) * (Table->Dimensions.y() / 2.) * (Table->Dimensions.z() / 2.));
    Table->Circumference = 
    {
        EquatorialCircumference(Table->Dimensions),
        MeridionalCircumference(Table->Dimensions)
    };
    Table->SurfaceArea = SurfaceArea(Table->Dimensions);
    Table->Volume = (4. / 3.) * M_PI * (Table->Dimensions.x() / 2.) * (Table->Dimensions.y() / 2.) * (Table->Dimensions.z() / 2.);
}

void LoadMass(const SETable& Data, PhysicalCharacteristics* Table)
{
    Table->Mass = GetObjectS(Data, "Mass", 0, std::numeric_limits<double>::quiet_NaN()) * EarthMass;
    if (isnan(Table->Mass))
    {
        Table->Mass = GetObjectS(Data, "MassSol", 0, std::numeric_limits<double>::quiet_NaN()) * SolarMass;
    }
    if (isnan(Table->Mass))
    {
        Table->Mass = GetObjectS(Data, "MassKg", 0, std::numeric_limits<double>::quiet_NaN());
    }
    if (isnan(Table->Mass))
    {
        Table->Mass = GetObjectS(Data, "MassJup", 0, std::numeric_limits<double>::quiet_NaN()) * JupiterMass;
    }
}

void ComputeMassRelativeParams(PhysicalCharacteristics* Table)
{
    Table->MeanDensity = Table->Mass / Table->Volume;
    Table->SurfaceGravity = (GravConstant * Table->Mass) / (Table->MeanRadius * Table->MeanRadius);
    Table->EscapeVelocity = sqrt((2. * GravConstant * Table->Mass) / Table->MeanRadius);
}

double RotationVelocity(double EquatorialRadius, double RotationPeriod)
{
    return (2. * M_PI * EquatorialRadius) / RotationPeriod;
}

void LoadBasicData(const SETable& RawData, OIDType CurrentID, const OrbitTableType& Orbit, PhysicalCharacteristics* Table)
{
    Table->Class = GetObjectS(RawData, "Class", 0, std::string("None"));
    LoadRadiuses(RawData, Table);
    ComputeRadiusesRelativeParams(Table);
    LoadMass(RawData, Table);
    ComputeMassRelativeParams(Table);
    Table->MomentOfInertiaFactor = GetObjectS(RawData, "InertiaMoment", 0, std::numeric_limits<double>::quiet_NaN());
    if (GetObjectS(RawData, "TidalLocked", 0, false))
    {
        Table->SiderealRotationPeriod = Orbit.at(CurrentID).Period;
    }
    else {Table->SiderealRotationPeriod = GetObjectS(RawData, "RotationPeriod", 0, std::numeric_limits<double>::quiet_NaN()) * 3600;}
    Table->EquatorialRotationVelocity = RotationVelocity(std::max(Table->Dimensions.x(), Table->Dimensions.z()), Table->SiderealRotationPeriod);
    Table->AxialTilt = GetObjectS(RawData, "Obliquity", 0, 0);
    if ((Table->AxialTilt > 90 && Table->AxialTilt < 270) || 
        (Table->AxialTilt < -90 && Table->AxialTilt > -270))
    {
        Table->SiderealRotationPeriod = -Table->SiderealRotationPeriod; // 反向自转周期为负
    }
}

void LoadLuminosity(const SETable& Data, PhysicalCharacteristics* Table)
{
    Table->Luminosity = GetObjectS(Data, "LumBol", 0, std::numeric_limits<double>::quiet_NaN());
    auto NoAccDisk = GetObjectS(Data, "NoAccretionDisk", 0, true);
    if (!NoAccDisk)
    {
        auto AccDiskTable = Data.find("AccretionDisk")->second[0].As<SETable>();
        if (isnan(Table->Luminosity)) {Table->Luminosity = 0;}
        Table->Luminosity += GetObjectS(AccDiskTable, "LuminosityBol", 0, std::numeric_limits<double>::quiet_NaN());
    }
    Table->Luminosity *= SolarLuminosity;
}

void LoadStar(const BasicTableType& BasicTable, OIDType CurrentID, const OrbitTableType& Orbit, PhysicalCharacteristics* Table)
{
    auto RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    LoadBasicData(RawData, CurrentID, Orbit, Table);
    LoadLuminosity(RawData, Table);
    Table->Temperature = GetObjectS(RawData, "Temperature", 0, std::numeric_limits<double>::quiet_NaN());
    if (isnan(Table->Temperature)) [[likely]]
    {
        Table->Temperature = GetObjectS(RawData, "Teff", 0, std::numeric_limits<double>::quiet_NaN());
    }
}

void ComputePlanetTemperature(const SETable& RawData, OIDType CurrentID, const ObjectListType& Stars, const StaticPosTableType& StaticPos, const PhysicalTableType& PhysTable, PhysicalCharacteristics* Table)
{
    double BaseTemperature = GetObjectS(RawData, "Teff", 0, 0); // 把Rogue planet考虑进去
    double EndogenousHeating = GetObjectS(RawData, "EndogenousHeating", 0, 0);
    double GreenHouse = 0;
    if (!GetObjectS(RawData, "NoAtmosphere", 0, false))
    {
        auto it = RawData.find("Atmosphere");
        bool HasTable = it != RawData.end();
        SETable AtmData;
        try {if (HasTable) {AtmData = it->second[0].As<SETable>();}}
        catch (...) {HasTable = 0;}
        if (HasTable) {GreenHouse = GetObjectS(RawData, "GreenHouse", 0, 0);}
    }

    Table->Albedo = 
    {
        GetObjectS(RawData, "AlbedoBond", 0, std::numeric_limits<double>::quiet_NaN()),
        GetObjectS(RawData, "AlbedoGeom", 0, std::numeric_limits<double>::quiet_NaN())
    };
    if (isnan(Table->Albedo.x()))
    {
        Table->Albedo.setConstant(GetObjectS(RawData, "Albedo", 0, std::numeric_limits<double>::quiet_NaN()));
    }

    SEVec3 CurrentPos = StaticPos.at(CurrentID);
    double Equilibrium = 0;
    double Fluxes = 0;
    for (auto i : Stars)
    {
        double StarLuminosity = PhysTable.at(i).Luminosity;
        double Distance = (CurrentPos - StaticPos.at(i)).lpNorm<2>();
        double AlbedoBond = Table->Albedo.x();
        double Flux = StarLuminosity / (4. * M_PI * Distance * Distance);
        Fluxes += Flux;
        Equilibrium += pow((Flux * (1. - AlbedoBond)) / (4. * StefanBoltzmann), 1. / 4.);
    }

    // 暂时只统计这四个热源，如果是卫星还有个潮汐加热，那个要用热力学模型才能计算，这里先不计了
    // （奈何热力学水实在太深了）
    Table->RadiantFlux = Fluxes;
    Table->Temperature = BaseTemperature + Equilibrium + EndogenousHeating + GreenHouse;
}

void ComputeSynodicRotationPeriod(OIDType CurrentID, const OrbitTableType& Orbit, PhysicalCharacteristics* Table)
{
    double Y = Orbit.at(CurrentID).Period;
    double D = Table->SiderealRotationPeriod;
    Table->SynodicRotationPeriod = (Y * D) / std::abs(Y - D);
}

double SolarSysESI(double MeanRadius, double BulkDensity, double EscapeVelocity, double Temperature)
{
    double RadiusSim = pow(1. - abs((MeanRadius - EarthRadius) / (MeanRadius + EarthRadius)), 0.57 / 4.);
    double DensitySim = pow(1. - abs((BulkDensity - EarthDensity) / (BulkDensity + EarthDensity)), 1.07 / 4.);
    double EVelSim = pow(1. - abs((EscapeVelocity - EarthEscapeVel) / (EscapeVelocity + EarthEscapeVel)), 0.70 / 4.);
    double TempSim = pow(1. - abs((Temperature - EarthTemperature) / (Temperature + EarthTemperature)), 5.58 / 4.);
    return RadiusSim * DensitySim * EVelSim * TempSim;
}

double ExtrasolarESI(double RadiantFlux, double MeanRadius)
{
    double FluxSim = pow(abs((RadiantFlux - SolarConstant) / (RadiantFlux + SolarConstant)), 2);
    double RadiusSim = pow(abs((MeanRadius - EarthRadius) / (MeanRadius + EarthRadius)), 2);
    return 1. - sqrt((FluxSim + RadiusSim) / 2.);
}

void ComputeESI(OIDType CurrentID, PhysicalCharacteristics* Table)
{
    if (ESIEstimator == "Extrasolar") {ExtrasolarESI(Table->RadiantFlux, Table->MeanRadius);}
    else {Table->ESI = SolarSysESI(Table->MeanRadius, Table->MeanDensity, Table->EscapeVelocity, Table->Temperature);}
}

void LoadPlanet(const BasicTableType& BasicTable, OIDType CurrentID, const OrbitTableType& Orbit, const ObjectListType& Stars, const StaticPosTableType& StaticPos, const PhysicalTableType& PhysTable, PhysicalCharacteristics* Table)
{
    auto RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    LoadBasicData(RawData, CurrentID, Orbit, Table);
    ComputeSynodicRotationPeriod(CurrentID, Orbit, Table);
    ComputePlanetTemperature(RawData, CurrentID, Stars, StaticPos, PhysTable, Table);
    if (BasicTable.at(CurrentID).first == "Planet" ||
        BasicTable.at(CurrentID).first == "Moon" ||
        BasicTable.at(CurrentID).first == "DwarfPlanet")
    {
        ComputeESI(CurrentID, Table);
    }
}

PhysicalCharTableType gbuffer_basic()
{
    spdlog::info("整理物理数据（此步骤预计耗时较长，请耐心等待）...");

    auto gbuffer_basic_Start = std::chrono::high_resolution_clock::now();

    int WaitSecond = 3;
    std::jthread LoadOrbitParamsFromRawData_Timer([WaitSecond](std::stop_token Stoken) 
    {
        while (!Stoken.stop_requested()) 
        {
            // 使用 stop_token 实现可中断的睡眠
            std::this_thread::sleep_for(std::chrono::seconds(WaitSecond));
            if (Stoken.stop_requested()) break;
            spdlog::info("整理物理数据（{}%）...", (100 * PhysicalTable.size()) / BASIC.size());
        }
    });

    for (auto i : StarList)
    {
        PhysicalCharacteristics Table;
        Table.ObjType = BASIC[i].first;
        LoadStar(BASIC, i, OrbitTable, &Table);
        PhysicalTable.insert({i, Table});
    }

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList))
    {
        PhysicalCharacteristics Table;
        Table.ObjType = BASIC[i].first;
        LoadPlanet(BASIC, i, OrbitTable, StarList, StaticPosTable, PhysicalTable, &Table);
        auto ParentBody = std::find_if(SystemTable.begin(), SystemTable.end(), 
            [i](SystemType::value_type v)
        {
            return std::find(v.second.begin(), v.second.end(), i) != v.second.end();
        });
        std::vector<int> IsStarBarycenter;
        std::ranges::set_intersection(StarList, SystemTable[ParentBody->first], std::back_inserter(IsStarBarycenter));
        if (BaryenterList.contains(ParentBody->first) && IsStarBarycenter.empty())
        {
            for (auto j : SystemTable[ParentBody->first])
            {
                if (i == j) {continue;}
                Table.SubSystemsList.push_back(IDENT[j].front());
            }
        }
        else
        {
            for (auto j : SystemTable[i])
            {
                Table.SubSystemsList.push_back(IDENT[j].front());
            }
        }
        PhysicalTable.insert({i, Table});
    }

    for (auto i : std::views::concat(SatelliteList, MinorPlanetList, CometList))
    {
        PhysicalCharacteristics Table;
        Table.ObjType = BASIC[i].first;
        LoadPlanet(BASIC, i, OrbitTable, StarList, StaticPosTable, PhysicalTable, &Table);
        PhysicalTable.insert({i, Table});
    }

    auto gbuffer_basic_End = std::chrono::high_resolution_clock::now();
    auto gbuffer_basic_Delay = std::chrono::duration_cast<std::chrono::seconds>(gbuffer_basic_End - gbuffer_basic_Start);
    
    spdlog::info("完成（已处理 {} 个物体，延迟：{} s）", PhysicalTable.size(), gbuffer_basic_Delay.count());

    spdlog::info("生成物理数据表...");

    PhysicalCharTableType Result;
    for (auto i : StarList)
    {
        PhysicalCharTableType::mapped_type Dst;
        Dst["Type"] = PhysicalTable[i].ObjType;
        Dst["Class"] = PhysicalTable[i].Class;
        Dst["MeanRadius"] = PhysicalTable[i].MeanRadius;
        Dst["Dimensions"] = PhysicalTable[i].Dimensions;
        Dst["Flattening"] = PhysicalTable[i].Flattening;
        Dst["Circumference"] = PhysicalTable[i].Circumference;
        Dst["SurfaceArea"] = PhysicalTable[i].SurfaceArea;
        Dst["Volume"] = PhysicalTable[i].Volume;
        Dst["Mass"] = PhysicalTable[i].Mass;
        Dst["MeanDensity"] = PhysicalTable[i].MeanDensity;
        Dst["Age"] = PhysicalTable[i].Age;
        Dst["SurfaceGravity"] = PhysicalTable[i].SurfaceGravity;
        Dst["MomentOfInertiaFactor"] = PhysicalTable[i].MomentOfInertiaFactor;
        Dst["EscapeVelocity"] = PhysicalTable[i].EscapeVelocity;
        Dst["SiderealRotationPeriod"] = PhysicalTable[i].SiderealRotationPeriod;
        Dst["EquatorialRotationVelocity"] = PhysicalTable[i].EquatorialRotationVelocity;
        Dst["AxialTilt"] = PhysicalTable[i].AxialTilt;
        Dst["Luminosity"] = PhysicalTable[i].Luminosity;
        Dst["Temperature"] = PhysicalTable[i].Temperature;
        Result.insert({i, Dst});
    }

    for (auto i : std::views::concat(PlanetList, DwarfPlanetList))
    {
        PhysicalCharTableType::mapped_type Dst;
        Dst["Type"] = PhysicalTable[i].ObjType;
        Dst["Class"] = PhysicalTable[i].Class;
        Dst["MeanRadius"] = PhysicalTable[i].MeanRadius;
        Dst["Dimensions"] = PhysicalTable[i].Dimensions;
        Dst["Flattening"] = PhysicalTable[i].Flattening;
        Dst["Circumference"] = PhysicalTable[i].Circumference;
        Dst["SurfaceArea"] = PhysicalTable[i].SurfaceArea;
        Dst["Volume"] = PhysicalTable[i].Volume;
        Dst["Mass"] = PhysicalTable[i].Mass;
        Dst["MeanDensity"] = PhysicalTable[i].MeanDensity;
        Dst["SurfaceGravity"] = PhysicalTable[i].SurfaceGravity;
        Dst["MomentOfInertiaFactor"] = PhysicalTable[i].MomentOfInertiaFactor;
        Dst["EscapeVelocity"] = PhysicalTable[i].EscapeVelocity;
        Dst["SynodicRotationPeriod"] = PhysicalTable[i].SynodicRotationPeriod;
        Dst["SiderealRotationPeriod"] = PhysicalTable[i].SiderealRotationPeriod;
        Dst["EquatorialRotationVelocity"] = PhysicalTable[i].EquatorialRotationVelocity;
        Dst["AxialTilt"] = PhysicalTable[i].AxialTilt;
        Dst["Albedo"] = PhysicalTable[i].Albedo;
        Dst["Temperature"] = PhysicalTable[i].Temperature;
        Dst["RadiantFlux"] = PhysicalTable[i].RadiantFlux;
        Dst["ESI"] = PhysicalTable[i].ESI;
        Dst["SubSystemsList"] = PhysicalTable[i].SubSystemsList;
        Result.insert({i, Dst});
    }

    for (auto i : std::views::concat(SatelliteList, MinorPlanetList, CometList))
    {
        PhysicalCharTableType::mapped_type Dst;
        Dst["Type"] = PhysicalTable[i].ObjType;
        Dst["Class"] = PhysicalTable[i].Class;
        Dst["MeanRadius"] = PhysicalTable[i].MeanRadius;
        Dst["Dimensions"] = PhysicalTable[i].Dimensions;
        Dst["Flattening"] = PhysicalTable[i].Flattening;
        Dst["Circumference"] = PhysicalTable[i].Circumference;
        Dst["SurfaceArea"] = PhysicalTable[i].SurfaceArea;
        Dst["Volume"] = PhysicalTable[i].Volume;
        Dst["Mass"] = PhysicalTable[i].Mass;
        Dst["MeanDensity"] = PhysicalTable[i].MeanDensity;
        Dst["SurfaceGravity"] = PhysicalTable[i].SurfaceGravity;
        Dst["MomentOfInertiaFactor"] = PhysicalTable[i].MomentOfInertiaFactor;
        Dst["EscapeVelocity"] = PhysicalTable[i].EscapeVelocity;
        Dst["SynodicRotationPeriod"] = PhysicalTable[i].SynodicRotationPeriod;
        Dst["SiderealRotationPeriod"] = PhysicalTable[i].SiderealRotationPeriod;
        Dst["EquatorialRotationVelocity"] = PhysicalTable[i].EquatorialRotationVelocity;
        Dst["AxialTilt"] = PhysicalTable[i].AxialTilt;
        Dst["Albedo"] = PhysicalTable[i].Albedo;
        Dst["Temperature"] = PhysicalTable[i].Temperature;
        Dst["RadiantFlux"] = PhysicalTable[i].RadiantFlux;
        if (BASIC.at(i).first == "Moon")
        {
            Dst["ESI"] = PhysicalTable[i].ESI;
        }
        Result.insert({i, Dst});
    }

    spdlog::info("完成");

    return Result;
}