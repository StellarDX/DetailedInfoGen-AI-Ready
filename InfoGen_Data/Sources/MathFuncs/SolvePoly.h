/////////////////////////////////////////////////////////////////
//                            解方程                            //
//                                                             //
// 求解由以下形式定义的多项式方程的解(降幂排序)                    //
//      n                                                      //
//      Σ (a_i * x^(n - i - 1)) = 0                            //
//     i=0                                                     //
//                                                             //
// 其中一次和二次使用求根公式，三次使用范盛金的算法，四次使用沈天珩  //
// 的算法，五次及以上使用杜兰德-肯纳迭代。                         //
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SOLVEPOLY__
#define __SOLVEPOLY__

#include "Eigen/Core"

using std::literals::operator""i;

/**
 * @brief 求解线性方程，如 ax + b = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 */
int SolveLinear(Eigen::Matrix<double, 2, 1> Coeffs, Eigen::Matrix<std::complex<double>, 1, 1>& Roots);

/**
 * @brief 求解二次方程，如 ax^2 + bx + c = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 */
int SolveQuadratic(Eigen::Matrix<double, 3, 1> Coeffs, Eigen::Matrix<std::complex<double>, 2, 1>& Roots);

/**
 * @brief 求解三次方程，如 ax^3 + bx^2 + cx + d = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 * @param Tolerence 误差的负对数
 */
int SolveCubic(Eigen::Matrix<double, 4, 1> Coeffs, Eigen::Matrix<std::complex<double>, 3, 1>& Roots, double Tolerence = 10);

/**
 * @brief 求解四次方程，如 ax^4 + bx^3 + cx^2 + dx + e = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 * @param Tolerence 误差的负对数
 */
int SolveQuartic(Eigen::Matrix<double, 5, 1> Coeffs, Eigen::Matrix<std::complex<double>, 4, 1>& Roots, double Tolerence = 10);

/* ************************************************************************** *\
   丹霞：寻找五次或以上方程的解法是一个困扰了数学家们300多年的问题。历史上无数的数学
   家试图推导这些多项式方程求解算法，但无一例外都失败了。1824年，阿贝尔等人证明了五次
   及更高次的多项式方程没有一般的代数解法，即这样的方程不能由方程的系数经有限次四则运
   算和开方运算求解。

   1926年，苏家驹发表了一篇文章，声称自己发现了一个五次方程的求解算法。随后华罗庚在看
   了苏家驹的文章以后尝试继续推导六次方程的求解算法，这意味着他要尝试否定先前阿贝尔等
   人已经证实的“五次以上方程的解无法用系数表示”这一理论，但是当他看了阿贝尔的论文以后，
   认为其“条理精严，无懈可击”。随后他就转而开始尝试在苏家驹的论文中寻找漏洞，最终发表
   了他的成名之作《苏家驹之代数的五次方程式解法不能成立之理由》。

   阿贝尔定律已经证实五次以上方程的解无法使用系数组成的根式表示。也就是这些方程没有通
   用的求解算法。但是，这并不意味着它们完全没办法求解，此处使用杜兰德-肯纳算法求解这
   些方程。

   参考文献：
    [1] 苏家驹.代数的五次方程式之解法[J].学艺,1926.
    [2] 华罗庚.来件——苏家驹之代数的五次方程式解法不能成立之理由[J].科学,1930.
    [3] Wikipedia Contributors. Durand–Kerner method — Wikipedia, The Free
        Encyclopedia[A/OL]. (2025).
        https://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
\* ************************************************************************** */

struct SolvePolyRoutine
{
    virtual int Run(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>& Roots)const = 0;
};

class DurandKernerSolvePoly : public SolvePolyRoutine
{
public:
    using InitValueArray = Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>;

    enum InitValueConfig {Exponential, Circular, Homotopic};

    InitValueArray InitValue = {};
    double AbsoluteTolerence = 14;
    double RelativeTolerence = 14;
    double MaxIter = 3;
    InitValueConfig InitValueType = Circular;

    static InitValueArray GetExponentialInitValue(double Power, std::complex<double> IValue = 0.4 + 0.9i);
    static InitValueArray GetCircularInitValue(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs);
    static InitValueArray GetHomotopicInitValue(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, double a = 0.5);

    int Run(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>& Roots)const override;
};

using DefaultSolvePolyRoutine = DurandKernerSolvePoly;

/**
 * @brief 求解任意多项式方程的解
 * http://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
 * @param Coeffs 多项式的系数，降幂排序
 * @param Roots 方程的解
 * @param Routine 求解算法，默认使用杜兰德-肯纳算法
 * @return 迭代次数
 */
uint64_t SolvePoly(Eigen::Matrix<double, Eigen::Dynamic, 1> Coeffs, Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1>& Roots, const SolvePolyRoutine& Routine = DefaultSolvePolyRoutine());

#endif