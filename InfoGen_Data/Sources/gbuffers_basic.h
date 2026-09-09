#pragma once

#ifndef __GBUF_BASIC__
#define __GBUF_BASIC__

#include <flat_map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct PhysicalCharacteristics
{
    std::string ObjType;
    std::string Class;
    double MeanRadius;
    SEVec3 Dimensions;
    SEVec3 Flattening;
    SEVec2 Circumference; // {Equatorial, Meridional}
    double SurfaceArea;
    double Volume;
    double Mass;
    double MeanDensity;
    double Age;
    double SurfaceGravity;
    double MomentOfInertiaFactor;
    double EscapeVelocity;
    double SynodicRotationPeriod;
    double SiderealRotationPeriod;
    double EquatorialRotationVelocity;
    double AxialTilt;
    SEVec2 Albedo;
    double Luminosity;
    double Temperature;
};

using PhysicalTableType = std::flat_map<OIDType, PhysicalCharacteristics>;

extern PhysicalTableType PhysicalTable;

PhysicalCharTableType gbuffer_basic();

#endif