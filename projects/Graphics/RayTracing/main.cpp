#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <omp.h>
#include <algorithm>

#include "vec3d.hpp"
#include "object.hpp"
#include "objReader.hpp"

#define M_PI 3.14159265358979323846   // pi

struct config {
	int width;
	int height;
	int samples;
	vec3d envColor;
	double envIntensity;
	vec3d lightColor;
	double lightIntensity;
	int maxDepth;
};

config gc;
config gc2;

class camera {
public:
	vec3d pos, front, up;

	void setCamera(vec3d _pos, vec3d _front, vec3d _up) {
		this->pos = _pos;
		this->front = _front;
		this->up = _up;
	}
};

camera ca;

objReader *objr;

inline double randRT(void) {
    return (double)((double)rand() / (double)RAND_MAX);
}

inline double clamp(double x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;
} 

inline int setColor(double x) {
    return int(pow(clamp(x), 1/2.2) * 255 + 0.5);
}


vec3d calcRefraction(vec3d &V, vec3d &N, double Ni, bool &inside) {
	double nr = !inside ? 1.0f / Ni : Ni;
	inside = !inside;
	vec3d T = N * (nr * (N * V) - sqrt(1 - nr * nr * (1 - pow(N * V, 2)))) - V * nr;
	return T;
}

vec3d randomReflection(vec3d &N, double phiIn) {
	double theta = 2.0* M_PI * randRT();
	double phi = phiIn > 1e-5 ? phiIn : asin(randRT()) + 0.01;
	vec3d u = fabs(N.x) > 0.5 ? vec3d(0, 1, 0) : vec3d(1, 0, 0);
	u = (u ^ N).normalize();
	vec3d v = N ^ u;
	vec3d d = (u*cos(theta) + v*sin(theta))*cos(phi) + N*sin(phi);
	return d.normalize();
}

vec3d calcReflection(vec3d &V, vec3d &N, material *mat) {
	double Ns = mat->Ns;
	if (Ns <= 1.0 || randRT()< 1.5 / sqrt(Ns))
		return randomReflection(N, 0);
	vec3d S = N * 2 * (N * V) - V;
	double phi1 = M_PI / 2.0 * (1 - 1 / sqrt(Ns));
	double phi2 = acos((S * N) / sqrt(S * S));
	double maxPhi = std::max(phi1, phi2);
	double phi = maxPhi + randRT() * (M_PI / 2.0 - maxPhi);
	return randomReflection(S, phi);
}


vec3d pathTracing(ray r, int depth, bool inside) {
	if (depth >= gc.maxDepth) {
		return vec3d(0, 0, 0);
	}

	intersaction inter;
	inter.isIntersacted = false;
	inter.min_t = DBL_MAX;
	for (int i = 0; i < objr->objectList.size(); i++)
		objr->objectList[i]->hit(inter, objr->vertexList, r);

	if (!inter.isIntersacted) {
		return gc.envColor * gc.envIntensity;
	}

	if (inter.obj->mat->isLight) {
		return gc.lightColor * gc.lightIntensity;
	}

	//phong model
	vec3d N;
	if (inter.normal * r.direction > 0)
		N = inter.normal * -1;
	else
		N = inter.normal;
	double Ns = inter.obj->mat->Ns;
	vec3d V = r.direction * -1;

	double Tr = inter.obj->mat->Tr;
	bool refraction = randRT() < Tr ? true : false;

	vec3d L;
	if (Tr > 1e-4 && refraction)
		L = calcRefraction(V, N, inter.obj->mat->Ni, inside);
	else
		L = calcReflection(V, N, inter.obj->mat);


	ray nextR(inter.pos, L);
	vec3d Ii = pathTracing(nextR, depth + 1, inside);

	if (Ii.x + Ii.y + Ii.z < 1e-6)
	{
		return vec3d(0, 0, 0);
	}

	vec3d Ie;
	if (!refraction) {
		vec3d Ka = inter.obj->mat->Ka;
		vec3d Ia = gc.envColor * gc.envIntensity;
		vec3d Kd = inter.obj->mat->Kd;
		vec3d Ks = inter.obj->mat->Ks;
		vec3d R = N * 2 * (N * L) - L;
		Ie = Ka.yMultiple(Ia) + Ii.yMultiple(Kd*(N*L) + Ks*pow(V*R, Ns));
	}
	else {
		Ie = Ii;
	}
	return Ie;
}



int main() {

    srand((unsigned int)time(NULL));


	// 场景1
	// 读取obj文件
	objr = new objReader();
	if (!objr->readObjFile("scene01.obj")) {
		printf("Error in reading obj files\n");
	}
	// 参数设定
	gc.width = 400;
	gc.height = 300;
	gc.samples = 10000;
	gc.envColor = vec3d(1, 1, 1);
	gc.envIntensity = 0;
	gc.lightColor = vec3d(1, 1, 1);
	gc.lightIntensity = 50;
	gc.maxDepth = 9;
	// 视角
	vec3d caPos(0, 4.5, 14.5);
	vec3d caFront(0, 0, -1);
	vec3d caUp(0, 1, 0);
	ca.setCamera(caPos, caFront, caUp);

	// 场景2
	//objr = new objReader();
	//if (!objr->readObjFile("scene02.obj")) {
	//	printf("Error in reading obj files\n");
	//}
	//gc.width = 400;
	//gc.height = 300;
	//gc.samples = 5000;
	//gc.envColor = vec3d(1, 1, 1);
	//gc.envIntensity = 0.9;
	//gc.lightColor = vec3d(1, 1, 1);
	//gc.lightIntensity = 50;
	//gc.maxDepth = 9;
	//vec3d caPos(1.5, 8.0, 19);
	//vec3d caFront(0, -0.35, -1);
	//vec3d caUp(0, 1, 0);
	//ca.setCamera(caPos, caFront, caUp);

    // 主程序
	time_t start, stop;
	time(&start);
	printf("Start Tracing...\n");

	// incremental algorithm
	double fov = 60.0f;
	double nearClipZ = 0.1f;
	double tx = tan(fov / 2.0f * M_PI / 180.0f) * nearClipZ;
	double aspect_ratio = gc.width * 1.0f / gc.height;
	double ty = tx / aspect_ratio;
	vec3d xDir = (ca.front ^ ca.up).normalize();
	vec3d pixelOriginPos = ca.pos + ca.front * nearClipZ - xDir * tx - ca.up * ty;
	vec3d pixelXInc = xDir * tx / gc.width * 2;
	vec3d pixelYIinc = ca.up * ty / gc.height * 2;

	vec3d *c = new vec3d[gc.width * gc.height];

//#pragma omp parallel for num_threads(4)

	for (int i = 0; i < gc.height; i++) {
		printf("\rRendering (%d samples per pixel) %5.2f%%", gc.samples, 100.0f * i / (gc.height - 1));
#pragma omp parallel for schedule(dynamic)
		for (int j = 0; j < gc.width; j++) {
			vec3d color(0, 0, 0);
			for (int k = 0; k < gc.samples; k++) {
				double xRand = randRT();
				double yRand = randRT();
				vec3d p = pixelOriginPos + pixelXInc * (j + xRand) + pixelYIinc * (i + yRand);
				vec3d d = (p - ca.pos).normalize();
				ray r(p, d);
				vec3d c = pathTracing(r, 0, false);
				color = color + c;
			}
			color = color / gc.samples;
			c[(gc.height - i - 1) * gc.width + j] = color;
		}
	}

	time(&stop);
	double duration = difftime(stop, start);
	printf("End Tracing, total time %.2lf seconds...\n", duration);

	FILE *fp = fopen("result.ppm", "w");
	fprintf(fp, "P3\n%d %d\n%d\n", gc.width, gc.height, 255);
	for (int i = 0; i < gc.width * gc.height; i++)
		fprintf(fp, "%d %d %d ", setColor(c[i].x), setColor(c[i].y), setColor(c[i].z));
	fclose(fp);

	system("pause");
}