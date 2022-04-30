#include <math.h>

// https://stackoverflow.com/a/28050328/14287583
template <typename T>
inline T fastcos (T x) noexcept {
	constexpr T tp = 1.0 / (2.0 * M_PI);
	x *= tp;
	x -= T (0.25) + std::floor (x + T (0.25));
	x *= T (16.0) * (std::abs (x) - T (0.5));
	return x;
}

template <typename T>
inline T fastsin (T x) noexcept {
	return fastcos (x + (M_PI / 2.0));
}
