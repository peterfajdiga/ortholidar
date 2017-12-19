//
// Created by peter on 6.12.2017.
//

#ifndef ORTHOLIDAR_POINT_H
#define ORTHOLIDAR_POINT_H


class Point3d {
public:
    double coords[3];
    double& x = coords[0];
    double& y = coords[1];
    double& z = coords[2];

    Point3d(double x, double y, double z);

    Point3d& operator=(const Point3d& src);

    double distSqTo(const Point3d& other) const;
    double distTo(const Point3d& other) const;



    // Comparators
    typedef bool (*comparator)(const Point3d*, const Point3d*);
    static bool compareX(const Point3d* a, const Point3d* b);
    static bool compareY(const Point3d* a, const Point3d* b);
    static bool compareZ(const Point3d* a, const Point3d* b);
    static const comparator comparators[3];
};


#endif //ORTHOLIDAR_POINT_H
