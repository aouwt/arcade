#include <math.h>

#include "math.hpp"

template <typename T>
inline T cos(T x) noexcept {
	constexpr T tp = 1./(2.*M_PI);
	x *= tp;
	x -= T(.25) + std::floor(x + T(.25));
	x *= T(16.) * (std::abs(x) - T(.5));
	return x;
}

template <typename T>
inline T sin(T x) noexcept {
	return cos(x + (M_PI/2));
}
