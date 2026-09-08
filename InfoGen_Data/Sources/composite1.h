#pragma once

#ifndef __COMPOSITE1__
#define __COMPOSITE1__

#include "composite.h"
#include "final.h"

#include <flat_set>

using StaticPosTableType = std::flat_map<OIDType, SEVec3>;
using PhysicalCharTableType = std::flat_map<OIDType, py::dict>;

extern StaticPosTableType StaticPosTable;

void Composite1(ReturnType* Result);

#endif