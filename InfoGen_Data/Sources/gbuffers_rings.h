#pragma once

#ifndef __GBUF_RINGS__
#define __GBUF_RINGS__

#include <flat_map>
#include <map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct RingsDetails
{
    SEString  Type; // "CircumPlanetaryDisc", "Halo", "SaturnELike", "JupiterTransparent", "NeptuneNarrow", "UranusSparse", "Colorful"
    SEReal    InnerRadius;
    SEReal    Width;
    SEReal    Thickness;
    SEReal    RockMaxSize;
    SEInteger ObjectCount; // 仅Type为"CircumplanetaryDisc"时有效
};

struct Rings
{
    std::vector<RingsDetails> Details;
};

using RingsPrevTableType = std::flat_map<OIDType, Rings>;

extern RingsPrevTableType RingsPrevTable;

RingsTableType gbuffers_rings();

#endif