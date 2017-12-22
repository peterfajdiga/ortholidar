//
// Created by peter on 21.12.2017.
//

#ifndef ORTHOLIDAR_COLOR_H
#define ORTHOLIDAR_COLOR_H


class Color {
public:
    unsigned short r, g, b;

    inline Color(unsigned short const r, unsigned short const g, unsigned short const b) : r(r), g(g), b(b) {}
};


#endif //ORTHOLIDAR_COLOR_H
