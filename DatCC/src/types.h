#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

//typedef unsigned __int8   BYTE;
//typedef unsigned __int16  WORD;
//typedef unsigned __int32  DWORD;
typedef __int8            SBYTE;
typedef __int16           SWORD;
typedef __int32           SDWORD;

//#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]