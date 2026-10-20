#pragma once

#ifndef __GBUF_ORBIT__
#define __GBUF_ORBIT__

#include <pybind11/pybind11.h>
#include <limits>

#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"

namespace py = pybind11;

struct OrbitCharacteristics
{
    SEString  RefPlane{};

    // 当前状态向量
    SEReal    GravParam = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    Time = std::numeric_limits<SEReal>::quiet_NaN();
    SEVec3    Position = SEVec3::Constant(std::numeric_limits<SEReal>::quiet_NaN());
    SEVec3    Velocity = SEVec3::Constant(std::numeric_limits<SEReal>::quiet_NaN());

    // 从原始数据读取的轨道根数
    SEReal    Period = std::numeric_limits<SEReal>::quiet_NaN(); // sec
    SEReal    PericenterDist = std::numeric_limits<SEReal>::quiet_NaN(); // m
    SEReal    AphelionDist = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    SemiMajorAxis = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    Eccentricity = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    Inclination = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    AscendingNode = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    Epoch = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    ArgOfPericenter = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    MeanAnomaly = std::numeric_limits<SEReal>::quiet_NaN();

    // 相对于黄道面的轨道面
    SEReal    AxialTilt = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    Equinox = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    InclinationEcliptic = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    AscNodeEcliptic = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    ArgOfPeriEcliptic = std::numeric_limits<SEReal>::quiet_NaN();

    // 会合周期
    SEReal    SynodicOrbitalPeriod = std::numeric_limits<SEReal>::quiet_NaN();

    // 双星轨道根数
    SEBoolean BinaryOrbit = false;
    SEBoolean IsPrimary = false;
    SEString  Primary{};
    SEString  Companion{};
    SEReal    BPeriod = std::numeric_limits<SEReal>::quiet_NaN(); // sec
    SEReal    BPericenterDist = std::numeric_limits<SEReal>::quiet_NaN(); // m
    SEReal    BAphelionDist = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BSemiMajorAxis = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BEccentricity = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BInclination = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BAscendingNode = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BEpoch = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BArgOfPericenter = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BMeanAnomaly = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BInclinationEcliptic = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BAscNodeEcliptic = std::numeric_limits<SEReal>::quiet_NaN();
    SEReal    BArgOfPeriEcliptic = std::numeric_limits<SEReal>::quiet_NaN();
};

using OrbitTableType = std::flat_map<OIDType, OrbitCharacteristics>;

extern OrbitTableType OrbitTable;

OrbitCharTableType gbuffer_orbit();

#endif
