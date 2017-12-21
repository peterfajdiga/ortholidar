//
// Created by peter on 19.12.2017.
//

#include <lasreader.hpp>
#include <laswriter.hpp>
#include "Las.h"

Las::Las(const char* inputFilename) {
    this->inputFilename = inputFilename;
    readPoints();
}

void Las::readPoints() {
    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);

    double const x_offset = reader->header.x_offset;
    double const y_offset = reader->header.y_offset;
    double const z_offset = reader->header.z_offset;
    double const x_scale  = reader->header.x_scale_factor;
    double const y_scale  = reader->header.y_scale_factor;
    double const z_scale  = reader->header.z_scale_factor;

    entries.clear();
    while (reader->read_point()) {
        const LASpoint& p = reader->point;
        entries.push_back(Entry(
                p.get_X() * x_scale + x_offset,
                p.get_Y() * y_scale + y_offset,
                p.get_Z() * z_scale + z_offset
        ));
    }

    reader->close();
    delete reader;
}


size_t Las::size() const {
    return entries.size();
}

std::vector<const Point3d*> Las::getPoints() const {
    size_t n = entries.size();
    std::vector<const Point3d*> points(n);
    for (size_t i = 0; i < n; i++) {
        points[i] = &entries[i].point;
    }
    return points;
}

void Las::setNormal(size_t i, const Vector3d* normal) {
    Entry& entry = entries[i];
    entry.included = true;
    entry.normal = normal;
}


void Las::save(const char* const outputFilename) const {
    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);
    LASwriteOpener writerOpener;
    writerOpener.set_file_name(outputFilename);
    LASwriter* const writer = writerOpener.open(&reader->header);

    for (size_t i = 0; reader->read_point(); i++) {
        const Entry& entry = entries[i];
        if (entry.included) {
            // TODO: add normal to point
            writer->write_point(&reader->point);
        }
    }

    reader->close();
    delete reader;
    writer->close();
    delete writer;
}


Las::Entry::Entry(double const x, double const y, double const z) : point(x, y, z) {}

Las::Entry::~Entry() {
    // TODO: use shared pointer
    delete normal;
}
