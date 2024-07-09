#define PI 3.1415926535897932384626433832795f

template <typename T>
T lerp(T x1, T x2, T a)
{
	return x1 * (1 - a) + x2 * a;
}

float DegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}

float RadiansToDegrees(float radians)
{
	return radians*180.f/PI;
}

template <typename T>
T clamp(T x, T left, T right)
{
	return min(max(x, left), right);
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}