#pragma once

#include <cstdio>
#include <vector>
#include <string>
#include <map>

#include "vec3d.hpp"
#include "object.hpp"

struct tripleArrayInt {
    int a, b, c;
    int &operator[] (const int i) {
        switch (i) {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            default: {
                printf("Error, index out of range!\n");
                return a;
            }
        }
    }
};

struct tripleArrayDouble {
	double a, b, c;
	double &operator[] (const int i) {
		switch (i) {
			case 0: return a;
			case 1: return b;
			case 2: return c;
			default: {
				printf("Error, index out of range!\n");
				return a;
			}
		}
	}
};



class objReader {
public:
    int numOfVertex;
	int numofObject;
	int numofMaterial;
    std::vector<vec3d> vertexList;
	std::map<std::string, material *> materialList;
	std::vector<rootObject *> objectList;
    
    
    objReader();
    ~objReader();

    bool readObjFile(const char *filename);
	bool readMtlFile(const char *filename);
};
