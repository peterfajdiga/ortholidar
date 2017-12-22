//
// Created by peter on 19.12.2017.
//

#ifndef ORTHOLIDAR_LAS_H
#define ORTHOLIDAR_LAS_H


#include <vector>
#include "../containers/Point3d.h"
#include "../containers/Vector3d.h"
#include "../containers/Color.h"

class Las {

private:
    class Entry {
    public:
        bool included = true;  // TODO: false
        const Point3d point;
        const Vector3d* normal = nullptr;
        const Color* color = nullptr;

        Entry(double x, double y, double z);
        ~Entry();
    };


private:
    const char* inputFilename;
    std::vector<Entry> entries;
    double p_minX, p_minY, p_maxX, p_maxY;

    void readPoints();

public:
    const double& minX = p_minX;
    const double& minY = p_minY;
    const double& maxX = p_maxX;
    const double& maxY = p_maxY;

    Las(const char* inputFilename);
    Las(const Las& src);

    size_t size() const;
    const Point3d& operator[](size_t i) const;
    std::vector<const Point3d*> getPoints() const;  // use sparingly, prefer operator[]
    bool isIncluded(size_t i) const;
    void setNormal(size_t i, const Vector3d* normal);
    void setColor(size_t i, const Color* color);

    // assumes length of included == original length of points
    // assumes length of normals == length of included points
    void save(const char *outputFilename) const;
};


#endif //ORTHOLIDAR_LAS_H
