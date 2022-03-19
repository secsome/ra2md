#pragma once

// Warning disables
#pragma warning(disable : 4996)
#pragma warning(disable : 6031)
#pragma warning(disable : 26812)
#pragma warning(disable : 28251)

// Windows Headers
#include <Windows.h>

// C++ Headers
#include <compare>

#include <string>
#include <string_view>
#include <sstream>

// C Headers
#include <cstring>

// Class Helpers
#define CLASS_NOCOPY(T) T(const T& another) = delete; T& operator= (const T& another) = delete;
#define CLASS_CANCOPY(T) T(const T& another); T& operator= (const T& another);
#define CLASS_CANDEFAULTCOPY(T) T(const T& another) = default; T& operator= (const T& another) = default;
#define CLASS_NOMOVE(T) T(T&& another) = delete; T& operator= (T&& another) = delete;
#define CLASS_CANMOVE(T) T(T&& another) = default; T& operator= (T&& another) = default;

// Enum Helpers

// Macros
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef MAX
#undef MAX
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define CLAMP(x, l, u) (((x) < (l)) ? (l) : (((x) > (u)) ? (u) : (x)))

// Template Helpers
