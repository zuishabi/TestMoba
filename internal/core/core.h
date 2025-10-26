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
#include <unordered_map>

class ComponentManager;

enum class ComponentType{
    MovingComponentType,
    AttackComponentType,
    AttributeComponentType,
    HitComponentType,
    SkillComponentType,
};


class Component {
public:
    virtual ~Component() = default;

    explicit Component(ComponentType type,std::shared_ptr<ComponentManager> mgr)
        :Type(type) ,Manager(std::move(mgr)){}
public:
    virtual void Update() {

    };
public:
    const ComponentType Type;
    bool Enable = true;
protected:
    std::shared_ptr<ComponentManager> Manager = nullptr;
};


// 组件管理器
class ComponentManager {
public:
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    ComponentManager() = delete;
    explicit ComponentManager(const std::unordered_map<int32_t,std::shared_ptr<ComponentManager>>& m) :
    ManagerMap(m){}

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
    const std::unordered_map<int32_t,std::shared_ptr<ComponentManager>>& ManagerMap;
    SyncerManager SyncerManager;
};


struct SkillInfo {
    b2Vec2 pos;
    b2Vec2 direction;
};


class Skill {
public:
    virtual ~Skill() = default;

    Skill(std::shared_ptr<ComponentManager> from):from(std::move(from)){}

    virtual void Execute(SkillInfo info) = 0;

    virtual void Update() = 0;

protected:
    std::shared_ptr<ComponentManager> from;

    [[nodiscard]] virtual bool CanExecute(int currentMana) const {
        if (!canExecute) {
            return false;
        }
        if (cost > currentMana) {
            return false;
        }
        return canExecute;
    }
private:
    uint32_t id;
    int cost;
    bool canExecute;
};


#endif //TESTSERVER_COMPONENT_H