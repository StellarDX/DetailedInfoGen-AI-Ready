#define _USE_MATH_DEFINES
#include "Keplerian.h"
#include "../MathFuncs/Trigonometry.h"
#include "../MathFuncs/SolvePoly.h"

#include "Eigen/Core"
#include <complex>

_KE_BEGIN

static const double __DEG2RAD = 0.01745329251994329576923690768489;
static const double __RAD2DEG = 57.295779513082320876798154814105;

///////////////////////////////////// BASE ////////////////////////////////////

inline __Elliptical_Inverse_Keplerian_Equation::__Elliptical_Inverse_Keplerian_Equation(double e)
{
    if (e >= 1)
    {
        throw std::logic_error("This function is only adapt for elliptical orbits.");
    }
    Eccentricity = e;
}

__Hyperbolic_Inverse_Keplerian_Equation::__Hyperbolic_Inverse_Keplerian_Equation(double e)
{
    if (e <= 1)
    {
        throw std::logic_error("This function is only adapt for hyperbolic orbits.");
    }
    Eccentricity = e;
}

///////////////////////////////////// ENKE ////////////////////////////////////

double __Enhanced_Inverse_Keplerian_Equation_Solver::BoundaryHandler(double MRad, double AbsTol, double RelTol)const
{
    // Using Bisection
    double fa = 2.7 * MRad;
    double fb = 0.301;
    double f = 0.154;
    double al = AbsTol / 1E7;
    double be = AbsTol / 0.3;

    while (fb - fa > al + f * be)
    {
        if (f - Eccentricity * sin(f) - MRad > 0)
        {
            fb = f;
        }
        else {fa = f;}
        f = (fa + fb) / 2.;
    }

    return f;
}

double __Enhanced_Inverse_Keplerian_Equation_Solver::operator()(double MeanAnomaly)const
{
    if (!Eccentricity) {return MeanAnomaly;}
    double RealAbsTol = pow(10, -AbsoluteTolerence);
    double RealRelTol = pow(10, -RelativeTolerence);

    double M = MeanAnomaly, Mr = M;
    if (M > 360)
    {
        Mr = fmod(M, 360);
    }
    double Flip = 0;
    if (Mr > 180)
    {
        Mr = 360 - Mr;
        Flip = 1;
    }
    else {Flip = -1;}

    double MRad = Mr * __DEG2RAD;
    double Tmp;

    if (Eccentricity > EBoundary && MRad < MBoundary)
    {
        Tmp = BoundaryHandler(MRad, RealAbsTol, RealRelTol);
    }
    else {Tmp = Run(MRad, RealAbsTol, RealRelTol);}

    return (M * __DEG2RAD + Flip * (MRad - Tmp)) * __RAD2DEG;
}

//////////////////////////////////// ENRKE ////////////////////////////////////

double __Newton_Inverse_Keplerian_Equation::Run(double MRad, double AbsTol, double RelTol)const
{
    double Delns = (2. * AbsTol) / (Eccentricity + RelTol);
    double Small = 0.999999; // std::nextafter(1., 0.);
    double ERes = MRad +
        Small * MRad * (M_PI - MRad) /
        (2.* MRad + Eccentricity - M_PI + (M_PI * M_PI / 4.) /
        (Eccentricity + RelTol));
    double f2 = Eccentricity * sin(ERes);
    double f3 = Eccentricity * cos(ERes);
    double f1 = 1 - f3;
    double f0 = ERes - f2 - MRad;
    double del = (-f0 / f1) * ((f1 * f1 * f1) - ((f0 * f1 * f2) / 2.) +
        (f0 * f0 * f3) / 3.) / ((f1 * f1 * f1) - (f0 * f1 * f2) + (f0 * f0 * f3) / 2.);
    while (del * del > f1 * Delns)
    {
        ERes += del;
        del = (MRad - ERes + Eccentricity * sin(ERes)) /
            (1. - Eccentricity * cos(ERes));
    }
    return ERes + del;
}

//////////////////////////////////// ENMAKE ///////////////////////////////////

double __Markley_Inverse_Keplerian_Equation::Run(double MRad, double AbsTol, double RelTol)const
{
    double alf = ((3. * M_PI * M_PI) +
        (1.6 * M_PI) * (M_PI) /
        (1. + Eccentricity)) / (M_PI * M_PI - 6);
    double d = 3. * (1. - Eccentricity) + alf * Eccentricity;
    double M2 = MRad * MRad;
    double q = 2. * alf * d * (1. - Eccentricity) - M2;
    double q2 = q * q;
    double r = 3. * alf * d * (d - 1. + Eccentricity) * MRad + M2 * MRad;
    double w0 = abs(r) + sqrt(q2 * q + r * r);
    double w = cbrt(w0 * w0);
    double ERes = (MRad + 2. * r * w / (w * w + w * q + q2)) / d;
    double f2 = Eccentricity * sin(ERes);
    double f3 = Eccentricity * cos(ERes);
    double f1 = 1 - f3;
    double f0 = ERes - f2 - MRad;
    double del0 = -f0 / (f1 - (f0 * f2 / 2.) / f1);
    double del1 = -f0 / (f1 + (del0 * f2 / 2.) + del0 * del0 * f3 / 6.);
    double del2 = del1 * del1;
    ERes += -f0 / (f1 + (del1 * f2 / 2.) + del2 * f3 / 6. - del1 * del2 * f2 / 24.);
    return ERes;
}

//////////////////////////////////// ENP5KE ///////////////////////////////////

__Piecewise_Quintic_Inverse_Keplerian_Equation::__Piecewise_Quintic_Inverse_Keplerian_Equation(double e)
    : __Piecewise_Quintic_Inverse_Keplerian_Equation::Mybase(e)
{
    GetCoefficients(Eccentricity, pow(10, -AbsoluteTolerence),
        &BlockBoundaries, &Breakpoints, &Coefficients);
}

void __Piecewise_Quintic_Inverse_Keplerian_Equation::GetCoefficients(
    double Eccentricity, double Tolerence,
    std::vector<int64_t>* kvec, std::vector<double>* bp,
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>* coeffs)
{
    std::vector<double> EGrid;
    GetCoefficients1(Eccentricity, Tolerence, &EGrid);
    kvec->resize(EGrid.size() + 1);
    std::fill(kvec->begin(), kvec->end(), 0);
    bp->resize(EGrid.size() + 1);
    coeffs->resize(EGrid.size(), 6);
    coeffs->setZero();
    GetCoefficients2(Eccentricity, EGrid, kvec, bp, coeffs);
}

void __Piecewise_Quintic_Inverse_Keplerian_Equation::GetCoefficients1(
    double Eccentricity, double Tolerence, std::vector<double>* Grid)
{
    double E2 = 1. - Eccentricity;
    double h0 = pow(Tolerence, 1. / 6) * (0.86 + 1.1* E2 + 1.5 * E2 * E2);
    // 以下两项是针对C数组使用的预测大小，但此处使用了动态数组，所以可以不需要了。
    //*n = ::llrint((CSE_PI - ln(E2) / CSE_SQRT2) / h0) + 2;
    //*Grid = std::vector<double>(*n);

    // Multistep routine begin
    double Ei = 0;
    while (Ei < 180)
    {
        Grid->push_back(Ei);
        Ei = __RAD2DEG * (Ei * __DEG2RAD +
            h0 * sqrt(1. - Eccentricity * cosd(Ei)));
    }
    Grid->back() = 180;
}

void __Piecewise_Quintic_Inverse_Keplerian_Equation::GetCoefficients2(
    double Eccentricity, const std::vector<double>& Grid,
    std::vector<int64_t>* kvec, std::vector<double>* bp,
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>* coeffs)
{
    uint64_t n = Grid.size();
    double e1 = Eccentricity;
    double e2 = e1 * e1;
    double e3 = e2 * e1;
    double e4 = e2 * e2;

    for (uint64_t jx = 0; jx < n; ++jx)
    {
        double Ej = Grid[jx];
        double sj = sind(Ej);
        double cj1 = cosd(Ej);
        double cj2 = cj1 * cj1;
        double dj1 = 1. / (1. - e1 * cj1);
        double dj2 = dj1 * dj1;
        double dej1 = sj* dj1;
        double dej2 = dej1 * dej1;
        double dej3 = dej2 * dej1;
        double dej4 = dej2 * dej2;

        bp->at(jx) = __RAD2DEG * (Ej * __DEG2RAD - e1 * sj);
        (*coeffs)(jx, 0) = Ej * __DEG2RAD;
        (*coeffs)(jx, 1) = dj1;
        (*coeffs)(jx, 2) = -e1 * dej1 / 2.;
        (*coeffs)(jx, 3) = -e1 * cj1 * dj1 / 6. + e2 * dej2 / 2.;
        (*coeffs)(jx, 4) = (e1 * dej1 + 10. * e2 * cj1 * dej1 * dj1 -
            15. * e3 * dej3) / 24.;
        (*coeffs)(jx, 5) = (e1 * cj1 * dj1 + 10. * e2 * cj2 * dj2 -
            15. * e2 * dej2 - 105. * e3 * cj1 * dj1 * dej2 +
            105. * e4 * dej4) / 120.;

        int64_t jy = int64_t(bp->at(jx) * n / 180.) + 1U;
        if (jy > 0 && jy < n) {++(kvec->at(jy));}
    }

    bp->back() = 180;
    int64_t kvj = 0;
    kvec->front() = 0;
    kvec->back() = n + 1;
    for (uint64_t jx = 1; jx < n; ++jx)
    {
        kvj += kvec->at(jx);
        kvec->at(jx) = kvj;
    }
}

uint64_t __Piecewise_Quintic_Inverse_Keplerian_Equation::FindInterval(double MRad)const
{
    uint64_t ny = Breakpoints.size() - 1;
    uint64_t Left = 0;
    uint64_t Right = ny;
    uint64_t i1 = uint64_t(MRad * __RAD2DEG * ny / 180.);
    uint64_t q = BlockBoundaries[i1] - 1;
    if (q > Left) {Left = q;}
    q = BlockBoundaries[i1 + 1] + 1;
    if (q < Right) {Right = q;}
    if (Breakpoints[Left + 1] * __DEG2RAD > MRad)
    {
        return Left;
    }
    ++Left;
    while (Left < Right - 1)
    {
        uint64_t Mid = (Left + Right) / 2ULL;
        if (Breakpoints[Mid] * __DEG2RAD > MRad)
        {
            Right = Mid;
        }
        else
        {
            Left = Mid;
        }
    }
    return Left;
}

double __Piecewise_Quintic_Inverse_Keplerian_Equation::BoundaryHandler
    (double MRad, double AbsTol, double RelTol)const
{
    uint64_t i = FindInterval(MRad);
    double Left = Coefficients(i, 0);
    double Right = Coefficients(i + 1, 0);
    double Mid = (Left + Right) / 2.;
    double al = AbsTol / 1E7;
    double be = AbsTol / 0.3;
    while (Right - Left > al + Mid * be)
    {
        if (Mid - Eccentricity * sin(Mid) - MRad > 0)
        {
            Right = Mid;
        }
        else {Left = Mid;}
        Mid = (Left + Right) / 2.;
    }
    return Mid;
}

double __Piecewise_Quintic_Inverse_Keplerian_Equation::Run(double MRad, double AbsTol, double RelTol)const
{
    uint64_t i = FindInterval(MRad);
    double delM = Coefficients(i, 1) * (MRad - Breakpoints[i] * __DEG2RAD);
    return Coefficients(i, 0) + delM *
        (1. + delM *
        (Coefficients(i, 2) + delM *
        (Coefficients(i, 3) + delM *
        (Coefficients(i, 4) + delM * Coefficients(i, 5)))));
}

////////////////////////////////// PARABOLIC //////////////////////////////////

double __Polynomial_Parabolic_Inverse_Keplerian_Equation::operator()(double MeanAnomaly) const
{
    double MRad = MeanAnomaly * __DEG2RAD;
    Eigen::Matrix<double, 4, 1> Coefficients;
    Coefficients[0] = 1;
    Coefficients[1] = 0;
    Coefficients[2] = 3;
    Coefficients[3] = -6 * MRad;
    Eigen::Matrix<std::complex<double>, 3, 1> Roots;
    Roots.setZero();
    SolveCubic(Coefficients, Roots);
    return Roots[0].real() * __RAD2DEG;
}

/////////////////////////////////// HKE-SDG ///////////////////////////////////
/*
    Copyright (C) 2018 by the UNIVERSIDAD POLITECNICA DE MADRID (UPM)
    AUthors: Virginia Raposo-Pulido and Jesus Pelaez

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "HKE-SDG.tbl"

__SDGH_Equacion_Inversa_de_Keplerh::__SDGH_Equacion_Inversa_de_Keplerh(double e)
    : __SDGH_Equacion_Inversa_de_Keplerh::Mybase(e)
{
    GetSegments(e, this->SegmentTable);
}

void __SDGH_Equacion_Inversa_de_Keplerh::GetSegments(double Eccentricity, double *SegTable)
{
    for (uint64_t i = 0; i < SegmentTableSize; ++i)
    {
        uint64_t Index = (i & 1) ? (i / 2U + SegmentTableBound) : (i / 2U);
        SegTable[Index] = SegmentCoeffsTable[Index] * Eccentricity - 0.1 * double(i);
    }
}

double __SDGH_Equacion_Inversa_de_Keplerh::SingularCornerInitEstimator(double MRad)const
{
    double eps = Eccentricity - 1.;
    double chi = MRad / sqrt(eps * eps * eps);
    double Lam = sqrt(8.0L + 9.0L * chi * chi);
    double T   = cbrt(Lam + 3.0L * chi);
    double T2  = T * T;
    double sig = 6.0L * chi / (2.0L + T2 + 4.0L / T2);

    static auto nuh_as = [](double sigmao, double epsilon)
    {
        double t1 = sigmao * sigmao;
        double t2 = t1 * sigmao;
        double t5 = t1 + 2.0;
        double t10 = t1 * t1;
        double t12 = t10 * t1;
        double t17 = t5 * t5;
        double t18 = t17 * t5;
        double t20 = epsilon * epsilon;
        double t25 = t10 * t10;
        double t26 = t25 * t1;
        double t34 = t17 * t17;
        double t42 = t25 * t12;
        double t44 = t25 * t10;
        double t55 = t20 * t20;
        double t60 = t25 * t25;
        double t73 = t34 * t34;

        // Converted to integer math.
        double e1 = t2 * (t1 + 20LL) / (60LL * t5) * epsilon;
        double e2 = t10 * sigmao * (t12 + 25LL * t10 + 340LL * t1 + 840LL) / (1400LL * t18) * t20;
        double e3 = t10 * t2 * (5LL * t26 + 166LL * t25 + 2505LL * t12 + 28240LL * t10 + 124100LL * t1 + 180000LL) / (126000LL * t34) / t5 * t20 * epsilon;
        double e4 = t25 * sigmao * (387LL * t42 + 16172LL * t44 + 306228LL * t26 + 3619848LL * t25 + 35945312LL * t12 + 205356480LL * t10 + 568176000LL * t1 + 603680000LL) / (155232000LL * t34) / t18 * t55;
        double e5 = t25 * t2 * (35203LL * t60 * t1 + 1768558LL * t60 + 40930516LL * t42 + 582574720LL * t44 + 5917801960LL * t26 + 53311645600LL * t25 + 348404653600LL * t12 + 1361270272000LL * t10 + 2820160000000LL * t1 + 2400448512000LL) / (209563200000LL * t73) / t5 * t55 * epsilon;

        return sigmao - e1 + e2 - e3 + e4 - e5;
    };

    return sqrt(eps) * nuh_as(sig, eps);
}

double __SDGH_Equacion_Inversa_de_Keplerh::SingularCornerInitEstimatorDOS(double MRad)const
{
    double S, u;
    for (int i = 0; i < 10; i++)
    {
        if (i == 0) {u = (MRad / Eccentricity);}
        else {u = (MRad + log(S + sqrt(S * S + 1.0L))) / Eccentricity;}
        S = u;
    }
    return S;
}

double __SDGH_Equacion_Inversa_de_Keplerh::SingularCornerInitEstimatorTRES(double MRad)const
{
    double S, u, z;
    double le, l2, lm, tt, phi, xi;

    le = log(Eccentricity);
    l2 = log(2.0L);
    lm = log(MRad);
    tt = sqrt(Eccentricity * Eccentricity + MRad * MRad);
    phi = (tt * (log(tt + MRad) - le)) / ((tt - 1.0) * MRad);
    xi = (-1.0L / (2.0L * pow(MRad, 3.0L))) * (pow(le - l2, 2.0L) + lm * (lm + 2.0L * (l2 - le)));
    z = (MRad / Eccentricity)*(1.0 + phi + xi);

    //printf("\n M= %020.18lf   e= %020.18lf", (double)M, (double)e);
    for (int i = 0; i < 10; i++)
    {
        if (i == 0) {u = z;}
        else {u = (MRad + log(S + sqrt(S * S + 1.0L))) / Eccentricity;}
        S = u;
        //printf("\n S= %020.18lf", (double)S);
    }
    //printf("\n S= %020.18lf\n\n", (double)S);
    return S;
}

double __SDGH_Equacion_Inversa_de_Keplerh::NewtonInitValue(double MeanAnomaly)const
{
    double e = Eccentricity;
    double M = MeanAnomaly * __DEG2RAD;

    if (abs(e - 1.0) <= 0.5L && abs(M) < 0.25L)
    {
        return sinh(SingularCornerInitEstimator(M));
    }

    uint64_t i = 0;
    while(M - SegmentTable[i] >= 0.0L && i < SegmentTableBound) {i++;}
    uint64_t j = i - 1;
    uint64_t k;

    if (j == SegmentTableBound - 1) {k = SegmentTableBound - 1;}
    else if (j == 0 && M < SegmentTable[SegmentTableBound]) {k = 0;}
    else if (j == SegmentTableBound - 2 &&
        M > SegmentTable[SegmentTableSize - 1])
    {
        k = SegmentTableBound - 2;
    }
    else if (M < SegmentTable[j + SegmentTableBound])
    {
        if ((M - SegmentTable[j]) <
            (SegmentTable[j + SegmentTableBound] - M)) {k = j;}
        else {k = j + SegmentTableBound - 1;}
    }
    else if (M > SegmentTable[j + SegmentTableBound])
    {
        if ((M - SegmentTable[j + SegmentTableBound]) <
            (SegmentTable[j + 1] - M))
        {
            k = j + SegmentTableBound;
        }
        else {k = j;}
    }

    return k >= PolynomTableSize ? 0.0 : TablaPolinomios[k](e, M);
}

Eigen::Matrix<double, 4, 1> __SDGH_Equacion_Inversa_de_Keplerh::VectorizedHKE(double Eccentricity, double MRad, double Init)
{
    double ad = 1.0L + Init * Init;
    double a = sqrt(ad);
    double at = ad*a;
    double ac = at*ad;

    return
    {
        Init - log(Init + a) / Eccentricity - MRad / Eccentricity,
        double(1.0L - 1.0L / a / Eccentricity),
        Init / at / Eccentricity,
        double((3.0L - 2.0L * ad) / ac / Eccentricity)
    };
}

double __SDGH_Equacion_Inversa_de_Keplerh::Run(double MeanAnomaly, uint64_t* NumberOfIters, double* Residual) const
{
    double RealAbsTol = pow(10, -AbsoluteTolerence);
    double RealRelTol = pow(10, -RelativeTolerence);

    double M = MeanAnomaly * __DEG2RAD;
    double e = Eccentricity;
    double eps = RealRelTol;

    double u = NewtonInitValue(MeanAnomaly);
    Eigen::Matrix<double, 4, 1> c = VectorizedHKE(e, M, u);

    if(abs(c[0]) < eps)
    {
        if (NumberOfIters) {*NumberOfIters = 0;}
        if (Residual) {*Residual = c[0];}
        return u * __RAD2DEG;
    }

    uint64_t MaxIter = floor(pow(10, MaxIterations));
    uint64_t i;
    for(i = 1; i < MaxIter; ++i)
    {
        double delta = 2.0L * c[0] / (c[1] + (c[1] > 0.0L ? 1 : -1) *
            sqrt(abs(c[1] * c[1] - 2.0 * c[0] * c[2])));
        delta += -c[3] * delta * delta * delta /
            (3.0L * c[3] * delta * delta - 6.0L * c[2] * delta + 6.0L * c[1]);
        u -= delta;
        c = VectorizedHKE(e, M, u);
        if (abs(u) <= 1.0L)
        {
            if ((abs(c[0]) < eps) || (abs(delta) < RealAbsTol)) {break;}
        }
        else
        {
            if ((abs(c[0]) < eps) || (abs(delta) < abs(u * RealAbsTol))) {break;}
        }
    }

    if (NumberOfIters) {*NumberOfIters = i;}
    if (Residual) {*Residual = c[0];}
    return u * __RAD2DEG;
}

double __SDGH_Equacion_Inversa_de_Keplerh::operator()(double MeanAnomaly)const
{
    return operator()(MeanAnomaly, nullptr, nullptr);
}

double __SDGH_Equacion_Inversa_de_Keplerh::operator()
    (double MeanAnomaly, uint64_t* NumberOfIters, double* Residual)const
{
    double MeanAnomaly0 = abs(MeanAnomaly);
    double ERad = Run(MeanAnomaly0, NumberOfIters, Residual) * __DEG2RAD;
    return __RAD2DEG * ((MeanAnomaly < 0 ? -1 : 1) * log(ERad + sqrt(1.0 + ERad * ERad)));
}

_KE_END

double KeplerianEquation(double Eccentricity, double EccentricAnomaly)
{
    if (Eccentricity == 0) {return EccentricAnomaly;}
    EccentricAnomaly *= KE::__DEG2RAD;
    if (Eccentricity > 0 && Eccentricity < 1)
    {
        return (EccentricAnomaly - Eccentricity * sin(EccentricAnomaly)) * KE::__RAD2DEG;
    }
    else if (Eccentricity == 1)
    {
        return (EccentricAnomaly / 2. + EccentricAnomaly * EccentricAnomaly * EccentricAnomaly / 6.) * KE::__RAD2DEG;
    }
    else
    {
        return (Eccentricity * sinh(EccentricAnomaly) - EccentricAnomaly) * KE::__RAD2DEG;
    }
}

double InverseKeplerianEquation(double Eccentricity, double MeanAnomaly)
{
    if (Eccentricity == 0) {return MeanAnomaly;}
    else if (Eccentricity > 0 && Eccentricity < 1)
    {
        return KE::__Newton_Inverse_Keplerian_Equation(Eccentricity)(MeanAnomaly);
    }
    else if (Eccentricity == 1)
    {
        return KE::__Polynomial_Parabolic_Inverse_Keplerian_Equation()(MeanAnomaly);
    }
    else
    {
        return KE::__SDGH_Equacion_Inversa_de_Keplerh(Eccentricity)(MeanAnomaly);
    }
}

double GetTrueAnomalyFromEccentricAnomaly(double Eccentricity, double EccentricAnomaly)
{
    if (Eccentricity == 0) {return EccentricAnomaly;}
    else if (Eccentricity < 1)
    {
        double EDeg = EccentricAnomaly;
        double bet = Eccentricity /
            (1. + sqrt(1. - Eccentricity * Eccentricity));
        return EDeg + 2. * arctand((bet * sind(EccentricAnomaly)) /
            (1. - bet * cosd(EccentricAnomaly)));
    }
    else if (Eccentricity == 1)
    {
        double ERad = EccentricAnomaly * KE::__DEG2RAD;
        return KE::__RAD2DEG * (2. * arctand(ERad));
    }
    else
    {
        double ERad = EccentricAnomaly * KE::__DEG2RAD;
        double TanPhi = tanh(ERad / 2.) *
            sqrt((Eccentricity + 1) / (Eccentricity - 1));
        return KE::__RAD2DEG * (2. * arctand(TanPhi));
    }
}

double GetEccentricAnomalyFromTrueAnomaly(double Eccentricity, double TrueAnomaly)
{
    if (Eccentricity == 0) {return TrueAnomaly;}
    else if (Eccentricity < 1)
    {
        double y = sqrt(1. - Eccentricity * Eccentricity) * sind(TrueAnomaly);
        double x = Eccentricity + cosd(TrueAnomaly);
        double r = arctan2d(y, x);
        return r + ((r < 0) ? 360 : 0);
    }
    else if (Eccentricity == 1)
    {
        double TDeg = TrueAnomaly / 2.;
        return KE::__RAD2DEG * (tand(TDeg));
    }
    else
    {
        double y = sqrt(Eccentricity * Eccentricity - 1) * sind(TrueAnomaly);
        double x = Eccentricity + cosd(TrueAnomaly);
        return KE::__RAD2DEG * (atanh(y / x));
    }
}