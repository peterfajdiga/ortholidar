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
    std::vector<Point3d> points;

    void readPoints();

public:
    Las(const char* inputFilename);
    const std::vector<Point3d>& getPoints() const;

    // assumes length of included == original length of points
    // assumes length of normals == length of included points
    void savePoints(const char* outputFilename,
                    const bool* included,
                    const std::vector<Vector3d>& normals) const;
};


#endif //ORTHOLIDAR_LAS_H
