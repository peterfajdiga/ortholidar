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

void Timer::log(const char* message) {
    fprintf(stderr, "%s in %d us\n", message, getDelta());
}

void Timer::log(const char *message, size_t const n) {
    fprintf(stderr, "%s in %f us (%lu samples)\n", message, getDelta() / (double)n, n);
}

microseconds Timer::getDelta() {
    return duration_cast<microseconds>(
            getTime() - startTime
    );
}

microseconds Timer::getTime() {
    return duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
    );
}
