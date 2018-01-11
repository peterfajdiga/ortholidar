//
// Created by peter on 21.12.2017.
//

#ifndef ORTHOLIDAR_COLOR_H
#define ORTHOLIDAR_COLOR_H


class Color {
public:
    unsigned short red, green, blue;

    Color() = default;

    inline Color(unsigned short const r, unsigned short const g, unsigned short const b) : red(r), green(g), blue(b) {}

    template<class Pixel>
    inline Color(const Pixel& p) : red(p.red), green(p.green), blue(p.blue) {}
};


#endif //ORTHOLIDAR_COLOR_H
