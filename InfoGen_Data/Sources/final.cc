#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "composite.h"
#include "final.h"

std::string InputFile;
uint16_t IFCodePage;

SETable RawData;

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

    try
    {
        Composite();
    }
    catch(const std::exception& e)
    {
        spdlog::critical("数据处理时出现错误：\n{}", e.what());
        return ReturnType();
    }

    return ReturnType();
}

PYBIND11_MODULE(InfoGen, InfoGenPythonExport)
{
    InfoGenPythonExport.doc() = "InfoGen C++ Main function";
    InfoGenPythonExport.def("InfoGen_Main", &InfoGen_Main, "InfoGen C++ Main function");
}

