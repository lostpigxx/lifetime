#include <cmath>

#include "object.hpp"


// 基类
rootObject::rootObject() {}

rootObject::~rootObject() {}


// Parameters
// r: ray information
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection
// www.cnblogs.com/samen168/p/5162337.html
bool rootObject::intersectTriangle(const ray &r, const vec3d v0, const vec3d v1, const vec3d v2,
	double *t, double *u, double *v) {
	// E1
	vec3d E1 = v1 - v0;

	// E2
	vec3d E2 = v2 - v0;

	// P
	vec3d P = r.direction ^ E2;

	// determinant
	double det = E1 * P;

	// keep det > 0, modify T accordingly
	vec3d T;
	if (det >0)
	{
		T = r.origin - v0;
	}
	else
	{
		T = v0 - r.origin;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = T * P;
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	vec3d Q = T ^ E1;

	// Calculate v and make sure u + v <= 1
	*v = r.direction * Q;
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = E2 * Q;

	double fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;

}

// 计算三角面片的法线
vec3d rootObject::calcNormal(const vec3d a, const vec3d b, const vec3d c) {
	vec3d ab = b - a;
	vec3d ac = c - a;
	return (ab ^ ac).normalize();
}


void rootObject::hit(intersaction &inter, std::vector<vec3d> vertexList, ray r) {
	for (int i = 0; i < faceList.size(); i++) {
		std::vector<int> &face = faceList[i];
		int faceSize = (int)face.size();
		if (faceSize < 3 || faceSize > 4) {
			printf("Error in hit function, since face vertexs can only be 3 or 4\n");
			return;
		}

		bool isHitTriangle;
		double t, u, v;
		isHitTriangle = intersectTriangle(r, 
			vertexList[face[0]], vertexList[face[1]], vertexList[face[2]], &t, &u, &v);
		if (!isHitTriangle && faceSize == 4) {
			isHitTriangle = intersectTriangle(r,
				vertexList[face[0]], vertexList[face[2]], vertexList[face[3]], &t, &u, &v);
		}
		
		if (isHitTriangle && t > 1e-4 && t < inter.min_t) {
			inter.isIntersacted = true;
			inter.min_t = t;
			inter.pos = r.origin + r.direction * t;
			inter.normal = calcNormal(vertexList[face[0]], vertexList[face[1]], vertexList[face[2]]);
			inter.obj = this;
		}
	}
}


// sphere
sphere::sphere() {}
sphere::~sphere() {}

void sphere::hit(intersaction &inter, std::vector<vec3d> vertexList, ray r) {
	vec3d PR = center - r.origin;
	r.direction.normalize();
	double t = PR * r.direction;
	double distance = sqrt(PR * PR - t * t);
	if (distance >= radius) 
		return;

	double delta = sqrt(radius * radius - distance * distance);
	double t1 = t - delta;
	double t2 = t + delta;

	if (t1 > 1e-4 && t1 < inter.min_t) {
		inter.isIntersacted = true;
		inter.min_t = t1;
		inter.obj = this;
		inter.pos = r.origin + r.direction * t1;
		inter.normal = (inter.pos - center).normalize();
	}
	else if (t1 < 1e-4 && t2 > 1e-4 && t2 < inter.min_t) {
		inter.isIntersacted = true;
		inter.min_t = t2;
		inter.obj = this;
		inter.pos = r.origin + r.direction * t2;
		inter.normal = (center - inter.pos).normalize();
	}
}

