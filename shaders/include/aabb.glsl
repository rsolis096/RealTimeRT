// Simple to test include guards

#ifndef AABB_GLSL
#define AABB_GLSL

#include "/interval.glsl"

float temp_func(Interval ins){
	return ins.min + 1.0f;
}


#endif // Must end with a newline
