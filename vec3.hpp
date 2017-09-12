#ifndef VEC3
#define VEC3

struct Vec3
{
	Vec3()
		: x_{0.0}
		, y_{0.0}
		, z_{0.0}
		, phi_{0.0}
	{}

	Vec3(float x, float y, float z, float phi)
		: x_{x}
		, y_{y}
		, z_{z}
		, phi_{0.0}
	{}

	float x_;
	float y_;
	float z_;
	float phi_;
};

#endif // VEC3