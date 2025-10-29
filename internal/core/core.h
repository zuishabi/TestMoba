//
// Created by 张俏悦 on 2025/10/20.
//

#ifndef TESTSERVER_COMPONENT_H
#define TESTSERVER_COMPONENT_H

#include <memory>
#include <utility>
#include <vector>
#include <box2d/math_functions.h>
#include "../utils/syncer.h"
#include "../objects/worldManager.h"

class ComponentManager;

enum class ComponentType{
    MovingComponentType,
    AttackComponentType,
    AttributeComponentType,
    HitComponentType,
    SkillComponentType,
    SkillObjectComponentType,
    BuffComponentType,
};


enum class ManagerType {
    Player,
    Skill,
};


class Component {
public:
    virtual ~Component() = default;

    explicit Component(ComponentType type,uint64_t id)
        :Type(type),id(id),manager(GameWorld::GetComponentManager(id)) {}
public:
    virtual void Update() {

    };
public:
    const ComponentType Type;
    bool Enable = true;
    uint64_t id;
    ComponentManager* manager;
};


// 组件管理器
class ComponentManager {
public:
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    ComponentManager() = delete;

    explicit ComponentManager(uint64_t id,ManagerType type):id(id),Type(type) {

    }

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
    uint64_t id;
    ManagerType Type;
    bool destroyed = false;
};


struct SkillInfo {
    b2Vec2 pos;
    float rotate;
};


class Skill {
public:
    virtual ~Skill() = default;

    Skill(uint64_t id):from(id){}

    virtual void Execute(SkillInfo info) = 0;

    virtual void Update() = 0;

protected:

    [[nodiscard]] virtual bool CanExecute(int currentMana) const {
        if (!canExecute) {
            return false;
        }
        if (cost > currentMana) {
            return false;
        }
        return canExecute;
    }
protected:
    uint64_t from;
    int cost;
    bool canExecute;
};


class Buff {
public:
    virtual void Update() = 0;
    bool destroyed = false;
    uint64_t id;
};


struct AttackInfo {
    int PhysicalDamage;
    int MagicDamage;
};

#endif //TESTSERVER_COMPONENT_H