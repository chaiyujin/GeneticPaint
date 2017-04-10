/* -----------------------------
* File   : defines.h
* Author : Yuki Chai
* Created: 2017.03.31
* Project: Yuki
*/
#pragma once
#ifndef __YUKI_UTILS_DEFINES_H__
#define __YUKI_UTILS_DEFINES_H__

#include <cstdarg>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <memory>
#include <sys/stat.h>
#include <type_traits>

/* some useful definition */
#define FOR(x, y) \
	for (typename std::remove_cv<typename std::remove_reference<decltype(y)>::type>::type x = 0;  x < (y); ++x)

// [s, e) increase from s
#define FORINC(x, s, e, step) \
	for (typename std::remove_cv<typename std::remove_reference<decltype(e)>::type>::type x = s; x < (e); x += step)

// [s, e) decrease from e - 1
#define FORDEC(x, s, e, step) \
	for (typename std::remove_cv<typename std::remove_reference<decltype(e)>::type>::type x = e - 1; x >= (s); x -= step)

namespace Yuki {
	/* some simple functions */
	// square of x
	template <class T>
	inline T sqr(T x) { return x * x; }

	// equal 
	inline bool equal(float x, float y) {
		return fabs(x - y) < 0.000001;
	}
	inline bool equal(double x, double y) {
		return fabs(x - y) < 0.00000001;
	}
	inline bool equal(int x, int y) {
		return x == y;
	}
}

#endif  // !__YUKI_UTILS_DEFINES_H__