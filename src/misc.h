//
// Created by peter on 25.12.2017.
//

#ifndef ORTHOLIDAR_MISC_H
#define ORTHOLIDAR_MISC_H

namespace misc {
    double frac(double v);

    template<typename T>
    T lerp(T const min, T const max, double const v) {
        return min + (max-min) * v;
    }

    template<class Pixel>
    Pixel lerpPixel(const Pixel& min, const Pixel& max, double const v) {
        Pixel p;
        p.red = lerp(min.red, max.red, v);
        p.green = lerp(min.green, max.green, v);
        p.blue = lerp(min.blue, max.blue, v);
        return p;
    }
}

#endif //ORTHOLIDAR_MISC_H
