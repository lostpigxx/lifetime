#pragma once

class vec3d {
public:
    double x, y, z;

    vec3d(double _x = 0, double _y = 0, double _z = 0):x(_x), y(_y), z(_z) {}
    vec3d(const vec3d &v);

    vec3d operator+(const vec3d &v) const;
    vec3d operator-(const vec3d &v) const;
    double operator*(const vec3d &v) const; //点乘
    vec3d operator*(double d) const;        //标量乘法
	vec3d operator/(double d) const;        //标量除法
    vec3d operator^(const vec3d &v) const;  //叉乘
    vec3d &operator=(const vec3d &v);
    vec3d &normalize();                     //归一化
	vec3d yMultiple(const vec3d &v) const;
};