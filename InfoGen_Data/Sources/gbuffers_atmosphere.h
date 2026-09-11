#pragma once

#ifndef __GBUF_ATMOSPHERE__
#define __GBUF_ATMOSPHERE__

#include <flat_map>
#include <map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct Atmosphere
{
    double SurfacePressure;
    double ScaleHeight;
    std::map<std::string, double> CompositionByVolume;
};

using AtmospherePrevTableType = std::flat_map<OIDType, Atmosphere>;

extern AtmospherePrevTableType AtmospherePrevTable;

AtmosphereTableType gbuffer_atmosphere();

#endif