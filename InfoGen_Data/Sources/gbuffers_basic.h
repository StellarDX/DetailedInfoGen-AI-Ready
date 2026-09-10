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
    double Age; // 仅Star启用
    double SurfaceGravity;
    double MomentOfInertiaFactor;
    double EscapeVelocity;
    double SynodicRotationPeriod; // 除Star外的其他物体启用
    double SiderealRotationPeriod;
    double EquatorialRotationVelocity;
    double AxialTilt;
    SEVec2 Albedo; // 除Star外的其他物体启用
    double Luminosity; // 仅Star启用
    double Temperature;

    std::vector<std::string> SubSystemsList; // 仅Planet和DwarfPlanet启用
};

using PhysicalTableType = std::flat_map<OIDType, PhysicalCharacteristics>;

extern PhysicalTableType PhysicalTable;

PhysicalCharTableType gbuffer_basic();

#endif