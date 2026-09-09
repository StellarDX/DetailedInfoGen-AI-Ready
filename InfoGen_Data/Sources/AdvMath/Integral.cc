#include "Integral.h"
#include "../MathFuncs/SolvePoly.h"

#include <limits>
#include <stdexcept>

double DefiniteIntegratingFunction::operator()(Function1D f, double a, double b)const
{
    if (a == b) {return 0;}

    // 此处由于积分上下限可以是无穷，所以需要一些准备工作。
    Function1D F;
    double A, B;
    double Scale = 1;

    if (b < a)
    {
        std::swap(b, a);
        Scale = -1;
    }

    // 无穷积分转化为-1到1的积分
    if (std::isinf(a) && std::isinf(b))
    {
        if (sgn(a) > 0 || sgn(b) < 0){throw std::logic_error("Invalid limits");}
        A = -1;
        B = 1;
        F = [&f](double t)
        {
            return f(t / (1. - t * t)) * ((1. + t * t) / pow((1. - t * t), 2));
        };
    }

    // 单边无穷积分转化为0到1的积分
    else if (!std::isinf(a) && std::isinf(b))
    {
        if (sgn(b) < 0) {throw std::logic_error("Invalid limits");}
        A = 0;
        B = 1;
        F = [a, &f](double t)
        {
            return f(a + t / (1. - t)) / pow(1. - t, 2);
        };
    }
    else if (std::isinf(a) && !std::isinf(b))
    {
        if (sgn(a) > 0) {throw std::logic_error("Invalid limits");}
        A = 0;
        B = 1;
        F = [b, &f](double t)
        {
            return f(b - (1. - t) / t) / (t * t);
        };
    }

    else
    {
        F = f;
        A = a;
        B = b;
    }

    return Scale * Run(F, A, B);
}

//////////////////////////////// 高斯-克朗罗德积分 ///////////////////////////////

#include "Integrations_GaussKronrod.tbl"

void GaussKronrodQuadrature::GetNodesAndWeights(uint64_t N, std::vector<double>* GaussCoeffs, std::vector<double>* KronrodCoeffs)
{
    // 特殊值直接返回
    if (GetNodesAndWeightsSpecialCases(N, GaussCoeffs, KronrodCoeffs)) {return;}
    throw std::runtime_error("对不起，暂未实现");
}

bool GaussKronrodQuadrature::GetNodesAndWeightsSpecialCases(uint64_t N, std::vector<double>* GaussCoeffs, std::vector<double>* KronrodCoeffs)
{
    bool OK = 0;
    uint64_t GaussOrder = KronrodCoeffs ? ((N - 1) / 2) : N;

    switch (GaussOrder)
    {
    case 7:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian07_Table), std::end(__Gaussian07_Table));
        OK = 1;
        break;
    case 10:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian10_Table), std::end(__Gaussian10_Table));
        OK = 1;
        break;
    case 15:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian15_Table), std::end(__Gaussian15_Table));
        OK = 1;
        break;
    case 20:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian20_Table), std::end(__Gaussian20_Table));
        OK = 1;
        break;
    case 25:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian25_Table), std::end(__Gaussian25_Table));
        OK = 1;
        break;
    case 30:
        *GaussCoeffs = std::vector<double>(std::begin(__Gaussian30_Table), std::end(__Gaussian30_Table));
        OK = 1;
        break;
    default:
        break;
    }

    if (!KronrodCoeffs) {return OK;}

    uint64_t KronrodOrder = N;

    switch (KronrodOrder)
    {
    case 15:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod15_Table), std::end(__Kronrod15_Table));
        OK = 1;
        break;
    case 21:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod21_Table), std::end(__Kronrod21_Table));
        OK = 1;
        break;
    case 31:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod31_Table), std::end(__Kronrod31_Table));
        OK = 1;
        break;
    case 41:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod41_Table), std::end(__Kronrod41_Table));
        OK = 1;
        break;
    case 51:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod51_Table), std::end(__Kronrod51_Table));
        OK = 1;
        break;
    case 61:
        *KronrodCoeffs = std::vector<double>(std::begin(__Kronrod61_Table), std::end(__Kronrod61_Table));
        OK = 1;
        break;
    default:
        break;
    }

    return OK;
}

double GaussKronrodQuadrature::GIntegrate(Function1D f, double *pL1) const
{
    uint64_t N = (Order - 1ULL) / 2ULL;
    uint64_t GaussStart = 1;
    double Result = 0;

    auto NodeIndex = [](uint64_t Node) {return 2 * Node;};
    auto WeightIndex = [](uint64_t Weight) {return 2 * Weight + 1;};

    if (N & 1)
    {
        Result = f(0) * GaussCoefficients[WeightIndex(0)];
    }
    else
    {
        Result = 0;
        GaussStart = 0;
    }

    double L1 = abs(Result);
    for (uint64_t i = GaussStart; i < GaussCoefficients.size() / 2ULL; ++i)
    {
        double fp = f(GaussCoefficients[NodeIndex(i)]);
        double fm = f(-GaussCoefficients[NodeIndex(i)]);
        Result += (fp + fm) * GaussCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * GaussCoefficients[WeightIndex(i)];
    }

    if (pL1) {*pL1 = L1;}
    return Result;
}

double GaussKronrodQuadrature::GKAdaptiveIntegrate(Function1D f, double a, double b, uint64_t Level, double Tol, double* LastErr, double* L1)const
{
    double Error;
    double Mean = (b + a) / 2.;
    double Scale = (b - a) / 2.;

    auto F = [&](double x)
    {
        return f(Scale * x + Mean);
    };

    double r1 = GKNonAdaptiveIntegrate(F, &Error, L1);
    double Estimate = Scale * r1;

    double Temp = Estimate * pow(10, -Tolerence);
    double Tol1 = abs(Temp);
    if (Tol == 0) {Tol = Tol1;}

    if (Level && (Tol < Error) && (Tol1 < Error))
    {
        double Mid = (a + b) / 2;
        double L1N;
        Estimate = GKAdaptiveIntegrate(f, a, Mid, Level - 1, Tol / 2, LastErr, L1);
        Estimate += GKAdaptiveIntegrate(f, Mid, b, Level - 1, Tol / 2, &Error, &L1N);
        if (LastErr) {*LastErr += Error;}
        if (L1) {*L1 += L1N;}
        return Estimate;
    }

    if(L1) {*L1 *= Scale;}
    if (LastErr) {*LastErr = Error;}

    return Estimate;
}

double GaussKronrodQuadrature::GKNonAdaptiveIntegrate(Function1D f, double *Error, double *pL1)const
{
    uint64_t N = Order;
    uint64_t GaussStart = 2;
    uint64_t KronrodStart = 1;
    uint64_t GaussOrder = (N - 1ULL) / 2ULL;

    double GaussResult = 0;
    double KronrodResult = 0;
    double fp, fm;

    auto NodeIndex = [](uint64_t Node) {return 2 * Node;};
    auto WeightIndex = [](uint64_t Weight) {return 2 * Weight + 1;};

    if (GaussOrder & 1)
    {
        fp = f(0);
        KronrodResult = fp * KronrodCoefficients[WeightIndex(0)];
        GaussResult = fp * GaussCoefficients[WeightIndex(0)];
    }
    else
    {
        fp = f(0);
        KronrodResult = fp * KronrodCoefficients[WeightIndex(0)];
        GaussStart = 1;
        KronrodStart = 2;
    }

    double L1 = abs(KronrodResult);

    for (uint64_t i = GaussStart; i < KronrodCoefficients.size() / 2ULL; i += 2)
    {
        fp = f(KronrodCoefficients[NodeIndex(i)]);
        fm = f(-KronrodCoefficients[NodeIndex(i)]);
        KronrodResult += (fp + fm) * KronrodCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * KronrodCoefficients[WeightIndex(i)];
        GaussResult += (fp + fm) * GaussCoefficients[WeightIndex(i / 2)];
    }

    for (uint64_t i = KronrodStart; i < KronrodCoefficients.size() / 2ULL; i += 2)
    {
        fp = f(KronrodCoefficients[NodeIndex(i)]);
        fm = f(-KronrodCoefficients[NodeIndex(i)]);
        KronrodResult += (fp + fm) * KronrodCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * KronrodCoefficients[WeightIndex(i)];
    }

    if (pL1) {*pL1 = L1;}
    if (Error)
    {
        *Error = std::max(abs(KronrodResult - GaussResult), abs(KronrodResult * std::numeric_limits<double>::epsilon() * 2.));
    }

    return KronrodResult;
}

double GaussKronrodQuadrature::Run(Function1D f, double a, double b)const
{
    return GaussOnly ? GaussIntegrate(f, a, b) : GaussKronrodIntegrate(f, a, b);
}

double GaussKronrodQuadrature::GaussIntegrate(Function1D f, double a, double b, double* L1Norm)const
{
    double Avg = (a + b) / 2.;
    double Scale = (b - a) / 2.;
    auto F = [&](double x)
    {
        return f(Avg + Scale * x);
    };

    double Estimate = Scale * GIntegrate(F, L1Norm);

    if (L1Norm) {*L1Norm *= Scale;}
    return Estimate;
}

double GaussKronrodQuadrature::GaussKronrodIntegrate(Function1D f, double a, double b, double* LastError, double* L1Norm)const
{
    return GKAdaptiveIntegrate(f, a, b, MaxLevels, 0, LastError, L1Norm);
}