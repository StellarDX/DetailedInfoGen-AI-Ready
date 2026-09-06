#pragma once

#ifndef __TRIGONOMETRY__
#define __TRIGONOMETRY__

#ifdef __linux
#define __cdecl __attribute__((cdecl))
#endif

extern "C" 
{
    // 专为角度设计的三角函数，实测精度比标准库更高
    double __cdecl sind(double x);
    double __cdecl cosd(double x);
    double __cdecl tand(double x);
    double __cdecl cotd(double x);
    // 这两个是直接求1/sin和1/cos的，精度可能丢失，但是没什么好的办法（说难听点就是没活了）
    double __cdecl secd(double x);
    double __cdecl cscd(double x);

    // 角度反三角函数
    double __cdecl arcsind(double x);
    double __cdecl arccosd(double x);
    double __cdecl arctand(double x);
    double __cdecl arccotd(double x);
    double __cdecl arcsecd(double x);
    double __cdecl arccscd(double x);

    // 二维反正切函数
    double __cdecl arctan2d(double _Y, double _X);
}

#endif