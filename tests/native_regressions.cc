#define _USE_MATH_DEFINES
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdexcept>

#include "gbuffers_orbit.h"
#include "gbuffers_basic.h"
#include "composite2.h"
#include "CONSTANTS.INC"
#include "Keplerian/Keplerian.h"
#include "gbuffers_hydrosphere.h"
#include "gbuffers_atmosphere.h"
#include "minobjs.h"
#include "final.h"

// Exercise production functions without extending the Python/public API.
void OrbitParamsToStateVectors(OrbitTableType*, const SystemType&, OIDType);
void ComputeStaticPos(const SystemType&, OIDType, const OrbitTableType&, StaticPosTableType*);
void ComputePlanetTemperature(const SETable&, OIDType, const ObjectListType&,
    const StaticPosTableType&, const PhysicalTableType&, PhysicalCharacteristics*);
void ComputeESI(OIDType, PhysicalCharacteristics*);
void LoadRadiuses(const SETable&, PhysicalCharacteristics*);
void LoadBasicData(const SETable&, OIDType, const OrbitTableType&, PhysicalCharacteristics*);
void LoadOrbitParamsFromRawData(const BasicTableType&, const SystemType&, OIDType, OrbitTableType*);
void MakeEclipticalReferencedOrbit(OrbitTableType*, const SystemType&, OIDType);
void LoadOcean(const BasicTableType&, OIDType, OceanPrevTableType*);
void LoadAtmosphere(const BasicTableType&, OIDType, AtmospherePrevTableType*);

void TransferBarycenter(OrbitTableType*, const SystemType&, OIDType, const BasicTableType&);

void Require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}

void Near(double actual, double expected)
{
    Require(std::isfinite(actual), "expected a finite value");
    if (std::abs(actual - expected) > 1e-10 * std::max(1., std::abs(expected)))
    {
        std::ostringstream message;
        message << std::setprecision(17) << "numerical mismatch: actual=" << actual << " expected=" << expected;
        throw std::runtime_error(message.str());
    }
}

void Initialization()
{
    for (unsigned char pattern : {0xA5, 0x5A})
    {
        alignas(OrbitCharacteristics) unsigned char storage[sizeof(OrbitCharacteristics)];
        std::memset(storage, pattern, sizeof storage);
        auto destroy = [](OrbitCharacteristics* p) { p->~OrbitCharacteristics(); };
        std::unique_ptr<OrbitCharacteristics, decltype(destroy)> row(
            new(storage) OrbitCharacteristics, destroy);
        bool expected = false;
        // Compare bytes first: never evaluate an indeterminate pre-fix bool.
        Require(std::memcmp(&row->BinaryOrbit, &expected, sizeof(bool)) == 0, "BinaryOrbit must initialize false");
        Require(std::memcmp(&row->IsPrimary, &expected, sizeof(bool)) == 0, "IsPrimary must initialize false");
        Require(row->RefPlane.empty() && row->Primary.empty() && row->Companion.empty(), "names must initialize empty");
        const std::array scalars{
            &OrbitCharacteristics::GravParam, &OrbitCharacteristics::Time,
            &OrbitCharacteristics::Period, &OrbitCharacteristics::PericenterDist,
            &OrbitCharacteristics::AphelionDist, &OrbitCharacteristics::SemiMajorAxis,
            &OrbitCharacteristics::Eccentricity, &OrbitCharacteristics::Inclination,
            &OrbitCharacteristics::AscendingNode, &OrbitCharacteristics::Epoch,
            &OrbitCharacteristics::ArgOfPericenter, &OrbitCharacteristics::MeanAnomaly,
            &OrbitCharacteristics::AxialTilt, &OrbitCharacteristics::Equinox,
            &OrbitCharacteristics::InclinationEcliptic, &OrbitCharacteristics::AscNodeEcliptic,
            &OrbitCharacteristics::ArgOfPeriEcliptic, &OrbitCharacteristics::SynodicOrbitalPeriod,
            &OrbitCharacteristics::BPeriod, &OrbitCharacteristics::BPericenterDist,
            &OrbitCharacteristics::BAphelionDist, &OrbitCharacteristics::BSemiMajorAxis,
            &OrbitCharacteristics::BEccentricity, &OrbitCharacteristics::BInclination,
            &OrbitCharacteristics::BAscendingNode, &OrbitCharacteristics::BEpoch,
            &OrbitCharacteristics::BArgOfPericenter, &OrbitCharacteristics::BMeanAnomaly,
            &OrbitCharacteristics::BInclinationEcliptic, &OrbitCharacteristics::BAscNodeEcliptic,
            &OrbitCharacteristics::BArgOfPeriEcliptic};
        for (auto member : scalars) Require(std::isnan(row.get()->*member), "unknown scalar must initialize NaN");
        Require(row->Position.array().isNaN().all(), "Position must initialize NaN");
        Require(row->Velocity.array().isNaN().all(), "Velocity must initialize NaN");
    }
}

void Position()
{
    for (bool absolute : {true, false})
    {
        IsAbsoluteOrbitParams = absolute;
        OrbitCharacteristics row{};
        row.Position = {-7, -8, -9};
        row.PericenterDist = 100.; row.Eccentricity = 0.; row.MeanAnomaly = 0.;
        row.Inclination = row.InclinationEcliptic = 0.;
        row.AscendingNode = row.AscNodeEcliptic = 0.;
        row.ArgOfPericenter = row.ArgOfPeriEcliptic = 0.;
        OrbitTableType table{{1, row}};
        OrbitParamsToStateVectors(&table, {}, 0);
        Near(table.at(1).Position.x(), 100.);
        Near(table.at(1).Position.y(), 0.); Near(table.at(1).Position.z(), 0.);
        row.ArgOfPericenter = row.ArgOfPeriEcliptic = 90.;
        table.at(1) = row;
        OrbitParamsToStateVectors(&table, {}, 0);
        Near(table.at(1).Position.x(), 0.); Near(table.at(1).Position.y(), 100.);
        table[2].Position = {0., 25., 0.};
        SystemType system{{0, {1}}, {1, {2}}, {2, {}}};
        StaticPosTableType positions;
        ComputeStaticPos(system, 0, table, &positions);
        Near(positions.at(0).norm(), 0.); Near(positions.at(2).y(), 125.);
    }
}

void Temperature()
{
    SETable raw{{"Albedo", {SEObject(0.)}}};
    StaticPosTableType pos{{0, {0.,0.,0.}}, {1, {1.,0.,0.}}, {2, {-1.,0.,0.}}};
    PhysicalTableType stars;
    stars[1].Luminosity = 4. * M_PI * 4. * StefanBoltzmann * std::pow(300., 4);
    stars[2].Luminosity = stars[1].Luminosity;
    PhysicalCharacteristics row{};
    ComputePlanetTemperature(raw, 0, {1}, pos, stars, &row);
    Near(row.Temperature, 300.);
    double singleFlux = row.RadiantFlux;
    ComputePlanetTemperature(raw, 0, {1,2}, pos, stars, &row);
    Near(row.RadiantFlux, singleFlux * 2.);
    Near(row.Temperature, 300. * std::pow(2., 0.25));
    raw.emplace("Teff", SEValues{SEObject(10.)});
    raw.emplace("EndogenousHeating", SEValues{SEObject(5.)});
    raw.emplace("Atmosphere", SEValues{SEObject(SETable{{"Greenhouse", {SEObject(7.)}}})});
    ComputePlanetTemperature(raw, 0, {1,2}, pos, stars, &row);
    Near(row.Temperature, 300. * std::pow(2., 0.25) + 22.);
    ComputePlanetTemperature(raw, 0, {}, pos, stars, &row);
    Near(row.RadiantFlux, 0.); Near(row.Temperature, 22.);
}

void ESI()
{
    ESIEstimator = "Extrasolar";
    PhysicalCharacteristics row{};
    row.RadiantFlux = SolarConstant; row.MeanRadius = EarthRadius; row.ESI = -123.;
    ComputeESI(1, &row); Near(row.ESI, 1.);
    ESIEstimator = "SolarSys";
    row.MeanDensity = EarthDensity; row.EscapeVelocity = EarthEscapeVel; row.Temperature = EarthTemperature;
    ComputeESI(1, &row); Near(row.ESI, 1.);
}

void RotationRadius()
{
    for (double flattening : {0., 0.2})
    {
        SETable raw{{"Radius", {SEObject(1.)}}, {"Mass", {SEObject(1.)}},
            {"RotationPeriod", {SEObject(1.)}}, {"Oblateness", {SEObject(flattening)}}};
        PhysicalCharacteristics row{};
        LoadBasicData(raw, 0, {}, &row);
        Near(row.EquatorialRotationVelocity, 2. * M_PI * 1000. / 3600.);
    }
}

void KeplerBoundary()
{
    // The real fixture contains this negative anomaly at e > 0.99.
    for (double e : {0.991083812, 0.9999})
    {
        for (double mean : {-725., -360., -2.85701972, -0.00001, 0., 0.00001, 2.85701972, 360., 725.})
        {
            double eccentric = InverseKeplerianEquation(e, mean);
            Require(std::isfinite(eccentric), "Kepler solution must be finite");
            double rad = eccentric * M_PI / 180.;
            Near((rad - e * std::sin(rad)) * 180. / M_PI, mean);
            Near(InverseKeplerianEquation(e, mean + 360.), eccentric + 360.);
            Near(InverseKeplerianEquation(e, -mean), -eccentric);
        }
    }
}

void ClearPythonTables()
{
    OrbitalCharacteristicsTable.clear(); PhysicalCharacteristicsTable.clear();
    AtmosphereTable.clear(); HydrosphereTable.clear(); BiosphereTable.clear(); RingsTable.clear();
}

void SequentialImportFidelity()
{
    auto directory = std::filesystem::temp_directory_path() /
        ("infogen-p1-sequential-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(directory);
    struct Cleanup { std::filesystem::path path; ~Cleanup() { std::filesystem::remove(path / "first.sc"); std::filesystem::remove(path / "second.sc"); std::filesystem::remove(path); } } cleanup{directory};
    auto write = [&](const std::string& name, double radius)
    {
        auto path = directory / (name + ".sc");
        std::ofstream file(path);
        file << "Star \"" << name << "\" { ParentBody \"" << name << "\" Class \"G2V\" Radius "
            << radius << " MassSol 1 LumBol 1 Teff 5785 RotationPeriod 24 Orbit { RefPlane \"Static\" SemiMajorAxis 0 Period 0 } }";
        return path;
    };
    auto firstPath = write("first",1.); auto secondPath = write("second",2.);
    py::dict args; args["input"] = firstPath.string();
    auto first = InfoGen_Main(py::kwargs(args));
    Require(first["MainID"].cast<std::string>() == "first", "first import differs");
    args["input"] = secondPath.string();
    auto second = InfoGen_Main(py::kwargs(args));
    Require(second["MainID"].cast<std::string>() == "second", "previous import contaminated source identity");
    Near(second["Objects"]["second"]["PhysicalCharacteristics"]["MeanRadius"].cast<double>(),2000.);
    Near(first["Objects"]["first"]["PhysicalCharacteristics"]["MeanRadius"].cast<double>(),1000.);
    auto firstDimensions = first["Objects"]["first"]["PhysicalCharacteristics"]["Dimensions"].cast<SEVec3>();
    Near(firstDimensions.x(),2000.);
}

void BinaryPair(const std::string& test)
{
    OrbitTableType table;
    SystemType sys{{0,{1,2}},{1,{}},{2,{}}};
    BasicTableType basic;
    for(OIDType id=0;id<=2;++id) basic[id]={id==0?"Barycenter":"Planet",{SEObject(std::to_string(id)),SEObject(SETable{})}};
    table[0].Period=999.; table[0].SemiMajorAxis=100.;
    for (OIDType id=1;id<=2;++id)
    {
        auto& o=table[id]; o.BinaryOrbit=false; o.PericenterDist=id; o.SemiMajorAxis=id*2.; o.AphelionDist=id*3.;
        o.Inclination=10.; o.AscendingNode=20.; o.MeanAnomaly=30.; o.ArgOfPericenter=id==1?0.:180.;
        o.Period=100.; o.Eccentricity=.5; o.Epoch=2451545.;
    }
    bool valid=false;
    if(test=="binary-negative") table[2].Inclination=-40.;
    if(test=="binary-apsis") table[2].ArgOfPericenter=20.;
    if(test=="binary-wrap") {table[2].AscendingNode=380.;table[2].ArgOfPericenter=540.;valid=true;}
    if(test=="binary-rounded") {table[2].Period*=1.+3E-9;valid=true;}
    if(test=="binary-small-period") {table[1].Period=1E-12;table[2].Period=2E-12;}
    if(test=="binary-period") table[2].Period=200.;
    if(test=="binary-frame") {table[1].RefPlane="Ecliptic";table[2].RefPlane="Equator";}
    if(test=="binary-missing") table[2].AscendingNode=std::numeric_limits<double>::quiet_NaN();
    if(test=="binary-epoch") table[2].Epoch+=1.;
    if(test=="binary-eccentricity") table[2].Eccentricity=.7;
    if(test=="binary-retrograde") {table[1].Inclination=table[2].Inclination=120.;table[1].Period=table[2].Period=-100.;valid=true;}
    if(test=="binary-tie") {table[2].PericenterDist=1.;valid=true;}
    if(test=="binary-empty") sys[0]={};
    if(test=="binary-single") {sys[0]={1};}
    TransferBarycenter(&table,sys,0,basic);
    Require(table[1].BinaryOrbit==valid && table[2].BinaryOrbit==valid,"binary classification differs from reproducible geometry");
    if(valid) { Near(table[1].BPeriod,999.); Near(table[2].BPeriod,test=="binary-retrograde"?-100.:100.); Near(table[2].BSemiMajorAxis,6.); }
}

void ScienceCoplanarOrientation()
{
    OrbitTableType table;
    SystemType sys{{0,{1,2}},{1,{}},{2,{}}};
    table[0].AxialTilt=0.; table[0].Equinox=0.;
    for(OIDType id=1;id<=2;++id)
    {
        auto& o=table[id];o.RefPlane="Ecliptic";o.PericenterDist=100.;o.Eccentricity=0.;o.MeanAnomaly=0.;
        o.Inclination=id==1?0.:180.;o.AscendingNode=30.;o.ArgOfPericenter=90.;
    }
    MakeEclipticalReferencedOrbit(&table,sys,0);
    IsAbsoluteOrbitParams=false;OrbitParamsToStateVectors(&table,sys,0);
    Near(table[1].Position.x(),-50.);Near(table[1].Position.y(),50.*std::sqrt(3.));
    Near(table[2].InclinationEcliptic,180.);
    Near(table[2].Position.x(),50.);Near(table[2].Position.y(),-50.*std::sqrt(3.));
}

void SynchronousTemperature()
{
    SETable raw{{"Albedo", {SEObject(0.)}}};
    StaticPosTableType positions{{0,{0.,0.,0.}}, {1,{1.,0.,0.}}};
    PhysicalTableType stars;
    stars[1].Luminosity = 4. * M_PI * 4. * StefanBoltzmann * std::pow(300.,4);
    SystemTable = {{1,{0}}, {0,{}}};
    PhysicalCharacteristics row{};
    row.SynodicRotationPeriod = std::numeric_limits<double>::infinity();
    ComputePlanetTemperature(raw,0,{1},positions,stars,&row);
    Near(row.Temperature,300.*std::pow(2.,0.25));
}

int main(int argc, char** argv)
{
    py::scoped_interpreter interpreter;
    try
    {
        Require(argc == 2, "missing test case");
        std::string test = argv[1];
        if(test == "initialization") Initialization();
        else if(test == "position") Position();
        else if(test == "temperature") Temperature();
        else if(test == "synchronous-temperature") SynchronousTemperature();
        else if(test == "esi") ESI();
        else if(test == "rotation") RotationRadius();
        else if(test == "kepler") KeplerBoundary();
        else if(test == "coplanar") ScienceCoplanarOrientation();
        else if(test == "sequential") SequentialImportFidelity();
        else if(test.starts_with("binary-")) BinaryPair(test);
        else throw std::runtime_error("unknown test case");
        ClearPythonTables();
        std::cout << "PASS " << test << '\n';
        return 0;
    }
    catch(const std::exception& error)
    {
        ClearPythonTables();
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
