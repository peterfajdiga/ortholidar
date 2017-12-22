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

Las::Las(const Las &src) :
        inputFilename(src.inputFilename),
        entries(src.entries),
        p_minX(src.p_minX), p_minY(src.p_minY), p_maxX(src.p_maxX), p_maxY(src.p_maxY),
          minX(    p_minX),   minY(    p_minY),   maxX(    p_maxX),   maxY(    p_maxY) {}

void Las::readPoints() {
    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);
    const LASheader& header = reader->header;

    double const x_offset = header.x_offset;
    double const y_offset = header.y_offset;
    double const z_offset = header.z_offset;
    double const x_scale  = header.x_scale_factor;
    double const y_scale  = header.y_scale_factor;
    double const z_scale  = header.z_scale_factor;

    p_minX = header.min_x;
    p_minY = header.min_y;
    p_maxX = header.max_x;
    p_maxY = header.max_y;

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

const Point3d& Las::operator[](size_t const i) const {
    return entries[i].point;
}

bool Las::isIncluded(size_t const i) const {
    return entries[i].included;
}

void Las::setNormal(size_t const i, const Vector3d* normal) {
    Entry& entry = entries[i];
    entry.included = true;
    entry.normal = normal;
}

void Las::setColor(size_t const i, const Color* color) {
    entries[i].color = color;
}


void Las::save(const char* const outputFilename) const {
    // following example https://github.com/LAStools/LAStools/blob/master/LASlib/example/lasexample_add_rgb.cpp

    LASreadOpener readerOpener;
    LASreader* const reader = readerOpener.open(inputFilename);

    LASheader outHeader = reader->header;
    outHeader.unlink();
    switch (outHeader.point_data_format) {
        case 0: {
            outHeader.point_data_format = 2;
            outHeader.point_data_record_length += 6;
            break;
        }
        case 1: {
            outHeader.point_data_format = 3;
            outHeader.point_data_record_length += 6;
            break;
        }
        case 4: {
            outHeader.point_data_format = 5;
            outHeader.point_data_record_length += 6;
            break;
        }
        case 2:
        case 3:
        case 5: break;  // already the correct format
        default: {
            throw 124;  // TODO
        }
    }

    LASpoint point;
    if (outHeader.laszip) {
        LASzip* newLaszip = new LASzip;
        newLaszip->setup(outHeader.point_data_format, outHeader.point_data_record_length);
        point.init(&outHeader, newLaszip->num_items, newLaszip->items, &outHeader);
        delete outHeader.laszip;
        outHeader.laszip = newLaszip;
    } else {
        point.init(&outHeader, outHeader.point_data_format, outHeader.point_data_record_length, &outHeader);
    }

    LASwriteOpener writerOpener;
    writerOpener.set_file_name(outputFilename);
    LASwriter* const writer = writerOpener.open(&outHeader);

    for (size_t i = 0; reader->read_point(); i++) {
        const Entry& entry = entries[i];
        if (entry.included) {
            point = reader->point;
            point.rgb[0] = entry.color->r;
            point.rgb[1] = entry.color->g;
            point.rgb[2] = entry.color->b;
            // TODO: add normal to laspoint
            writer->write_point(&point);
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
    delete color;
}
