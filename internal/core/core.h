//
// Created by 张俏悦 on 2025/10/20.
//

#ifndef TESTSERVER_COMPONENT_H
#define TESTSERVER_COMPONENT_H

#include <memory>
#include <utility>
#include <vector>
#include <box2d/box2d.h>
#include <box2d/id.h>
#include <box2d/math_functions.h>
#include <chrono>
#include "../utils/syncer.h"

class ComponentManager;

enum class ComponentType{
    MovingComponentType,
    AttackComponentType,
    AttributeComponentType,
    HitComponentType,
};


class Component {
public:
    virtual ~Component() = default;

    explicit Component(ComponentType type,ComponentManager* mgr)
        :Type(type) ,Manager(mgr){}
public:
    virtual void Update() {

    };
public:
    const ComponentType Type;
    bool Enable = true;
protected:
    ComponentManager* Manager = nullptr;
};


// 组件管理器
class ComponentManager {
public:
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    ComponentManager() = default;

    template<typename T>
    T* GetComponent(ComponentType type) {
        for (auto &ptr : componentList) {
            if (ptr->Type == type) {
                return dynamic_cast<T*>(ptr.get());
            }
        }
        return nullptr;
    }

    template<typename T,typename... Args>
    T* AddComponent(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = ptr.get();
        componentList.emplace_back(std::move(ptr));
        return rawPtr;
    }

    void Update() {
        for (auto& c : componentList) {
            if (c->Enable) {
                c->Update();
            }
        }
    }
private:
    std::vector<std::unique_ptr<Component>> componentList;
public:
    SyncerManager SyncerManager;
};

#endif //TESTSERVER_COMPONENT_H