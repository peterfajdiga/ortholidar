//
// Created by peter on 19.12.2017.
//

#ifndef ORTHOLIDAR_LAS_H
#define ORTHOLIDAR_LAS_H


#include <vector>
#include "../containers/Point3d.h"
#include "../containers/Vector3d.h"

class Las {
private:
    const char* inputFilename;

public:
    Las(const char* inputFilename);
    void readPoints(std::vector<Point3d>& outPoints) const;
    void savePoints(const char* outputFilename, const Vector3d* const normals[]) const;
};


#endif //ORTHOLIDAR_LAS_H
