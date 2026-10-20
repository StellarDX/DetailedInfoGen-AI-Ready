#include <cstdint>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "composite.h"
#include "composite1.h"
#include "composite2.h"
#include "final.h"
#include "gbuffers_orbit.h"
#include "gbuffers_basic.h"
#include "gbuffers_atmosphere.h"
#include "gbuffers_hydrosphere.h"
#include "gbuffers_biosphere.h"
#include "gbuffers_rings.h"

std::string InputFile;
uint16_t    IFCodePage;
bool        IsAbsoluteOrbitParams;
double      CommonPlaneThreshold;
std::string ESIEstimator;
bool        SortSystem;
std::string AsteroidsTablePred;
uint64_t    AsteroidsTableLimit;

SETable     RawData;

namespace
{
// Python-valued scratch tables must release their references while the caller's
// interpreter/GIL is alive, not during DLL/global destruction after shutdown.
struct PythonBufferScope
{
    ~PythonBufferScope()
    {
        OrbitalCharacteristicsTable.clear();
        PhysicalCharacteristicsTable.clear();
        AtmosphereTable.clear();
        HydrosphereTable.clear();
        BiosphereTable.clear();
        RingsTable.clear();
    }
};

void ResetImportState()
{
    // These are per-import scratch data, not a cross-file cache. Retaining them
    // lets flat_map::insert silently keep a previous object's identity/values.
    RawData.clear(); BASIC.clear(); IDENT.clear(); SystemTable.clear();
    BarycenterID = 0;
    BaryenterList.clear(); StarList.clear(); PlanetList.clear(); DwarfPlanetList.clear();
    SatelliteList.clear(); MinorPlanetList.clear(); CometList.clear(); MinorObjectList.clear();
    OrbitTable.clear(); StaticPosTable.clear(); PhysicalTable.clear();
    AtmospherePrevTable.clear(); OceanPrevTable.clear(); LifePrevTable.clear(); RingsPrevTable.clear();
}
}

template<typename Tp>
Tp GetArgFromPy(const py::kwargs& args, char const* Key, Tp Default)
{
    if (args.contains(Key))
    {
        Default = args[Key].cast<Tp>();
    }
    return Default;
}

void LoadArguments(const py::kwargs& args)
{
    InputFile = GetArgFromPy(args, "input", std::string());
    IFCodePage = GetArgFromPy(args, "code_page", 65001);
    IsAbsoluteOrbitParams = GetArgFromPy(args, "absolute_orbit", false);
    CommonPlaneThreshold = GetArgFromPy(args, "common_plane_threshold", 9.);
    ESIEstimator = GetArgFromPy(args, "esi_estimator", std::string("SolarSys"));
    SortSystem = GetArgFromPy(args, "sort_system", false);
    AsteroidsTablePred = GetArgFromPy(args, "exceptional_asteroids_pred", std::string("LargestByDiameter"));
    AsteroidsTableLimit = GetArgFromPy(args, "exceptional_asteroids_limit", 50);
}

ReturnType InfoGen_Main(const py::kwargs& args)
{
    PythonBufferScope PythonBuffers;
    ResetImportState();
    LoadArguments(args);
    if (InputFile.empty())[[unlikely]]
    {
        spdlog::critical("输入文件路径为空");
        return ReturnType();
    }

    spdlog::info("输入文件：{}", InputFile);

    std::string ParserReport;
    ISCStream is;
    auto ParseStart = std::chrono::high_resolution_clock::now();
    try {is = ParseFile(InputFile, &ParserReport);}
    catch(const std::exception& e)
    {
        spdlog::critical("分析失败：\n{}", e.what());
        return ReturnType();
    }
    auto ParseEnd = std::chrono::high_resolution_clock::now();
    auto ParseDelay = std::chrono::duration_cast<std::chrono::milliseconds>(ParseEnd - ParseStart);

    RawData = is.GetObject().As<SETable>();

    spdlog::info("分析已完成，读取了 {} 个物体，延迟：{} ms\n{}", 
        RawData.size(), ParseDelay.count(), ParserReport);

    ReturnType Result;
    try
    {
        Composite(&Result);
        Composite1(&Result);
        Composite2(&Result);
    }
    catch(const std::exception& e)
    {
        spdlog::critical("数据处理时出现错误：\n{}", e.what());
        return ReturnType();
    }

    return Result;
}

PYBIND11_MODULE(InfoGen, InfoGenPythonExport)
{
    InfoGenPythonExport.doc() = "InfoGen C++ Main function";
    InfoGenPythonExport.def("InfoGen_Main", &InfoGen_Main, "InfoGen C++ Main function");
}

