#include <cstdio>
#include <cmath>

#include "vec3d.hpp"

vec3d::vec3d(const vec3d &v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

vec3d vec3d::operator+(const vec3d &v) const {
    return vec3d(x+v.x, y+v.y, z+v.z);
}

vec3d vec3d::operator-(const vec3d &v) const {
    return vec3d(x-v.x, y-v.y, z-v.z);
}

double vec3d::operator*(const vec3d &v) const {
    return x*v.x + y*v.y + z*v.z;
}

vec3d vec3d::operator*(double d) const {
    return vec3d(x*d, y*d, z*d);
}

vec3d vec3d::operator/(double d) const {
	return vec3d(x/d, y/d, z/d);
}

vec3d vec3d::operator^(const vec3d &v) const {
    return vec3d(y*v.z - z*v.y,  z*v.x - x*v.z, x*v.y - y*v.x);
}

vec3d &vec3d::operator =(const vec3d &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

vec3d &vec3d::normalize() {
    return *this = (*this) * (1 / sqrt(x*x + y*y + z*z));
}

vec3d vec3d::yMultiple(const vec3d &v) const {
	return vec3d(x*v.x, y*v.y, z*v.z);
}