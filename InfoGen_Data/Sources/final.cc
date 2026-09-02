// 测试编译

#include <iostream>
#include <fmt/format.h>
#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ANTLR/SELexer.h"
#include "Mini-NeoCSE/ANTLR/SEParser.h"
#include "Mini-NeoCSE/ISCStream.h"

#include "final.h"

namespace py = pybind11;

using ArgsType = py::dict;
using ReturnType = std::vector<std::string>;

int InfoGen_Main()
{

    return 0;
}

PYBIND11_MODULE(InfoGen, InfoGenPythonExport)
{
    InfoGenPythonExport.doc() = "InfoGen C++ Main function";
    InfoGenPythonExport.def("InfoGen_Main", &InfoGen_Main, "InfoGen C++ Main function");
}

