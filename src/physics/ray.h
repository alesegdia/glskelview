#ifndef __RAY_H__
#define __RAY_H__

#include "../math/vector3.h"
#include "../math/matrix34.h"

struct Ray {
	Vector3 Origin;
	Vector3 Dir;
	float Distance;
};

struct Plane {
	Vector3 Point;
	Vector3 Normal;
};

struct Box {
	Box(const Vector3& min, const Vector3& max)
	{ Min.Set(min); Max.Set(max); }
	Vector3 Min;
	Vector3 Max;
};

bool RayBoxIntersection (
		const Ray& ray,
		const Box& box,
		Matrix34& model,
		float& distance);


#endif

