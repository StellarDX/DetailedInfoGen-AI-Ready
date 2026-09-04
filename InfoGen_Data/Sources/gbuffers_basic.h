#pragma once

#ifndef __GBUF_BASIC__
#define __GBUF_BASIC__

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"

namespace py = pybind11;

py::dict gbuffer_basic(const SETable& Table);

#endif