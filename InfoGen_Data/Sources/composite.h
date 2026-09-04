#pragma once

#ifndef __COMPOSITE__
#define __COMPOSITE__

#include "Mini-NeoCSE/ISCStream.h"

#include <flat_map>

// 这里由于原始数据的键不唯一，需要挂一个代理主键建立对应关系，要不然后续无法进行
using OIDType = uint64_t;
using BasicTableType = std::flat_map<OIDType, std::pair<SEKey, SEValues>>;
using IdentTableType = std::flat_map<OIDType, std::vector<std::string>>;
using SystemType = std::flat_map<OIDType, std::vector<OIDType>>;

extern BasicTableType BASIC;
extern IdentTableType IDENT;
extern OIDType BarycenterID;
extern SystemType SystemTable;

void Composite();

#endif