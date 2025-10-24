//
// Created by 张俏悦 on 2025/10/22.
//

#ifndef TESTSERVER_COMPONENTS_H
#define TESTSERVER_COMPONENTS_H

#include "../core.h"

// 移动组件
class MovingComponent :public Component{
public:
    MovingComponent(int speed,b2BodyId bodyID,ComponentManager* mgr,SyncerManager* syncerManager,std::shared_ptr<MoveSyncer> moveSyncer):
    Component(ComponentType::MovingComponentType,mgr),Speed(speed),BodyID(bodyID),moveSyncer(moveSyncer){
        syncerManager->AddSyncer(moveSyncer);
        moveSyncer->SetPos({200,200});
        target = {200,200};
    }
public:
    void Update() override;

    void SetTargetDirection(b2Vec2 target);

    void Interrupt();
public:
    int Speed;
    b2BodyId BodyID;
    bool CanMove = true;
    b2Vec2 target;
private:
    std::shared_ptr<MoveSyncer> moveSyncer;
};


class AttackComponent :public Component {
public:
    explicit AttackComponent(ComponentManager* mgr)
    :Component(ComponentType::AttackComponentType,mgr) {

    }
public:
    void Update() override;

    void Attack(ComponentManager* targetComponent);

    void Interrupt();
public:
    bool isBullet = false;
    bool isShooting = false;
    float scale = 100;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, 1000000000>>> nextPoint;
    ComponentManager* target = nullptr;
};


class AttributeComponent:public Component {
public:
    AttributeComponent(ComponentManager* mgr,SyncerManager* syncerManager,std::shared_ptr<HealthSyncer> healthSyncer,
        std::shared_ptr<ManaSyncer> manaSyncer,std::shared_ptr<AttackSpeedSyncer> attackSpeedSyncer):
    Component(ComponentType::AttributeComponentType,mgr),manaSyncer(manaSyncer),healthSyncer(healthSyncer),attackSpeedSyncer(attackSpeedSyncer) {
        syncerManager->AddSyncer(this->healthSyncer);
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
    HitComponent(ComponentManager* mgr):Component(ComponentType::HitComponentType,mgr) {}
public:
    void Hit(ComponentManager* target);
};

#endif //TESTSERVER_COMPONENTS_H