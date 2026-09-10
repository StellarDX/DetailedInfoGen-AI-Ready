#pragma once

#ifndef __CONSTANTS__
#define __CONSTANTS__

static const double GravConstant    = 6.67430E-11; // 2022 CODATA
static const double BoltzmannConst  = 1.380649E-23; // 2022 CODATA
static const double PlanckConst     = 6.62607015E-34; // 2022 CODATA
static const double ReducedPlanck   = PlanckConst / (2. * M_PI);
static const double SpeedOfLight    = 299792458; // 2022 CODATA
static const double StefanBoltzmann = (M_PI * M_PI * BoltzmannConst * BoltzmannConst * BoltzmannConst * BoltzmannConst) / (60. * ReducedPlanck * ReducedPlanck * ReducedPlanck * SpeedOfLight * SpeedOfLight);

static const double SynodicDay      = 86400; // 1会合日（即一次日出到下一次日出）的平均时长（即24时）
static const double TropicalYear    = 31556925.18747072; // 2000年1月1日测得的平均值，为365.2421896698年（365天5时48分45.19秒）

static const double Km              = 1000;
static const double AU              = 149597870700;
static const double EarthRadius     = 6378137;
static const double JupiterRadius   = 71488000; // Galanti E , Smirnova M , Ziv M ,et al.The size and shape of Jupiter[J].Nature Astronomy, 2026, 10(4):493-501.DOI:10.1038/s41550-026-02777-x.
static const double SolarRadius     = 695660000; // 日震学估计值, SOHO使用水星凌日法测得的值为696342000

static const double EarthMass       = 5.9721684E+24; // DE440
static const double JupiterMass     = 1.898125E+27; // JUP310
static const double SolarMass       = 1.988475E+30; // IAU2025

static const double SolarLuminosity = 3.828E+26; // Mamajek E E , Prsa A , Torres G ,et al.IAU 2015 Resolution B3 on Recommended Nominal Conversion Constants for Selected Solar and Planetary Properties[J].Physics, 2015.DOI:10.48550/arXiv.1510.07674.

#endif