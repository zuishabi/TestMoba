//
// Created by 张俏悦 on 2025/10/22.
//

#ifndef TESTSERVER_COMPONENTS_H
#define TESTSERVER_COMPONENTS_H

#include "../core.h"
#include "../syncers/syncer.h"

// 移动组件
class MovingComponent :public Component{
public:
    MovingComponent(uint64_t id,int speed,b2Vec2 targetPos):
    Component(ComponentType::MovingComponentType,id),Speed(speed),moveSyncer(std::make_shared<MoveSyncer>(id)),target(targetPos){
        manager->SyncerManager->AddSyncer(moveSyncer);
        moveSyncer->SetPos({200,200});
        target = {200,200};
    }
public:
    void Update() override;

    void SetTargetDirection(b2Vec2 target);

    void Interrupt();

    void ProcessInput(b2Vec2 target);
public:
    int Speed;
    int OverrideSpeed = 0;
    bool CanMove = true;
    b2Vec2 target;
private:
    std::shared_ptr<MoveSyncer> moveSyncer;
};


class AttackComponent :public Component {
public:
    explicit AttackComponent(uint64_t id,float scale):Component(ComponentType::AttackComponentType,id),scale(scale) {

    }
public:
    void Update() override;

    void Attack( uint64_t targetID);

    void Interrupt();
public:
    bool isBullet = false;
    bool isShooting = false;
    float scale;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, 1000000000>>> nextPoint;
    uint64_t targetID;
};


class AttributeComponent:public Component {
public:
    AttributeComponent(uint64_t id):Component(ComponentType::AttributeComponentType,id),
    manaSyncer(std::make_shared<ManaSyncer>(id)),healthSyncer(std::make_shared<HealthSyncer>(id)),
    attackSpeedSyncer(std::make_shared<AttackSpeedSyncer>(id)) {
        manager->SyncerManager->AddSyncer(this->healthSyncer);
        manager->SyncerManager->AddSyncer(this->manaSyncer);
        manager->SyncerManager->AddSyncer(this->attackSpeedSyncer);
        healthSyncer->SetHealth(100);
        healthSyncer->SetMaxHealth(100);
        manaSyncer->SetMana(100);
        manaSyncer->SetMaxMana(100);
        attackSpeedSyncer->SetSpeed(1);
    }
public:
    void SetHealth(int health) {
        healthSyncer->SetHealth(health);
    }

    int GetHealth() const {
        return healthSyncer->GetHealth();
    }

    void SetMana(int mana) {
        manaSyncer->SetMana(mana);
    }

    int GetMana() const{
        return manaSyncer->GetMana();
    }

    void SetAttackSpeed(float speed) {
        attackSpeedSyncer->SetSpeed(speed);
    }

    float GetAttackSpeed() const {
        return attackSpeedSyncer->GetSpeed();
    }

    void Update() override;
private:
    std::shared_ptr<ManaSyncer> manaSyncer;
    std::shared_ptr<HealthSyncer> healthSyncer;
    std::shared_ptr<AttackSpeedSyncer> attackSpeedSyncer;
};


class HitComponent:public Component{
public:
    HitComponent(uint64_t id):Component(ComponentType::HitComponentType,id) {}
public:
    void Hit(uint64_t target,AttackInfo info);
};


class SkillComponent:public Component {
public:
    explicit SkillComponent(uint64_t id,std::array<std::unique_ptr<Skill>,4> skills):
    Component(ComponentType::SkillComponentType,id),skills(std::move(skills)) {
    }
    void Update() override;
public:
    void ExecuteSkill(int pos,SkillInfo info);
private:
    std::array<std::unique_ptr<Skill>,4> skills;
};


class SkillObjectComponent:public Component {
public:
    SkillObjectComponent(uint64_t id,uint64_t from):Component(ComponentType::SkillObjectComponentType,id),from(from){}
protected:
    uint64_t from;
};


class BuffComponent:public Component {
public:
    BuffComponent(uint64_t id):Component(ComponentType::BuffComponentType,id) {}
public:
    void Update() override;

    void AddBuff(std::unique_ptr<Buff> buff);
private:
    std::vector<std::unique_ptr<Buff>> buffs;
};


class StateSyncer:public Syncer {
public:
    explicit StateSyncer(uint64_t id):Syncer(id,SyncerType::StateSyncer){}
public:
    std::shared_ptr<Packet>getSync()override {
        updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto state = packet->mutable_state_sync();
        state->set_id(id);
        state->set_state(static_cast<uint32_t>(currentState));
        return packet;
    }

    void SetState(State state) {
        updated = true;
        currentState = state;
    }

    [[nodiscard]] State GetState() const {
        return currentState;
    }

    State currentState;
};


// 用于进行状态同步
class StateMachineComponent:public Component {
public:
    explicit StateMachineComponent(uint64_t id):
    Component(ComponentType::StateMachineComponentType,id),currentState(std::make_shared<StateSyncer>(id)){}
public:
    std::shared_ptr<StateSyncer> currentState;
};

#endif //TESTSERVER_COMPONENTS_H