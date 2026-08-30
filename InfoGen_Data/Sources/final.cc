// 测试编译

#include <fmt/format.h>
#include <iostream>
#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"

#include "final.h"

namespace py = pybind11;

using ArgsType = py::dict;
using ReturnType = std::vector<std::string>;

int InfoGen_Main()
{
    std::cout << fmt::format("Hello World\n");
    return 0;
}

PYBIND11_MODULE(InfoGen, InfoGenPythonExport)
{
    InfoGenPythonExport.doc() = "Test";
    InfoGenPythonExport.def("InfoGen_Main", &InfoGen_Main, "Test Function");
}

