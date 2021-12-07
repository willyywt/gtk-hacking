#pragma once
#include "framework.h"
size_t HW_ScanInt(const char* text, int* pval);
size_t HW_ScanDouble(const char* text, double* pval);
size_t HW_JumpSpace(char* buf);
int issep(int C);
int double_equal(double x, double y);

/* For c++ enum bitwise operations to suppress compile error between int and enum.
 * If the compile errors persist you must have passed incompatiable types to bitwise operators.
 */
#ifdef __cplusplus
template<class T> inline T operator~ (T a) { return (T)~(int)a; }
template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }
#endif
