/****************************************************************************************\
*                                          积分                                          *
\****************************************************************************************/

#pragma once

#ifndef __INTEGRAL__
#define __INTEGRAL__

#include <functional>
#include <cstdint>

using Function1D = std::function<double(double)>;

/**
 * @brief 一元函数的定积分。
 */
typedef class DefiniteIntegratingFunction
{
protected:
    virtual double Run(Function1D f, double a, double b)const = 0;
public:
    double operator()(Function1D f, double a, double b)const;
}IntegralFunction;

#if defined __GNUG__ && !defined(__clang__)
#define __Tbl_FpType __float128 // GCC已经支持四倍精度(15+112)
#else
#define __Tbl_FpType long double // 设置为当前编译器最大可支持精度(一般为15+64长精度)
#endif

extern const __Tbl_FpType __Gaussian07_Table[8];
extern const __Tbl_FpType __Kronrod15_Table[16];
extern const __Tbl_FpType __Gaussian10_Table[10];
extern const __Tbl_FpType __Kronrod21_Table[22];
extern const __Tbl_FpType __Gaussian15_Table[16];
extern const __Tbl_FpType __Kronrod31_Table[32];
extern const __Tbl_FpType __Gaussian20_Table[20];
extern const __Tbl_FpType __Kronrod41_Table[42];
extern const __Tbl_FpType __Gaussian25_Table[26];
extern const __Tbl_FpType __Kronrod51_Table[52];
extern const __Tbl_FpType __Gaussian30_Table[30];
extern const __Tbl_FpType __Kronrod61_Table[62];

#undef __Tbl_FpType
#undef __Tbl_Fp

/**
 * @brief 高斯-克朗罗德积分 (实为高斯积分和高斯-克朗罗德积分两种方法的合并)
 *
 * 「每日一积，成为积佬」
 *
 * @example
 *  计算exp(-t^2)从0到+inf的积分：
 *      auto f1 = [](double t) { return exp(-t*t); };
 *      GaussKronrodQuadrature IntegralFunc;
 *      cout << IntegralFunc(f1, 0, std::numeric_limits<double>::infinity()) << '\n';
 *  输出：0.886226925452758 (sqrt(pi)/2)
 */
class GaussKronrodQuadrature : public DefiniteIntegratingFunction
{
public:
    using Mybase = DefiniteIntegratingFunction;

protected:
    std::vector<double> GaussCoefficients;
    std::vector<double> KronrodCoefficients;

    uint64_t  Order     = 21;
    double    Tolerence = 14;
    uint64_t  MaxLevels = 15;

    static bool GetNodesAndWeightsSpecialCases(uint64_t N, std::vector<double>* GaussCoeffs, std::vector<double>* KronrodCoeffs = nullptr);

    double GIntegrate(Function1D f, double* pL1)const;
    double GKAdaptiveIntegrate(Function1D f, double a, double b, uint64_t Level, double Tol, double* LastErr, double* L1)const;
    double GKNonAdaptiveIntegrate(Function1D f, double* Error, double* pL1)const;
    double Run(Function1D f, double a, double b)const override;

public:
    bool      GaussOnly = 0; // 仅使用高斯积分 (不推荐)

    GaussKronrodQuadrature() : GaussKronrodQuadrature(21) {}
    GaussKronrodQuadrature(uint64_t N) : Order(N) {GetNodesAndWeights(N, &GaussCoefficients, &KronrodCoefficients);}

    static void GetNodesAndWeights(uint64_t N, std::vector<double>* GaussCoeffs, std::vector<double>* KronrodCoeffs = nullptr);

    double GaussIntegrate(Function1D f, double a, double b, double* L1Norm = nullptr)const;
    double GaussKronrodIntegrate(Function1D f, double a, double b, double* LastError = nullptr, double* L1Norm = nullptr)const;
};

#endif