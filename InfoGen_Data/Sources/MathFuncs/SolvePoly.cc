#include <vector>
#define _USE_MATH_DEFINES
#include "SolvePoly.h"
#include "Trigonometry.h"

int SolveLinear(Eigen::Matrix<double, 2, 1> Coeffs, Eigen::Matrix<std::complex<double>, 1, 1> Roots)
{
    double a = Coeffs[0];
    double b = Coeffs[1];

    Roots[0] = -b / a;

    return 0;
}

int SolveQuadratic(Eigen::Matrix<double, 3, 1> Coeffs, Eigen::Matrix<std::complex<double>, 2, 1> Roots)
{
    double a = Coeffs[0];
    double b = Coeffs[1];
    double c = Coeffs[2];

    std::complex<double> del = b * b - 4.0 * a * c;

    Roots[0] = (-b + sqrt(del)) / (2. * a);
    Roots[1] = (-b - sqrt(del)) / (2. * a);

    return 0;
}

int __Zero_Like(double x, double Tolerence) {return std::abs(x) < Tolerence;}

int SolveCubic(Eigen::Matrix<double, 4, 1> Coeffs, Eigen::Matrix<std::complex<double>, 3, 1>& Roots, double Tolerence)
{
    /***************************************************************************
     * 三次方程求解算法
     *
     * 求解三次方程有两种算法，分别为卡尔丹公式和盛金公式。但是后来有人发现这两种算法都
     * 有问题：
     *  1) 卡尔丹公式没有认真对待复数解，且算法中含有大量复数开方。
     *  2) 范盛金的算法仍有部分缺陷需要更正
     * 对此2019年田红亮基于范盛金的成果进行了修正，下文采用此方法。
     *
     * 本算法的原作者范盛金于2018年9月6日晚在东莞离世，享年63岁
     *
     * 参考文献：
     *  [1] 范盛金.一元三次方程的新求根公式与新判别法[J].海南师范学院学报(自然科学版),
     *      1989,2(2):91-98.
     *  [2] 田红亮.一元三次方程根的解法[J].湖北工程学院学报,2019,39(06):97-105.
     *  [3] 雪鳕.卡丹公式欺骗了五百年所有数学家[EB/OL].(2013-11-08)[2019-08-18].
     *      http://blog.sina.com.cn/s/blog_6aa947af0101dy4f.html.
     *  [4] 沈天珩.一元高次方程寻根记杂谈[EB/OL].(2020-02-03)[2025-05-09].
     *      https://zhuanlan.zhihu.com/p/104832975
     *  [5] 毕式明,潘俊宇.“盛金公式”发明者、著名数学家范盛金辞世[EB/OL].南方Plus,
     *      (2018-09-08)[2025-05-09]
     *      https://www.sohu.com/a/252667814_100116740
     ***************************************************************************/

    double a = Coeffs[0];
    double b = Coeffs[1];
    double c = Coeffs[2];
    double d = Coeffs[3];
    double RTol = pow(10, -Tolerence);

    double A = b * b - 3. * a * c;
    double B = b * c - 9. * a * d;
    double C = c * c - 3. * b * d;

    double DEL = B * B - 4. * A * C;

    if (__Zero_Like(DEL, RTol)) // Δ = 0，3个实数根
    {
        if (__Zero_Like(A, RTol)) // A = 0，3个相等实数根
        {
            Roots[0] = -b / (3. * a);
            Roots[1] = Roots[0];
            Roots[2] = Roots[0];

            return 1;
        }
        else // A > 0，3个实数根中有2个相等
        {
            Roots[0] = (B / A) - (b / a);
            Roots[1] = -B / (2. * A);
            Roots[2] = Roots[1];

            return 2;
        }
    }

    if (DEL > 0) // Δ > 0，1个实数根和2个共轭复数根
    {
        double cbrtY1 = cbrt(((3. * a * B) - (2. * A * b) + (3. * a * sqrt(DEL))) / 2.);
        double cbrtY2 = cbrt(((3. * a * B) - (2. * A * b) - (3. * a * sqrt(DEL))) / 2.);

        Roots[0] = (cbrtY1 + cbrtY2 - b) / (3. * a);
        Roots[1] = (-(2. * b) - cbrtY1 - cbrtY2 + (1i * sqrt(3) * (cbrtY1 - cbrtY2))) / (6. * a);
        Roots[2] = (-(2. * b) - cbrtY1 - cbrtY2 + (1i * sqrt(3) * (cbrtY2 - cbrtY1))) / (6. * a);

        return 3;
    }

    if (DEL < 0) // Δ < 0，A > 0，3个不相等实数根
    {
        double tet = arccosd(((3. * a * B) - (2. * A * b)) / (2. * A * sqrt(A)));

        Roots[0] = ((2. * sqrt(A) * cosd(tet / 3.)) - b) / (3. * a);
        Roots[1] = ((2. * sqrt(A) * cosd((tet + 360) / 3.)) - b) / (3. * a);
        Roots[2] = ((2. * sqrt(A) * cosd((tet + 720) / 3.)) - b) / (3. * a);

        return 4;
    }

    return -1;
}

template <typename T> int sgn(T val) {return (T(0) < val) - (val < T(0));}

int SolveQuartic(Eigen::Matrix<double, 5, 1> Coeffs, Eigen::Matrix<std::complex<double>, 4, 1>& Roots, double Tolerence)
{
    /***************************************************************************
     * 四次方程求解算法
     *
     * 历史上最先提出一般形式的四次方程的解法的是卡尔丹的学生费拉里。但是其公式复杂度
     * 极高，对此，沈天珩对公式进行了简化。同样的，相比费拉里算法，沈天珩算法中不会出
     * 现复数开方。
     *
     * 沈天珩在知乎上另一篇四次方程求根公式证明文章下的评论说，他并没有对此公开发表过
     * 论文，仅仅只是在他本科辅修的毕设中提到过这个算法。对于计算机求解代码而言，不该
     * 出现根号时应尽量避免，万不得已出现根号最好避免复数开方，这些都是优势所在。毕竟
     * 像沈天珩算法这种除了求解以外还能判虚实判重根的算法太少了。
     *
     * 参考文献：
     *  [1] 沈天珩.一元高次方程寻根记杂谈[EB/OL].(2020-02-03)[2025-05-09].
     *      https://zhuanlan.zhihu.com/p/104832975
     ***************************************************************************/

    double a = Coeffs[0];
    double b = Coeffs[1];
    double c = Coeffs[2];
    double d = Coeffs[3];
    double e = Coeffs[4];
    double RTol = pow(10, -Tolerence);

    double D = 3. * b * b - 8. * a * c;
    double E = -b * b * b + 4. * a * b * c - 8. * a * a * d;
    double F = 3. * b * b * b * b + 16. * a * a * c * c - 16. * a * b * b * c + 16. * a * a * b * d - 64. * a * a * a * e;

    double A = D * D - 3. * F;
    double B = D * F - 9. * E * E;
    double C = F * F - 3. * D * E * E;

    double DEL = B * B - 4. * A * C;

    if (__Zero_Like(D, RTol) && __Zero_Like(E, RTol) && __Zero_Like(F, RTol)) // D = E = F = 0，一个四重实根
    {
        Roots[0] = -b / (4. * a);
        Roots[1] = Roots[0];
        Roots[2] = Roots[0];
        Roots[3] = Roots[0];

        return 0;
    }

    if (!__Zero_Like(D * E * F, RTol) && __Zero_Like(A, RTol) && __Zero_Like(B, RTol) && __Zero_Like(C, RTol)) // D * E * F != 0，A = B = C = 0，一个三重实根和一个独立实根
    {
        Roots[0] = (-(b * D) + (9. * E)) / (4. * a * D);
        Roots[1] = (-(b * D) - (3. * E)) / (4. * a * D);
        Roots[2] = Roots[1];
        Roots[3] = Roots[1];

        return 1;
    }

    if (__Zero_Like(E, RTol) && __Zero_Like(F, RTol) && !__Zero_Like(D, RTol)) // E = F = 0，D != 0，两个二重根
    {
        Roots[0] = (-b + sqrt(D + 0i)) / (4. * a);
        Roots[1] = Roots[0];
        Roots[2] = (-b - sqrt(D + 0i)) / (4. * a);
        Roots[3] = Roots[2];

        return 2;
    }

    if (!__Zero_Like(A * B * C, RTol) && __Zero_Like(DEL, RTol)) // A * B * C != 0，Δ = 0，一个二重实根，若A * B > 0，则其余两根为两个不等实根；若A * B < 0，则其余两根为一对共轭虚根。
    {
        Roots[0] = (-b + ((2. * A * E) / B) + sqrt((2. * B) / A + 0i)) / (4. * a);
        Roots[1] = (-b + ((2. * A * E) / B) - sqrt((2. * B) / A + 0i)) / (4. * a);
        Roots[2] = (-b - ((2. * A * E) / B)) / (4. * a);
        Roots[3] = Roots[2];

        return 3;
    }

    if (DEL > 0) // Δ > 0，两个不等实根和一对共轭虚根
    {
        double cbrtz1 = cbrt((A * D) + (3. * ((-B + sqrt(DEL)) / 2.)));
        double cbrtz2 = cbrt((A * D) + (3. * ((-B - sqrt(DEL)) / 2.)));
        double sqrtz = sqrt((D * D) - (D * (cbrtz1 + cbrtz2)) + ((cbrtz1 + cbrtz2) * (cbrtz1 + cbrtz2)) - (3. * A));

        Roots[0] = ((-b + (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) + (sqrt(((+2. * D) - (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a));
        Roots[1] = ((-b + (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) - (sqrt(((+2. * D) - (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a));
        Roots[2] = ((-b - (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) + (sqrt(((-2. * D) + (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a)) * 1i;
        Roots[3] = ((-b - (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) - (sqrt(((-2. * D) + (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a)) * 1i;

        return 4;
    }

    if (DEL < 0) // Δ < 0，若D与F均为正数，则为四个不等实根；否则为两对不等共轭虚根。
    {
        if (__Zero_Like(E, RTol))
        {
            if (F > 0)
            {
                Roots[0] = (-b + sqrt(D + (2. * sqrt(F)) + 0i)) / (4. * a);
                Roots[1] = (-b - sqrt(D + (2. * sqrt(F)) + 0i)) / (4. * a);
                Roots[2] = (-b + sqrt(D - (2. * sqrt(F)) + 0i)) / (4. * a);
                Roots[3] = (-b - sqrt(D - (2. * sqrt(F)) + 0i)) / (4. * a);

                return 5;
            }

            if (F < 0)
            {
                Roots[0] = (((-2. * b) + sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) + (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[1] = (((-2. * b) + sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) - (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[2] = (((-2. * b) - sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) + (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[3] = (((-2. * b) - sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) - (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;

                return 6;
            }
        }

        else
        {
            double tet = arccosd(((3. * B) - (2. * A * D)) / (2. * A * sqrt(A)));
            double y1 = (D - (2. * sqrt(A) * cosd((tet / 3.)))) / 3.;
            double y2 = (D + (sqrt(A) * (cosd(tet / 3.) + sqrt(3) * sind(tet / 3.)))) / 3.;
            double y3 = (D + (sqrt(A) * (cosd(tet / 3.) - sqrt(3) * sind(tet / 3.)))) / 3.;

            double sqrty1 = sqrt(abs(y1));
            double sqrty2 = sqrt(y2); // 此时y2恒>0
            double sqrty3 = sqrt(abs(y3));

            if (D > 0 && F > 0)
            {
                Roots[0] = (-b + (sgn(E) * sqrty1) + (sqrty2 + sqrty3)) / (4. * a);
                Roots[1] = (-b + (sgn(E) * sqrty1) - (sqrty2 + sqrty3)) / (4. * a);
                Roots[2] = (-b - (sgn(E) * sqrty1) + (sqrty2 - sqrty3)) / (4. * a);
                Roots[3] = (-b - (sgn(E) * sqrty1) - (sqrty2 - sqrty3)) / (4. * a);

                return 7;
            }

            else
            {
                Roots[0] = ((-b - sqrty2) / (4. * a)) + (((sgn(E) * sqrty1) + sqrty3) / (4. * a)) * 1i;
                Roots[1] = ((-b - sqrty2) / (4. * a)) - (((sgn(E) * sqrty1) + sqrty3) / (4. * a)) * 1i;
                Roots[2] = ((-b + sqrty2) / (4. * a)) + (((sgn(E) * sqrty1) - sqrty3) / (4. * a)) * 1i;
                Roots[3] = ((-b + sqrty2) / (4. * a)) - (((sgn(E) * sqrty1) - sqrty3) / (4. * a)) * 1i;

                return 8;
            }
        }
    }

    return -1;
}

DurandKernerSolvePoly::InitValueArray DurandKernerSolvePoly::GetExponentialInitValue(double Power, std::complex<double> IValue)
{
    InitValueArray InitValue;
    for (size_t i = 0; i < Power; i++)
    {
        InitValue[i] = pow(Power, i);
    }
    return InitValue;
}

DurandKernerSolvePoly::InitValueArray DurandKernerSolvePoly::GetCircularInitValue(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs)
{
    int N = Coeffs.size() - 1;
    double R = pow(abs(Coeffs[N]), 1.0 / N);
    InitValueArray x0(N);
    for (int j = 0; j < N; ++j)
    {
        double tet = 360. * j / N;
        x0[j] = {R * cosd(tet), R * sind(tet)};
    }
    return x0;
}

DurandKernerSolvePoly::InitValueArray DurandKernerSolvePoly::GetHomotopicInitValue(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, double a)
{
    // 使用同伦加速
    // 来源：江源.应用同伦方法加速DURAND-KERNER算法[D].中国科学技术大学,2011.

    int N = Coeffs.size() - 1;
    double R = pow(abs(Coeffs[N]), 1.0 / N);

    Eigen::Matrix<double, Eigen::Dynamic, 1> Q;
    Q.resize(Coeffs.size());
    Q.setZero();
    Q[0] = 1;
    Q[N] = -std::abs(Coeffs[N]);

    Eigen::Matrix<double, Eigen::Dynamic, 1> dQ;
    dQ.resize(N);
    dQ.setZero();
    dQ[0] = N;
    Eigen::Matrix<double, Eigen::Dynamic, 1> P = Coeffs;
    auto PolyDerivative = [](Eigen::Matrix<double, Eigen::Dynamic, 1> Coefficients)
    {
        if (!Coefficients.size()) {return Eigen::Matrix<double, Eigen::Dynamic, 1>();}
        Eigen::Matrix<double, Eigen::Dynamic, 1> NewCoeffs;
        for (uint64_t i = 0; i < Coefficients.size() - 1; ++i)
        {
            NewCoeffs[i] = Coefficients[i] * (Coefficients.size() - 1 - i);
        }
        return NewCoeffs;
    };
    Eigen::Matrix<double, Eigen::Dynamic, 1> dP = PolyDerivative(P);

    InitValueArray x0;
    x0.resize(N);
    x0.setZero();
    for (int j = 0; j < N; ++j)
    {
        double tet = 360. * j / N;
        x0[j] = {R * cosd(tet), R * sind(tet)};
    }

    int64_t M = ceil(a * N);
    double h = 1.0 / M;
    double t = 0.0;

    auto f = [](Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, std::complex<double> x)->std::complex<double>
    {
        std::complex<double> SIG = 0;
        uint64_t N = Coeffs.size() - 1;
        for (size_t i = 0; i < Coeffs.size(); i++)
        {
            SIG += Coeffs[i] * pow(x, N - i);
        }
        return SIG;
    };

    InitValueArray x1;
    x1.resize(N);
    x1.setZero();
    for (int k = 0; k < M; ++k)
    {
        for (int j = 0; j < N; ++j)
        {
            std::complex<double> Q_val = f(Q, x0[j]);
            std::complex<double> P_val = f(P, x0[j]);
            std::complex<double> dQ_val = f(dQ, x0[j]);
            std::complex<double> dP_val = f(dP, x0[j]);

            std::complex<double> denominator = (1.0 - t) * dQ_val + t * dP_val;
            x1[j] = x0[j] + (Q_val - P_val) / denominator * h;
        }
        t += h;
        x0 = x1;
    }

    return x0;
}

int DurandKernerSolvePoly::Run(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>& Roots)const
{
    if (Coeffs.size() == 0) { return 0;}
    uint64_t Power = Coeffs.size() - 1;
    Roots.resize(Power);
    Roots.tail(Roots.size()).setZero();

    double Base = Coeffs[0];
    Coeffs /= Base;

    if (InitValue.size() != 0)
    {
        if (InitValue.size() != Power)
        {
            throw std::logic_error("Initial value is too few.");
        }
        Roots = InitValue;
    }
    else
    {
        switch (InitValueType)
        {
        case Exponential:
            Roots = GetExponentialInitValue(Coeffs.size());
            break;
        case Circular:
            Roots = GetCircularInitValue(Coeffs);
            break;
        case Homotopic:
            Roots = GetHomotopicInitValue(Coeffs);
            break;
        };
    }

    auto f = [&](std::complex<double> x)->std::complex<double>
    {
        std::complex<double> SIG = 0;
        for (size_t i = 0; i < Coeffs.size(); i++)
        {
            SIG += Coeffs[i] * pow(x, Power - i);
        }
        return SIG;
    };

    auto g = [&](std::complex<double> x, uint64_t Ignore)->std::complex<double>
    {
        std::complex<double> PI = 1;
        for (size_t i = 0; i < Roots.size(); i++)
        {
            if (i != Ignore) { PI *= x - Roots[i]; }
        }
        return PI;
    };

    uint64_t it = 0;
    double RealAbsError = pow(10, -AbsoluteTolerence);
    double RealRelError = pow(10, -RelativeTolerence);
    double LastError;
    uint64_t MaxIterR = pow(10, MaxIter);

    while (it < MaxIterR)
    {
        std::vector<double> Diffs;
        Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1> LastSolution = Roots;
        for (size_t i = 0; i < Power; i++)
        {
            std::complex<double> Diff = f(Roots[i]) / g(Roots[i], i);
            Diffs.push_back(std::abs(Diff));
            Roots[i] -= Diff;
        }

        double MaxDiff = 0;
        for (uint64_t i = 0; i < Power; ++i)
        {
            double LastSolution0 = std::abs(LastSolution[i]);
            double AbsoluteError = std::abs(Diffs[i]);
            double Error = (LastSolution0 > RealAbsError) ? (AbsoluteError / LastSolution0) : AbsoluteError;
            MaxDiff = std::max(MaxDiff, Error);
        }
        if (MaxDiff < RealRelError) {break;}
        //if (it && MaxDiff > LastError) {break;}

        LastError = MaxDiff;
        ++it;
    }

    return it;
}

uint64_t SolvePoly(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>& Roots, const SolvePolyRoutine& Routine)
{
    return Routine.Run(Coeffs, Roots);
}