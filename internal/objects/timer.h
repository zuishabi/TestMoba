//
// Created by 张俏悦 on 2025/10/17.
//

#ifndef TESTSERVER_TIMER_H
#define TESTSERVER_TIMER_H

#include <chrono>
#include <utility>
#include <vector>
#include <functional>
#include <algorithm>

class TimerManager {
private:
    using Clock = std::chrono::steady_clock;

    struct Timer {
        int id;
        Clock::time_point startTime;
        std::chrono::milliseconds duration;
        std::function<void()> callback;
        bool repeating;

        bool isExpired(Clock::time_point now) const {
            return now - startTime >= duration;
        }

        void reset(Clock::time_point now) {
            startTime = now;
        }
    };

    std::vector<Timer> timers;
    int nextId = 0;

public:
    int addTimer(std::chrono::milliseconds duration,std::function<void()> callback,bool repeating = false) {
        timers.push_back({nextId++, Clock::now(), duration, std::move(callback), repeating});
        return timers.back().id;
    }

    void update() {
        auto now = Clock::now();
        auto it = timers.begin();

        while (it != timers.end()) {
            if (it->isExpired(now)) {
                it->callback();

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