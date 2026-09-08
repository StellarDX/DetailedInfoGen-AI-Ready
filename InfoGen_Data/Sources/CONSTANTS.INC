#pragma once

#ifndef __CONSTANTS__
#define __CONSTANTS__

static const double SynodicDay    = 86400; // 1会合日（即一次日出到下一次日出）的平均时长（即24时）
static const double TropicalYear  = 31556925.18747072; // 2000年1月1日测得的平均值，为365.2421896698年（365天5时48分45.19秒）

static const double Km            = 1000;
static const double AU            = 149597870700;
static const double EarthRadius   = 6378137;
static const double JupiterRadius = 71488000; // Galanti E , Smirnova M , Ziv M ,et al.The size and shape of Jupiter[J].Nature Astronomy, 2026, 10(4):493-501.DOI:10.1038/s41550-026-02777-x.
static const double SolarRadius   = 695660000; // 日震学估计值, SOHO使用水星凌日法测得的值为696342000

#endif