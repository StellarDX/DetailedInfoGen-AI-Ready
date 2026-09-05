#pragma once

#ifndef __FINAL__
#define __FINAL__

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>

#include "Mini-NeoCSE/ISCStream.h"

namespace py     = pybind11;

using ArgsType   = py::kwargs;
using ReturnType = py::dict;

extern std::string InputFile;
extern uint16_t    IFCodePage;
extern bool        IsAbsoluteOrbitParams;

extern SETable     RawData;

ReturnType InfoGen_Main(const py::kwargs& args);

#endif