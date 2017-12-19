//
// Created by peter on 14.12.2017.
//

#ifndef ORTHOLIDAR_VECTOR3D_H
#define ORTHOLIDAR_VECTOR3D_H


class Vector3d {
public:
    double x, y, z;

    Vector3d(double x, double y, double z);

    void pointUp();
};


#endif //ORTHOLIDAR_VECTOR3D_H
