//
// Created by 张俏悦 on 2025/11/4.
//

#ifndef TESTMOBA_EVENT_H
#define TESTMOBA_EVENT_H

#include <functional>
#include <vector>

template <typename... Args>
class Signal {
public:
    using slot_t = std::function<void(Args...)>;
    using id_t = std::size_t;

    Signal() = default;
    virtual ~Signal() = default;

    template <typename F>
    id_t connect(F&& f) {
        slot_t s = std::forward<F>(f);
        id_t id = ++next_id_;
        slots_.emplace_back(id, std::move(s));
        return id;
    }

    // 根据 id 断开
    bool disconnect(id_t id) {
        for (auto it = slots_.begin(); it != slots_.end(); ++it) {
            if (it->first == id) {
                slots_.erase(it);
                return true;
            }
        }
        return false;
    }

    void disconnect_all() {
        slots_.clear();
    }

    // 使用快照复制当前槽，避免回调中修改 slots_ 导致迭代器失效
    void emit(Args... args) {
        auto snapshot = slots_;
        for (auto &p : snapshot) {
            if (p.second) p.second(args...);
        }
    }

private:
    std::vector<std::pair<id_t, slot_t>> slots_;
    id_t next_id_ = 0;
};

#endif //TESTMOBA_EVENT_H