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

float length3(const float4& v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float smoothstep(float edge0, float edge1, float x) {
	float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}


// Вспомогательная функция сплайна Кэтмулла-Рома
float4 catmullRom(const float4& p0, const float4& p1, const float4& p2, const float4& p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	float4 result;
	result.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
	result.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
	result.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);
	result.w = 0.5f * ((2.0f * p1.w) + (-p0.w + p2.w) * t + (2.0f * p0.w - 5.0f * p1.w + 4.0f * p2.w - p3.w) * t2 + (-p0.w + 3.0f * p1.w - 3.0f * p2.w + p3.w) * t3);

	return result;
}

float getRandFloat()
{
	return (rand() % 1000) / 500. - 1.;
}

float4 getRandFloat4()
{
	return {
		getRandFloat(),
		getRandFloat(),
		getRandFloat(),
		getRandFloat()
	};
}

