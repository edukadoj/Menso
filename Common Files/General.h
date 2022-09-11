#ifndef GeneralH
#define GeneralH

#define __STDC__ __STDC__
#ifndef UNICODE
#define UNICODE
#endif
#ifndef STRICT
#define STRICT
#endif
//#define _WIN32_IE 0x0700

#include <windows.h>
#include <string>
#include <iostream>
//using namespace std;



using Unsigned = unsigned long long; // can be either "unsigned long long" or "unsigned"
const auto MAXUnsigned = Unsigned(-1);


using Uchar = unsigned char;

extern const char* app_name;
extern const wchar_t* window_title;

inline void print_error(const wchar_t*
#ifdef _DEBUG
    message
#endif
)
{
#ifdef _DEBUG
    MessageBoxW(0, message, message, MB_OK);
#endif
    throw 1;
}

inline void print_error(const char* 
#ifdef _DEBUG
    message
#endif
)
{
#ifdef _DEBUG
    MessageBoxA(0, message, message, MB_OK);
#endif
    throw 1;
}


template <typename T>
T
#ifndef _DEBUG
&
#endif
val(
#ifdef _DEBUG
    const
#endif
    T& x)
{
    return x;
}


#endif