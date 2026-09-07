#pragma once

#ifndef __GBUF_ORBIT__
#define __GBUF_ORBIT__

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"

namespace py = pybind11;

struct OrbitCharacteristics
{
    SEString RefPlane;

    // 当前状态向量
    SEReal   GravParam;
    SEReal   Time;
    SEVec3   Position;
    SEVec3   Velocity;

    // 从原始数据读取的轨道根数
    SEReal   Period; // sec
    SEReal   PericenterDist; // m (不用半长轴的原因是抛物线轨道半长轴为inf，而近日点距离始终有值)
    SEReal   Eccentricity;
    SEReal   Inclination;
    SEReal   AscendingNode;
    SEReal   ArgOfPericenter;
    SEReal   MeanAnomaly;

    // 相对于黄道面的轨道面
    SEReal   AxialTilt;
    SEReal   Equinox;
    SEReal   InclinationEcliptic;
    SEReal   AscNodeEcliptic;
    SEReal   ArgOfPeriEcliptic;
};

using OrbitTableType = std::flat_map<OIDType, OrbitCharacteristics>;

extern OrbitTableType OrbitTable;

OrbitCharTableType gbuffer_orbit();

#endif