
#include "ray.h"

/*
float planeIntersection (const Ray& ray, const Plane& plane)
{
	float d = -(plane.Normal.Dot(plane.Point));

	float num = ray.Origin.Dot(plane.Normal) + d;
	float den = ray.Dir.Dot(plane.Normal);

	if (den == 0) return -1.0f;

	return -(num/den);
}
*/

bool RayBoxIntersection (
		const Ray& ray,
		const Box& box,
		Matrix34& model,	// world model matrix
		float& distance)		// out: distance for collision
{
	float tMin = 0.0f;
	float tMax = 100000.0f;
	float eps = 0.001f;

	Vector3 worldspace_pos(model[3].x, model[3].y, model[3].z);
	Vector3 delta(worldspace_pos);
	delta.Subtract(ray.Origin);

	{
		Vector3 xaxis(model[0].x,model[0].y,model[0].z);
		float e = xaxis.Dot(delta);
		float f = ray.Dir.Dot(xaxis);

		if(fabs(f)>eps) {
			float t1 = (e+box.Min.x)/f;
			float t2 = (e+box.Max.x)/f;

			if(t1>t2) { float w=t1;t1=t2;t2=w; }

			if(t2<tMax) tMax=t2;
			if(t1>tMin) tMin=t1;
			if(tMax<tMin) return false;
		} else {
			if(-e+box.Min.x>0.f||-e+box.Max.x<0.f)
				return false;
		}
	}

	{
		Vector3 yaxis(model[1].x,model[1].y,model[1].z);
		float e = yaxis.Dot(delta);
		float f = ray.Dir.Dot(yaxis);

		if(fabs(f)>eps) {
			float t1 = (e+box.Min.y)/f;
			float t2 = (e+box.Max.y)/f;

			if(t1>t2) { float w=t1;t1=t2;t2=w; }

			if(t2<tMax) tMax=t2;
			if(t1>tMin) tMin=t1;
			if(tMax<tMin) return false;
		} else {
			if(-e+box.Min.y>0.f||-e+box.Max.y<0.f)
				return false;
		}
	}

	{
		Vector3 zaxis(model[2].x,model[2].y,model[2].z);
		float e = zaxis.Dot(delta);
		float f = ray.Dir.Dot(zaxis);

		if(fabs(f)>eps) {
			float t1 = (e+box.Min.z)/f;
			float t2 = (e+box.Max.z)/f;

			if(t1>t2) { float w=t1;t1=t2;t2=w; }

			if(t2<tMax) tMax=t2;
			if(t1>tMin) tMin=t1;
			if(tMax<tMin) return false;
		} else {
			if(-e+box.Min.z>0.f||-e+box.Max.z<0.f)
				return false;
		}
	}

	distance = tMin;
	return true;
}

