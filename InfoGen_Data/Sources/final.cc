#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

#include "Mini-NeoCSE/ANTLR/SELexer.h"
#include "Mini-NeoCSE/ANTLR/SEParser.h"
#include "Mini-NeoCSE/ISCStream.h"

#include "final.h"

namespace py = pybind11;

using ArgsType = py::kwargs;
using ReturnType = std::vector<std::string>;

template<typename Tp>
Tp GetArgFromPy(const py::kwargs& args, char const* Key, Tp Default)
{
    if (args.contains(Key))
    {
        Default = args[Key].cast<Tp>();
    }
    return Default;
}

ReturnType InfoGen_Main(const py::kwargs& args)
{
    std::string InputFile = GetArgFromPy(args, "file", std::string());
    if (InputFile.empty())[[unlikely]]
    {
        spdlog::critical("输入文件路径为空");
        return ReturnType();
    }
    uint16_t IFCodePage = GetArgFromPy(args, "code_page", 65001);

    spdlog::info("输入文件：{}", InputFile);

    std::string ParserReport;
    auto ParseStart = std::chrono::high_resolution_clock::now();
    ISCStream is = ParseFile(InputFile, &ParserReport);
    auto ParseEnd = std::chrono::high_resolution_clock::now();
    auto ParseDelay = std::chrono::duration_cast<std::chrono::milliseconds>(ParseEnd - ParseStart);

    SETable Table = is.GetObject().As<SETable>();

    spdlog::info("分析已完成，读取了 {} 个物体，延迟：{} ms", Table.size(), ParseDelay.count());
    std::cout << ParserReport << '\n';

    return ReturnType();
}

PYBIND11_MODULE(InfoGen, InfoGenPythonExport)
{
    InfoGenPythonExport.doc() = "InfoGen C++ Main function";
    InfoGenPythonExport.def("InfoGen_Main", &InfoGen_Main, "InfoGen C++ Main function");
}

