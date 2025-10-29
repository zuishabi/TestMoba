//
// Created by 张俏悦 on 2025/10/17.
//

#ifndef TESTSERVER_TIMER_H
#define TESTSERVER_TIMER_H

#include <chrono>
#include <vector>

class TimerManager {
private:
    using Clock = std::chrono::steady_clock;

    struct Timer {
        Clock::time_point startTime;
        std::chrono::milliseconds duration;
        bool repeating;

        bool isExpired(Clock::time_point now) const {
            return now - startTime >= duration;
        }

        void reset(Clock::time_point now) {
            startTime = now;
        }
    };

    std::vector<Timer> timers;

public:
    Timer* addTimer(std::chrono::milliseconds duration,bool repeating = false) {
        timers.push_back({ Clock::now(), duration, repeating});
        return &timers.back();
    }

    void update() {
        auto now = Clock::now();
        auto it = timers.begin();

        while (it != timers.end()) {
            if (it->isExpired(now)) {

                if (it->repeating) {
                    it->reset(now);
                    ++it;
                } else {
                    it = timers.erase(it);
                }
            } else {
                ++it;
            }
        }
    }
};

#endif //TESTSERVER_TIMER_H