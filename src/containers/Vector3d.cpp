//
// Created by peter on 14.12.2017.
//

#include "Vector3d.h"

Vector3d::Vector3d(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector3d::pointUp() {
    if (z < 0.0) {
        x *= -1;
        y *= -1;
        z *= -1;
    }
}
