//
// Created by 张俏悦 on 2025/10/22.
//

#ifndef TESTSERVER_COMPONENTS_H
#define TESTSERVER_COMPONENTS_H

#include "../core.h"
#include "../syncers/syncer.h"
#include "../../utils/event.h"

// 移动组件
class MovingComponent :public Component{
public:
    explicit MovingComponent(uint64_t id,b2Vec2 startPos):Component(ComponentType::MovingComponentType,id),
    speed(std::make_shared<SpeedSyncer>(id)),moveSyncer(std::make_shared<MoveSyncer>(id)){
        manager->SyncerManager->AddSyncer(moveSyncer);
        moveSyncer->SetPos(startPos);
        manager->SyncerManager->AddSyncer(speed);
        speed->SetSpeed(100);
        target = startPos;
    }
public:
    void Update() override;

    void SetTargetDirection(b2Vec2 target);

    void ProcessInput(b2Vec2 target);
public:
    int OverrideSpeed = 0;
    bool CanMove = true; // 是否可以进行主动移动
    b2Vec2 target; // 移动的目标位置
    Signal<b2Vec2> MovingSignal;
private:
    std::shared_ptr<MoveSyncer> moveSyncer;
    std::shared_ptr<SpeedSyncer> speed;
};


class AttackComponent :public Component {
public:
    explicit AttackComponent(uint64_t id,float scale):Component(ComponentType::AttackComponentType,id),scale(scale) {

    }
public:
    void Update() override;

    void Attack( uint64_t targetID);

public:
    bool isBullet = false;
    bool isShooting = false;
    float scale;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, 1000000000>>> nextPoint;
    uint64_t targetID;
    Signal<uint64_t> startAttackSignal;
    Signal<uint64_t> stopAttackSignal;
};


class AttributeComponent:public Component {
public:
    AttributeComponent(uint64_t id):Component(ComponentType::AttributeComponentType,id),
    manaSyncer(std::make_shared<ManaSyncer>(id)),healthSyncer(std::make_shared<HealthSyncer>(id)),
    attackSpeedSyncer(std::make_shared<AttackSpeedSyncer>(id)){

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
public:
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
    explicit SkillComponent(uint64_t id):
    Component(ComponentType::SkillComponentType,id),skillAttributeSyncer(std::make_shared<SkillAttributeSyncer>(id)){
        manager->SyncerManager->AddSyncer(this->skillAttributeSyncer);
        skillAttributeSyncer->SetAttribute({100,100,100});
    }

    void Update() override;
public:

    void LoadSkills(std::array<std::unique_ptr<Skill>,4> skills) {
        this->skills = std::move(skills);
    }

    void ExecuteSkill(int pos,ExecuteSkillInfo info);

    std::shared_ptr<SkillAttributeSyncer> skillAttributeSyncer;
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


class StateMachineComponent;


class StateNode {
public:
    StateNode(StateMachineComponent* stateMachine):stateMachine(stateMachine){}

    virtual void OnEnter() = 0;

    virtual void Update() = 0;

    virtual void OnExit() = 0;

    State GetState() {
        return state;
    }

    bool activate;
protected:
    State state;
    StateMachineComponent* stateMachine;
};


// 用于进行同步状态
class StateMachineComponent:public Component {
public:
    explicit StateMachineComponent(uint64_t id):
    Component(ComponentType::StateMachineComponentType,id),currentState(std::make_shared<StateSyncer>(id)) {
        std::cout << "create state machine:" << id << std::endl;
        manager->SyncerManager->AddSyncer(currentState);
        InitializeNodes();
    }
public:
    void Update() override;

    void InitializeNodes();

    void SetStateNode(State state) {
        if (currentNode != nullptr) {
            if (state == currentNode->GetState()) {
                return;
            }
            currentNode->OnExit();
        }
        currentNode = nodes[static_cast<uint32_t>(state)].get();
        currentNode->OnEnter();
        currentState->SetState(state);
    }

private:
    std::shared_ptr<StateSyncer> currentState;
    std::vector<std::unique_ptr<StateNode>> nodes;
    StateNode* currentNode;
};



#endif //TESTSERVER_COMPONENTS_H