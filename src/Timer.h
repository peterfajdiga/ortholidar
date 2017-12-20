//
// Created by peter on 13.12.2017.
//

#ifndef ORTHOLIDAR_TIMER_H
#define ORTHOLIDAR_TIMER_H

#include <chrono>


class Timer {
private:
    std::chrono::microseconds startTime;

public:
    Timer();
    void restart();
    void log(const char* message) const;
    void log(const char* message, size_t n) const;
    std::chrono::microseconds getDelta() const;

    static std::chrono::microseconds getTime();
};


#endif //ORTHOLIDAR_TIMER_H
