//
// Created by 张俏悦 on 2025/10/14.
//

#ifndef TESTSERVER_CONTAINERS_H
#define TESTSERVER_CONTAINERS_H

template<typename T>
struct Vector2 {
    T x,y;
};


template<typename T>
struct SaveQueue {
public:
    SaveQueue() = default;
    SaveQueue(const SaveQueue<T>&) = delete; // 当编译器遇到delete时则判定为非法，因此不能使用拷贝构造函数，因为有锁。
    virtual ~SaveQueue() {
        clear(); // 这里可以避免在销毁时并发访问。
    }

    const T& front() { // 返回值不能被修改
        std::scoped_lock lock(muxQueue); // 在这个作用域内使用这把锁，并且c++17后可以根据传入的类型自动识别模板类型
        return deqQueue.front();
    }

    const T& back() {
        std::scoped_lock lock(muxQueue);
        return deqQueue.back();
    }

    void push_back(const T& item) {
        std::scoped_lock lock(muxQueue);
        deqQueue.emplace_back(std::move(item));
    }

    void push_front(const T& item) {
        std::scoped_lock lock (muxQueue);
        deqQueue.emplace_front(std::move(item));
    }

    bool empty() {
        std::scoped_lock lock(muxQueue);
        return deqQueue.empty();
    }

    size_t count() {
        std::scoped_lock lock(muxQueue);
        return deqQueue.size();
    }

    void clear() {
        std::scoped_lock lock(muxQueue);
        deqQueue.clear();
    }

    // Removes and returns item from front of Queue
    T pop_front()
    {
        std::scoped_lock lock(muxQueue);
        auto t = std::move(deqQueue.front());
        deqQueue.pop_front();
        return t;
    }

    T pop_back() {
        std::scoped_lock lock(muxQueue);
        auto t = std::move(deqQueue.back());
        deqQueue.pop_back();
        return t;
    }
protected:
    std::mutex muxQueue;
    std::deque<T> deqQueue;
};


#endif //TESTSERVER_CONTAINERS_H