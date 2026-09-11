#pragma once

#ifndef __GBUF_BIO__
#define __GBUF_BIO__

#include <flat_map>
#include <map>

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite1.h"

namespace py = pybind11;

struct Life
{
    std::string Class;
    std::string Type;
    std::vector<std::string> Biome;
};

using LifePrevTableType = std::flat_map<OIDType, Life>;

extern LifePrevTableType LifePrevTable;

BiosphereTableType gbuffers_biosphere();

#endif