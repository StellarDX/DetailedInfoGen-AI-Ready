#pragma once

#ifndef __GBUF_ORBIT__
#define __GBUF_ORBIT__

#include <pybind11/pybind11.h>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"

namespace py = pybind11;

struct OrbitCharacteristics
{
    SEString  RefPlane;

    // 当前状态向量
    SEReal    GravParam;
    SEReal    Time;
    SEVec3    Position;
    SEVec3    Velocity;

    // 从原始数据读取的轨道根数
    SEReal    Period; // sec
    SEReal    PericenterDist; // m
    SEReal    AphelionDist;
    SEReal    SemiMajorAxis;
    SEReal    Eccentricity;
    SEReal    Inclination;
    SEReal    AscendingNode;
    SEReal    Epoch;
    SEReal    ArgOfPericenter;
    SEReal    MeanAnomaly;

    // 相对于黄道面的轨道面
    SEReal    AxialTilt;
    SEReal    Equinox;
    SEReal    InclinationEcliptic;
    SEReal    AscNodeEcliptic;
    SEReal    ArgOfPeriEcliptic;

    // 会合周期
    SEReal    SynodicOrbitalPeriod;

    // 双星轨道根数
    SEBoolean BinaryOrbit;
    SEBoolean IsPrimary;
    SEString  Primary;
    SEString  Companion;
    SEReal    BPeriod; // sec
    SEReal    BPericenterDist; // m
    SEReal    BAphelionDist;
    SEReal    BSemiMajorAxis;
    SEReal    BEccentricity;
    SEReal    BInclination;
    SEReal    BAscendingNode;
    SEReal    BEpoch;
    SEReal    BArgOfPericenter;
    SEReal    BMeanAnomaly;
    SEReal    BInclinationEcliptic;
    SEReal    BAscNodeEcliptic;
    SEReal    BArgOfPeriEcliptic;
};

using OrbitTableType = std::flat_map<OIDType, OrbitCharacteristics>;

extern OrbitTableType OrbitTable;

OrbitCharTableType gbuffer_orbit();

#endif