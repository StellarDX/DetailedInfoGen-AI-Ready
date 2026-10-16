#pragma once

#ifndef __GBUF_ATMOSPHERE__
#define __GBUF_ATMOSPHERE__

#include <flat_map>
#include <map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct Atmosphere // 气候字段不单独建表，合并到大气表中（TODO，热力学的水实在太深了）
{
    double SurfacePressure;
    double ScaleHeight;
    double Density;
    double GlobalWindSpeed;
    double MaxTemperature;
    double MeanTemperature;
    double MinTemperature;
    std::map<std::string, double> CompositionByVolume;
};

using AtmospherePrevTableType = std::flat_map<OIDType, Atmosphere>;

extern AtmospherePrevTableType AtmospherePrevTable;

AtmosphereTableType gbuffer_atmosphere();

#endif