// ---------------------------------------- 开普勒方程 ---------------------------------------- //

/*
    丹霞：开普勒方程是航天动力学基础方程，也是开普勒定律的数学描述，其定义为M = E - e * sin(E)。它看似简单但
    实则是一个超越方程，这意味着无法使用初等或者解析的方式精确求解这个方程的逆，所以实际计算的时候，数值算法还是
    逃不了的_(:з」∠)_。当然这个问题也是一个困扰了学界200年之久的问题，直到像牛顿迭代这样的数值算法的出现，这
    个问题才得以解决。牛顿迭代法的实现非常简单，仅需要函数的导函数和一个初值就可以快速收敛。不过问题同样出现在这
    个初值的选择上，因为它是影响牛顿迭代速度的一个关键因子。学界对于这个初始值的算法可以说五花八门，例如2006年
    美国海军天文台在文献[1]中就给出了一种三阶初值估计算法，但是由此算法得到的初值接牛顿迭代仍然可能出现收敛慢的
    情况，故可以认为它是一种“不太稳定”的算法。2021年Richard J. Mathar在文献[2]中也提出了一种更好的初值确定
    方法，而且实验的结果也算是比较好看的了。本研究最初尝试先使用幂级数展开到前几项以确定一个初始值然后再接牛顿迭
    代，但效果依然不理想，直到看到了文献[3]。文献[3]中提到了三种算法，分别是增强型牛顿迭代，增强型Markley算法
    和分段五次多项式拟合，并且在近抛物线轨道的近日点附近会自动切换到二分法以保证求解精度，并且除了增强型牛顿迭代
    以外，另外两种算法都没有循环结构，所以速度也非常快。经后续实验，此方法在64位浮点下精度可达1-2个ULP，可以认
    为是开普勒方程反函数的“正解”。此处的开普勒方程求解算法使用文献[3]中的实现。

    对于双曲线轨道的开普勒方程，其定义为M = e * sinh(E) - E，Virginia Raposo-Pulido和Jesus Pelaez给出
    了一种四倍精度的HKE–SDG算法[4]，此算法使用多项式拟合接牛顿迭代实现，经实验仅需3次迭代就能在4倍精度下到达大
    约1-2个ulp的精度。另外2024年3月初，广东工业大学的吴柏生老师等人在SCI发表了一种新的快速求解算法，这里称它为
    “吴柏生算法”[5]。吴柏生算法的原理大致是分段帕德逼近接一次施罗德迭代，仅需要评估不超过三个超越函数，所以它的
    速度可能是很快的。不过由于吴柏生算法并未开源，故目前无法验证其准确性。所以本文仍然使用HKE–SDG算法计算双曲开
    普勒方程。

    抛物线这种情况反而是最简单的，定义为M = (1 / 2) * E + (1 / 6) * E^3，直接就是一个多项式[6]。此方程可使
    用三次方程求根公式（如范盛金算法）求解，结果必然为1实2虚，其中唯一实根就是要得到的解。

    参考文献：
    [1] Murison M A .A Practical Method for Solving the Kepler Equation[J].  2006.
        DOI:10.13140/2.1.5019.6808.
    [2] Mathar R J .Improved First Estimates to the Solution of Kepler's Equation[J].  2021.
        DOI:10.48550/arXiv.2108.03215.
    [3] Tommasini D , Olivieri D N .Two fast and accurate routines for solving the elliptic
        Kepler equation for all values of the eccentricity and mean anomaly[J].天文学与天体物理,
        2022, 658:A196.DOI:10.1051/0004-6361/202141423.
    [4] Raposo-Pulido V ,J. Peláez.An efficient code to solve the Kepler equation. Hyperbolic
        case[J].Astronomy and Astrophysics, 2018, 619.DOI:10.1051/0004-6361/201833563.
    [5] Wu B , Zhou Y , Lim C W ,et al.A new method for solving the hyperbolic Kepler
        equation[J].Applied Mathematical Modelling, 2024, 127(000):7.
        DOI:10.1016/j.apm.2023.12.017.
    [6] 平近点角 - 卫星百科(灰机Wiki)
        https://sat.huijiwiki.com/wiki/%E5%B9%B3%E8%BF%91%E7%82%B9%E8%A7%92
*/

#pragma once

#ifndef __KEPLERIAN__
#define __KEPLERIAN__

#include "Eigen/Core"

#define _KE_BEGIN namespace KE {
#define _KE_END }
#define _KE KE::

_KE_BEGIN

/**
 * @brief 椭圆开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @param Eccentricity 离心率
 * @return 平近点角
 */
double __Elliptical_Keplerian_Equation(double Eccentricity, double EccentricAnomaly);

/**
 * @brief 抛物线开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
double __Parabolic_Keplerian_Equation(double EccentricAnomaly);

/**
 * @brief 双曲开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @param Eccentricity 离心率
 * @return 平近点角
 */
double __Hyperbolic_Keplerian_Equation(double Eccentricity, double EccentricAnomaly);

/**
 * @brief 椭圆开普勒方程的反函数
 */
class __Elliptical_Inverse_Keplerian_Equation
{
protected:
    double Eccentricity;
public:
    __Elliptical_Inverse_Keplerian_Equation(double e);
    virtual double operator()(double MeanAnomaly)const = 0;
};

/**
 * @brief 抛物线开普勒方程的反函数
 */
class __Parabolic_Inverse_Keplerian_Equation
{
public:
    __Parabolic_Inverse_Keplerian_Equation() {}
    virtual double operator()(double MeanAnomaly)const = 0;
};

/**
 * @brief 双曲开普勒方程的反函数
 */
class __Hyperbolic_Inverse_Keplerian_Equation
{
protected:
    double Eccentricity;
public:
    __Hyperbolic_Inverse_Keplerian_Equation(double e);
    virtual double operator()(double MeanAnomaly)const = 0;
};

/**
 * @brief 椭圆开普勒方程求解工具
 */
class __Enhanced_Inverse_Keplerian_Equation_Solver
    : public __Elliptical_Inverse_Keplerian_Equation
{
public:
    using Mybase = __Elliptical_Inverse_Keplerian_Equation;

protected:
    double AbsoluteTolerence = 14.522878745280337562704972096745; // 3E-15
    double RelativeTolerence = 15.657577319177793764036061134032; // 2.2E-16

    double EBoundary         = 0.99;
    double MBoundary         = 0.0045;

    virtual double Run(double MRad, double AbsTol, double RelTol)const = 0;
    virtual double BoundaryHandler(double MRad, double AbsTol, double RelTol)const;

public:
    __Enhanced_Inverse_Keplerian_Equation_Solver(double e) : Mybase(e) {}
    double operator()(double MeanAnomaly)const final;
};

// ENRKE
class __Newton_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;
protected:
    double Run(double MRad, double AbsTol, double RelTol)const override;
public:
    __Newton_Inverse_Keplerian_Equation(double e) : Mybase(e) {}
};

// ENMAKE
class __Markley_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;
protected:
    double Run(double MRad, double AbsTol, double RelTol)const override;
public:
    __Markley_Inverse_Keplerian_Equation(double e) : Mybase(e) {}
};

// ENP5KE
class __Piecewise_Quintic_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;

protected:
    std::vector<int64_t> BlockBoundaries;
    std::vector<double> Breakpoints;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Coefficients;

    static void GetCoefficients1(double Eccentricity, double Tolerence,
        std::vector<double>* Grid/*, uint64* n*/);
    static void GetCoefficients2(double Eccentricity,
        const std::vector<double>& Grid, //uint64 n,
        std::vector<int64_t>* kvec, std::vector<double>* bp,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>* coeffs);

    uint64_t FindInterval(double MRad)const;
    double BoundaryHandler(double MRad, double AbsTol, double RelTol)const override;
    double Run(double MRad, double AbsTol, double RelTol)const override;

public:
    __Piecewise_Quintic_Inverse_Keplerian_Equation(double e);

    static void GetCoefficients(double Eccentricity, double Tolerence,
        /*uint64* n,*/ std::vector<int64_t>* kvec, std::vector<double>* bp,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>* coeffs);
};

/**
 * @brief 抛物线开普勒方程求解工具
 */
class __Polynomial_Parabolic_Inverse_Keplerian_Equation
    : public __Parabolic_Inverse_Keplerian_Equation
{
public:
    double operator()(double MeanAnomaly)const;
};

/**
 * @brief 双曲开普勒方程求解工具
 */
class __SDGH_Equacion_Inversa_de_Keplerh
    : public __Hyperbolic_Inverse_Keplerian_Equation
{
public:
    using Mybase     = __Hyperbolic_Inverse_Keplerian_Equation;
    using STableType = double;
    using PTableType = std::function<double(double, double)>;

    constexpr static const uint64_t SegmentTableSize  = 51;
    constexpr static const uint64_t SegmentTableBound = 26;
    constexpr static const uint64_t PolynomTableSize  = 50;
    constexpr static const uint64_t PolynomTableBound = 26;

    static const STableType SegmentCoeffsTable[SegmentTableSize];
    static const PTableType TablaPolinomios[PolynomTableSize];

protected:
    double AbsoluteTolerence = 15.65;
    double RelativeTolerence = 15.65;
    double MaxIterations     = 1.69897;

    double SegmentTable[SegmentTableSize];

    double SingularCornerInitEstimator(double MRad)const;
    double SingularCornerInitEstimatorDOS(double MRad)const;
    double SingularCornerInitEstimatorTRES(double MRad)const;

    double NewtonInitValue(double MeanAnomaly)const;
    double Run(double MeanAnomaly, uint64_t* NumberOfIters, double* Residual)const;

public:
    __SDGH_Equacion_Inversa_de_Keplerh(double e);

    double operator()(double MeanAnomaly)const override;
    double operator()(double MeanAnomaly, uint64_t* NumberOfIters, double* Residual)const;

    static void GetSegments(double Eccentricity, double* SegTable);
    static Eigen::Matrix<double, 4, 1> VectorizedHKE(double Eccentricity, double MRad, double Init);
};

using DefaultEllipticalIKE = KE::__Newton_Inverse_Keplerian_Equation;
using DefaultParabolicIKE  = KE::__Polynomial_Parabolic_Inverse_Keplerian_Equation;
using DefaultHyperbolicIKE = KE::__SDGH_Equacion_Inversa_de_Keplerh;

_KE_END

double KeplerianEquation(double Eccentricity, double EccentricAnomaly);
double InverseKeplerianEquation(double Eccentricity, double MeanAnomaly);

double GetTrueAnomalyFromEccentricAnomaly(double Eccentricity, double EccentricAnomaly);
double GetEccentricAnomalyFromTrueAnomaly(double Eccentricity, double TrueAnomaly);

#endif