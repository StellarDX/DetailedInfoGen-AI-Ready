#pragma once

#ifndef __GBUF_OCEAN__
#define __GBUF_OCEAN__

#include <flat_map>
#include <map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct Ocean
{
    double Height;
    std::map<std::string, double> CompositionByVolume;
};

using OceanPrevTableType = std::flat_map<OIDType, Ocean>;

extern OceanPrevTableType OceanPrevTable;

HydrosphereTableType gbuffers_hydrosphere();

#endif