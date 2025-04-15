#define PI 3.1415926535897932384626433832795f

float lerp(float x1, float x2, float a)
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

float clamp(float x, float left, float right)
{
	return min(max(x, left), right);
}

int clamp(int x, int left, int right)
{
	return min(max(x, left), right);
}


float sign(float val) {
	return (float)((0.f < val) - (val < 0.f));
}