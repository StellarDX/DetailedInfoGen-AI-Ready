// Fast arctan function for double and quaduple precision
// By Stephen L. Moshier, and conversion to degree math by StellarDX

#include <bit>
#include <cmath>
#include <cstdint>

#include "Trigonometry.h"

/*
    Copyright 2001 by Stephen L. Moshier <moshier@na-net.ornl.gov>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <https://www.gnu.org/licenses/>.
*/

extern "C" {

const double __Arctan128F_table_deg[84]
{
    00.0000000000000000000000000000000000000000L,
    07.1250163489017975619533008412068447490521L,
    14.0362434679264785828923201591634243209744L,
    20.5560452195834643082936127473437989791071L,
    26.5650511770779893515721937204532946712042L,
    32.0053832080834955607906457504046593579533L,
    36.8698976458440212968556125590934106575916L,
    41.1859251657096458050885863671792182784213L,
    45.0000000000000000000000000000000000000000L,
    48.3664606634298011218170598073969592388696L,
    51.3401917459099093959941376648274946222520L,
    53.9726266148963932572877357378046855130586L,
    56.3099324740202130864745054383398605797436L,
    58.3924977537510977502258106270963068609151L,
    60.2551187030577762650976882821134340914606L,
    61.9275130641470428342153596816731513580511L,
    63.4349488229220106484278062795467053287958L,
    64.7988763545249293116717660584366694053642L,
    66.0375110254218167601281706089277521020895L,
    67.1663458220824572860367076000700136633829L,
    68.1985905136481882297551339130563354323346L,
    69.1455419604216531611236876277810654402749L,
    70.0168934781000216185478097317031829447158L,
    70.8209919741892798907393496676340966291800L,
    71.5650511770779893515721937204532946712042L,
    72.2553283749430678841321949616009548382570L,
    72.8972710309476280395678768212887251445330L,
    73.4956386182449809198049843809577590645978L,
    74.0546040990771452023423104767389057414866L,
    74.5778386812613292966902479354481993917095L,
    75.0685828218624471034664630177570432654172L,
    75.5297058999341119008614748942659160353395L,
    75.9637565320735214171076798408365756790256L,
    76.3730051401084601807490599136970130157227L,
    76.7594800848127953452927039011556750268628L,
    77.1249984403875243568144698313386307384243L,
    77.4711922908484892313201264387101296497702L,
    77.7995312726192156714642002175936708679060L,
    78.1113419603720248567193715252835251474090L,
    78.4078245897089297748047548836396153355925L,
    78.6900675259797869135254945616601394202564L,
    78.9590598196762680055346197516601394192500L,
    79.2157021324374000415679466360505485960655L,
    79.4608162713717708772701905797737593789527L,
    79.6951535312339680547165811613598840136126L,
    79.9194020124576787078897222188620709784246L,
    80.1341930569156324887966534595302948823442L,
    80.3401069215576657564347059633326300317354L,
    80.5376777919743826088599294582579801842628L,
    80.7273982227996989984350074743462057825644L,
    80.9097230791776782170194932958520692926472L,
    81.0850730428521331308964493827140605307915L,
    81.2538377374447910627989195110206954742514L,
    81.4163785198860524099433092824837031614871L,
    81.5730309785193272446493026899952281397913L,
    81.7241071729247990867826969447624163298091L,
    81.8698976458440212968556125590934106575916L,
    82.0106732336031030071851597788775597853982L,
    82.1466866980217811257666345824346454330691L,
    82.2781741998690252899641892513779630860153L,
    82.4053566314085550157241367448705747504968L,
    82.5284408234076242453706847869211179952524L,
    82.6476206401076402619125548792905262505675L,
    82.7630779740319955507712233617584302621887L,
    82.8749836510982024380466991587931552509479L,
    82.9834982552770912494646989971111377847773L,
    83.0887728809753189790609806820434204280777L,
    83.1909498203865948125406219095688981093018L,
    83.2901631922430668615328987011825871328298L,
    83.3865395176852435417529328324851381837791L,
    83.4801982483430142745987074118196253418673L,
    83.5712522511701451993653221435208523289576L,
    83.6598082540900906040058623351725053777480L,
    83.7459672560835322468322981035404675931693L,
    83.8298249049703911359092144520250796594112L,
    83.9114718458048251004768203359816339928747L,
    83.9909940425054749567214190268905043079029L,
    84.0684730750802374823316651504987122315785L,
    84.1439864145710430274128234363174296908480L,
    84.2176076776359595049695176731856819893711L,
    84.2894068625003574873041186517656398885208L,
    84.3594505678431771476257293153790217278740L,
    84.4278021960362043537523667947959173771668L,
    90.0000000000000000000000000000000000000000L
};

/*
 *	Inverse circular tangent for quaduple precision based on degrees
 *      (arctangent)
 *
 * DESCRIPTION:
 *
 * Returns degree double between -90 and +90 whose tangent is x.
 *
 * The function uses a rational approximation of the form
 * t + t^3 P(t^2)/Q(t^2), optimized for |t| < 0.09375.
 *
 * The argument is reduced using the identity
 *    arctan x - arctan u  =  arctan ((x-u)/(1 + ux))
 * and an 83-entry lookup table for arctan u, with u = 0, 1/8, ..., 10.25.
 * Use of the table improves the execution speed of the routine.
 *
 * ACCURACY:
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -19, 19       4e5       1.7e-34     5.4e-35
 *
 * WARNING:
 *
 * This program uses integer operations on bit fields of floating-point
 * numbers.  It does not work with data structures other than the
 * structure assumed.
 */

double __cdecl __IEEE754_ATAN128F_C64F(double x)
{
    int32_t k, sign, lx;
    double t, u, p, q;
    double xhi;

    xhi = x;
    k = std::bit_cast<uint64_t>(x) >> 32;
    lx = std::bit_cast<uint64_t>(x) & (~uint32_t(0));
    sign = k & 0x80000000;

    // #ifndef TRIGONOMETRY_USE_RADIANS
    const double* __ArctanF128_table = __Arctan128F_table_deg;
    // #else
    // const double* __ArctanF128_table =
    //     SwitchUnit ? __Arctan128F_table_deg : __Arctan128F_table_rad;
    // #endif

    /* Check for IEEE special cases.  */
    k &= 0x7fffffff;
    if (k >= 0x7ff00000)
    {
        /* NaN. */
        if (((k - 0x7ff00000) | lx) != 0)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        /* Infinity. */
        if (sign) { return -__ArctanF128_table[83]; }
        else { return __ArctanF128_table[83]; }
    }

    static const double
        R2D = 57.2957795130823208767981548141051703324054724665643215491602438612028471483215526324409689958511109442;

    if (k <= 0x3c800000) /* |x| <= 2**-55.  */
    {
        /* Raise inexact.  */
        if (1e300L + x > 0.0) { return x * R2D; }
    }

    if (k >= 0x46c00000) /* |x| >= 2**109.  */
    {
        /* Saturate result to {-,+}90.  */
        if (sign) { return -__ArctanF128_table[83]; }
        else { return __ArctanF128_table[83]; }
    }

    if (sign) { x = -x; }

    if (k >= 0x40248000) /* 10.25 */
    {
        k = 83;
        t = -1.0 / x;
    }
    else
    {
        /* Index of nearest table element.
       Roundoff to integer is asymmetrical to avoid cancellation when t < 0
           (cf. fdlibm). */
        k = int32_t(8.0 * x + 0.25);
        u = 0.125 * k;
        /* Small arctan argument.  */
        t = (x - u) / (1.0 + x * u);
    }

    /* Arctan of small argument t.  */
    static double
        p0 = -4.283708356338736809269381409828726405572E+1L,
        p1 = -8.636132499244548540964557273544599863825E+1L,
        p2 = -5.713554848244551350855604111031839613216E+1L,
        p3 = -1.371405711877433266573835355036413750118E+1L,
        p4 = -8.638214309119210906997318946650189640184E-1L,
        q0 = +1.285112506901621042780814422948906537959E+2L,
        q1 = +3.361907253914337187957855834229672347089E+2L,
        q2 = +3.180448303864130128268191635189365331680E+2L,
        q3 = +1.307244136980865800160844625025280344686E+2L,
        q4 = +2.173623741810414221251136181221172551416E+1L;

    u = t * t;
    p = ((((p4 * u) + p3) * u + p2) * u + p1) * u + p0;
    q = ((((u + q4) * u + q3) * u + q2) * u + q1) * u + q0;
    u = t * u * p / q + t;

    // arctan x = arctan u  +  arctan t
    // 丹霞：这里由于剩余部分已经非常小，单位转换造成的误差已经可以忽略不计
    // 故此处直接将剩余部分进行单位转换后加上对应表中的值。
    u = __ArctanF128_table[k] + (u * R2D);
    if (sign) { return (-u); }
    else { return u; }
}

double __cdecl arctand(double x) {return __IEEE754_ATAN128F_C64F(x);}
double __cdecl arccotd(double x) {return __IEEE754_ATAN128F_C64F(1. / x);}

/* arctan2(y, x)
 * Method :
 *	1. Reduce y to positive by atan2l(y,x)=-atan2l(-y,x).
 *	2. Reduce x to positive by (if x and y are unexceptional):
 *		ARG (x+iy) = arctan(y/x)	   ... if x > 0,
 *		ARG (x+iy) = pi - arctan[y/(-x)]   ... if x < 0,
 *
 * Special cases:
 *
 *	ATAN2((anything), NaN ) is NaN;
 *	ATAN2(NAN , (anything) ) is NaN;
 *	ATAN2(+-0, +(anything but NaN)) is +-0  ;
 *	ATAN2(+-0, -(anything but NaN)) is +-pi ;
 *	ATAN2(+-(anything but 0 and NaN), 0) is +-pi/2;
 *	ATAN2(+-(anything but INF and NaN), +INF) is +-0 ;
 *	ATAN2(+-(anything but INF and NaN), -INF) is +-pi;
 *	ATAN2(+-INF,+INF ) is +-pi/4 ;
 *	ATAN2(+-INF,-INF ) is +-3pi/4;
 *	ATAN2(+-INF, (anything but,0,NaN, and INF)) is +-pi/2;
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */
double __cdecl arctan2d(double _Y, double _X)
{
    // Use Sun Microsystem's implementation, and convert to Degrees

    if (std::isnan(_Y) || std::isnan(_X)) {return std::numeric_limits<double>::quiet_NaN();}
    if (_X == 1) {return arctand(_Y);}

    int sign_x = std::signbit(_X) ? -1 : 1;
    int sign_y = std::signbit(_Y) ? -1 : 1;
    int m = (sign_x < 0 ? 2 : 0) + (sign_y < 0 ? 1 : 0);

    if (_X == 1) {return arctand(_Y);}

    if (_Y == 0)
    {
        switch(m)
        {
            case 0:
            case 1: return double(_Y);	/* atan(+-0,+anything)=+-0 */
            case 2: return double(180); /* atan(+0,-anything) = pi */
            case 3: return double(-180);/* atan(-0,-anything) =-pi */
        }
    }

    if (_X == 0)
    {
        return (sign_y < 0) ? -90 : 90;
    }

    if (std::isinf(_X))
    {
        if (std::isinf(_Y))
        {
            switch(m)
            {
                case 0: return double(45);   /* atan(+INF,+INF) */
                case 1: return double(-45);  /* atan(-INF,+INF) */
                case 2: return double(135);  /*atan(+INF,-INF)*/
                case 3: return double(-135); /*atan(-INF,-INF)*/
            }
        }
        else
        {
            switch(m)
            {
                case 0: return double(0);          /* atan(+...,+INF) */
                case 1: return double
                    (std::bit_cast<double>(1ULL << 63)); /* atan(-...,+INF) */
                case 2: return double(180);        /* atan(+...,-INF) */
                case 3: return double(-180);       /* atan(-...,-INF) */
            }
        }
    }

    if (std::isinf(_Y))
    {
        return double((sign_y < 0) ? -90 : 90);
    }

    double z, ratio = std::abs(_Y / _X);
    if (ratio > 1.0e120)
    {
        /* |y/x| > 2^120 */
        z = 90;
    }
    else if (sign_x < 0 && ratio < 1.0e-120)
    {
        /* |y/x| < 2^-120 */
        z = 0;
    }
    else
    {
        z = arctand(ratio); /* safe to do y/x */
    }

    switch (m)
    {
        case 0: return double(z);       /* atan(+,+) */
        case 1: return double(-z);      /* atan(-,+) */
        case 2: return double(180 - z); /* atan(+,-) */
        case 3: return double(z - 180); /* atan(-,-) */
    }
    return double(z);
}

}

#if 0 // Test program for arctan generated by Deepseek
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>

int main()
{
    std::vector<double> test_values =
    {
        0.0,
        0.0001,
        0.1,
        0.5,
        1.0,
        1.5,
        2.0,
        5.0,
        10.0,
        100.0,
        1000.0,
        10000.0,
        0.2679491924311227064725536584941,
        0.577350269189625764509148780502,
        1.7320508075688772935274463415059,
        -0.1,
        -1.0,
        -10.0,
        -100.0,
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::quiet_NaN(),
        1e-100,
        1e100
    };

    std::cout << std::setprecision(16);

    std::cout << "\nTesting atan in degrees:\n";
    std::cout << std::setw(20) << "Input"
              << std::setw(25) << "Custom atan"
              << std::setw(25) << "Std atan (converted)"
              << std::setw(25) << "Difference" << "\n";

    for (double x : test_values)
    {
        double custom = __IEEE754_ATAN128F_C64F(double(x));
        double std_atan_deg = std::atan(x) * 180.0 / M_PI;
        double diff = std::abs(custom - std_atan_deg);

        std::cout << std::setw(20) << x
                  << std::setw(25) << custom
                  << std::setw(25) << std_atan_deg
                  << std::setw(25) << diff << "\n";
    }

    // 测试边界条件
    std::cout << "\nTesting special cases:\n";
    double inf = std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();

    auto test_special = [](double x, const char* name)
    {
        double custom_deg = __IEEE754_ATAN128F_C64F(double(x));
        double std_atan = std::atan(x);
        double std_atan_deg = std_atan * 180.0 / M_PI;

        std::cout << "Case: " << name << " (" << x << ")\n";
        std::cout << "  Degrees - Custom: " << custom_deg << ", Std: " << std_atan_deg << "\n\n";
    };

    test_special(inf, "Positive Infinity");
    test_special(-inf, "Negative Infinity");
    test_special(nan, "NaN");
    test_special(0.0, "Zero");
    test_special(-0.0, "Negative Zero");
    test_special(1e-100, "Very small value");
    test_special(1e100, "Very large value");
}
#endif