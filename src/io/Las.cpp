//
// Created by peter on 19.12.2017.
//

#include <lasreader.hpp>
#include <laswriter.hpp>
#include "Las.h"

Las::Las(const char* inputFilename) {
    this->inputFilename = inputFilename;
}

void Las::readPoints(std::vector<Point3d>& outPoints) const {
    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);

    double const x_offset = reader->header.x_offset;
    double const y_offset = reader->header.y_offset;
    double const z_offset = reader->header.z_offset;
    double const x_scale  = reader->header.x_scale_factor;
    double const y_scale  = reader->header.y_scale_factor;
    double const z_scale  = reader->header.z_scale_factor;

    while (reader->read_point()) {
        const LASpoint& p = reader->point;
        outPoints.push_back(Point3d(
                p.get_X() * x_scale + x_offset,
                p.get_Y() * y_scale + y_offset,
                p.get_Z() * z_scale + z_offset
        ));
    }

    reader->close();
    delete reader;
}

void Las::savePoints(const char* const outputFilename, const Vector3d* const normals[]) const {
    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);
    LASwriteOpener writerOpener;
    writerOpener.set_file_name(outputFilename);
    LASwriter* const writer = writerOpener.open(&reader->header);

    for (int i = 0; reader->read_point(); i++) {
        const Vector3d* const normal = normals[i];
        if (normal == nullptr) continue;
        // TODO: add normal to point
        writer->write_point(&reader->point);
    }

    reader->close();
    delete reader;
    writer->close();
    delete writer;
}
