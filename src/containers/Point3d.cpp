//
// Created by peter on 6.12.2017.
//

#include <cmath>
#include "Point3d.h"

Point3d::Point3d(double const x, double const y, double const z) : coords{x, y, z} {}

// maintain x y z references
Point3d::Point3d(const Point3d& src) : coords{src.x, src.y, src.z}, x(coords[0]), y(coords[1]), z(coords[2]) {}

Point3d& Point3d::operator=(const Point3d& src) {
    x = src.x;
    y = src.y;
    z = src.z;
}

double Point3d::distSqTo(const Point3d& other) const {
    double const dx = other.x - x;
    double const dy = other.y - y;
    double const dz = other.z - z;
    return dx*dx + dy*dy + dz*dz;
}

double Point3d::distTo(const Point3d& other) const {
    return sqrt(distSqTo(other));
}



// Comparators

bool Point3d::compareX(const Point3d* const a, const Point3d* const b) {
    return a->x < b->x;
}

bool Point3d::compareY(const Point3d* const a, const Point3d* const b) {
    return a->y < b->y;
}

bool Point3d::compareZ(const Point3d* const a, const Point3d* const b) {
    return a->z < b->z;
}

const Point3d::comparator Point3d::comparators[3] = {&Point3d::compareX, &Point3d::compareY, &Point3d::compareZ};
