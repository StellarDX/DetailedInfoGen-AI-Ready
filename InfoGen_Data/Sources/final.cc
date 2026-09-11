#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "composite.h"
#include "composite1.h"
#include "composite2.h"
#include "final.h"

std::string InputFile;
uint16_t    IFCodePage;
bool        IsAbsoluteOrbitParams;
double      CommonPlaneThreshold;
std::string ESIEstimator;
bool        SortSystem;

SETable     RawData;

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
    InputFile = GetArgFromPy(args, "file", std::string());
    IFCodePage = GetArgFromPy(args, "code_page", 65001);
    IsAbsoluteOrbitParams = GetArgFromPy(args, "absolute_orbit", false);
    CommonPlaneThreshold = GetArgFromPy(args, "common_plane_threshold", 9.);
    ESIEstimator = GetArgFromPy(args, "esi_estimator", std::string("SolarSys"));
    SortSystem = GetArgFromPy(args, "sort_system", false);
}

ReturnType InfoGen_Main(const py::kwargs& args)
{
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

