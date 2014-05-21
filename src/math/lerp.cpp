#include "lerp.h"


float lerp(float u, float t0, float t1) {
	return (1-u)*t0 + u*t1;
}

float invlerp(float t, float t0, float t1) {
	return (t-t0)/(t1-t0);
}
