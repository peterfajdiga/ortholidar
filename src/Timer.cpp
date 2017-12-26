//
// Created by peter on 13.12.2017.
//

#include <cstdio>
#include "Timer.h"

using namespace std::chrono;

Timer::Timer() {
    restart();
}

void Timer::restart() {
    startTime = getTime();
}

void Timer::log(const char* message) const {
    fprintf(stderr, "%s in %lf s\n", message, getDelta() / 1e6);
}

void Timer::log(const char *message, size_t const n) const {
    fprintf(stderr, "%s in %lf s (%lu samples)\n", message, getDelta() / 1e6 / n, n);
}

microseconds Timer::getDelta() const {
    return duration_cast<microseconds>(
            getTime() - startTime
    );
}

microseconds Timer::getTime() {
    return duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
    );
}
