//
// Created by 张俏悦 on 2025/10/17.
//

#ifndef TESTSERVER_TIMER_H
#define TESTSERVER_TIMER_H

#include <chrono>

struct Timer {
    using Clock = std::chrono::steady_clock;
    std::chrono::steady_clock::time_point startTime = Clock::now();
    std::chrono::steady_clock::duration duration = std::chrono::seconds(0);
    bool repeating;

    bool IsExpired() const {
        return Clock::now() - startTime >= duration;
    }

    void Reset(Clock::duration duration) {
        this->duration = duration;
        startTime = Clock::now();
    }
};
#endif //TESTSERVER_TIMER_H