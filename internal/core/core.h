//
// Created by 张俏悦 on 2025/10/20.
//

#ifndef TESTSERVER_COMPONENT_H
#define TESTSERVER_COMPONENT_H

#include <memory>
#include <utility>
#include <vector>
#include <box2d/math_functions.h>
#include "../worldManager.h"
#include "../../protos/test.pb.h"

enum class State:uint32_t{
    IDLE = 0,
    MOVING = 1,
    ATTACK = 2,
    SKILL = 3,
    CC = 4,
    MOVE_SKILL = 5,
};


enum class ManagerType {
    Player,
    Skill,
};


// 执行技能的信息
struct ExecuteSkillInfo {
    b2Vec2 pos;
    float rotate;
};


// 技能的信息
struct SkillInfo {
    int pos;
    float rotate;
    float timeLeft;
};


struct SkillAttribute {
    int scale;
    int strength;
    int efficiency;
};


struct AttackInfo {
    int PhysicalDamage;
    int MagicDamage;
};


struct ObjectInfo {
    uint32_t type_id; // 实体类型的id
    uint64_t id; // 实体对应的id
    float x; // 实体坐标
    float y;
    float scale; // 实体缩放
    float angle; // 实体角度
};


class ComponentManager;
class SyncerManager;

enum class ComponentType{
    MovingComponentType,
    AttackComponentType,
    AttributeComponentType,
    HitComponentType,
    SkillComponentType,
    SkillObjectComponentType,
    BuffComponentType,
    StateMachineComponentType,
    MoveTargetComponentType,
    MoveDirectionComponentType,
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

    explicit ComponentManager(uint64_t id,ManagerType type):
    id(id),Type(type),SyncerManager(std::make_unique<class SyncerManager>()) {

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
    std::unique_ptr<SyncerManager> SyncerManager;
    uint64_t id;
    ManagerType Type;
    bool destroyed = false;
};


class Skill {
public:
    virtual ~Skill() = default;

    Skill(uint32_t id,uint64_t from):from(from),id(id){}

    virtual void Execute(ExecuteSkillInfo info) = 0;

    virtual void Update() = 0;

    virtual void Interrupt(){}

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
    bool canExecute; // 是否可以被执行
    uint32_t id; // 技能的id
    bool activated = false;
};


enum class SyncerType {
    HealthSyncer,
    MoveSyncer,
    ManaSyncer,
    SpeedSyncer,
    AttackSpeedSyncer,
    SkillInfoSyncer,
    StateSyncer,
    SkillAttributeSyncer,
    ObjectSyncer,
};


class Syncer{
public:
    virtual ~Syncer() = default;

    explicit Syncer(uint64_t id,SyncerType type) : id(id), type(type) {
    }

public:
    virtual std::shared_ptr<Packet> getSync() = 0;

    [[nodiscard]] SyncerType GetType() const {
        return type;
    }

    [[nodiscard]] bool IsUpdated() const{
        return updated;
    }
protected:
    uint64_t id;
    bool updated = false;
    SyncerType type;
};


class SyncerManager {
public:
    void AddSyncer(const std::shared_ptr<Syncer>& syncer) {
        syncers[syncer->GetType()] = syncer;
    }

    std::shared_ptr<Syncer> GetSyncer(SyncerType type) {
        auto it = syncers.find(type);
        if (it != syncers.end()) {
            return std::dynamic_pointer_cast<Syncer>(it->second);
        }
        return nullptr;
    }

    std::unordered_map<SyncerType,std::shared_ptr<Syncer>> GetSyncers() {
        return syncers;
    }
private:
    std::unordered_map<SyncerType, std::shared_ptr<Syncer>> syncers;
};


class Buff {
public:
    Buff(uint64_t from,uint64_t owner):from(from),owner(owner){}
public:
    virtual void Update() = 0;
    virtual void OnLoad() = 0;
public:
    bool destroyed = false;
    uint64_t from;
    uint64_t owner;
};


#endif //TESTSERVER_COMPONENT_H