//
// Created by peter on 11.1.2018.
//

#ifndef ORTHOLIDAR_NORMALS_H
#define ORTHOLIDAR_NORMALS_H


#include <Eigen/Dense>
#include "../io/Las.h"
#include "../Timer.h"
#include "KDTree.h"



namespace normals {
    static const double NORMAL_RADIUS = 2.0;
    static const double NOISE_THRESHOLD = 200.0;

    // free after use
    Vector3d* calculateNormal(const KDTree* tree, const Point3d& p);

    void calculateNormals(Las& las);
};


#endif //ORTHOLIDAR_NORMALS_H
