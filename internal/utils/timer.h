//
// Created by 张俏悦 on 2025/10/17.
//

#ifndef TESTSERVER_TIMER_H
#define TESTSERVER_TIMER_H

#include <chrono>

struct Timer {
    using Clock = std::chrono::steady_clock;
    std::chrono::steady_clock::time_point startTime = Clock::now();
    std::chrono::duration<double> duration = std::chrono::seconds(0);
    bool start = false;

    bool IsExpired() {
        if (!start) {
            return false;
        }
        if (Clock::now() - startTime >= duration) {
            start = false;
            return true;
        }
        return false;
    }

    void Reset(std::chrono::duration<double> duration) {
        start = true;
        this->duration = duration;
        startTime = Clock::now();
    }

    void Stop() {
        start = false;
    }
};
#endif //TESTSERVER_TIMER_H