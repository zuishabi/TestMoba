//
// Created by 张俏悦 on 2025/10/20.
//


#include <box2d/box2d.h>
#include <box2d/id.h>
#include "components.h"
#include "../../objects/worldManager.h"


void AttackComponent::Update() {
    if (isShooting && std::chrono::steady_clock::now() >= nextPoint) {
        ComponentManager* target = GameWorld::GetComponentManager(targetID);
        auto hit = target->GetComponent<HitComponent>(ComponentType::HitComponentType);
        if (hit != nullptr) {
            hit->Hit(targetID,{10});
        }
        isShooting = false;
    }
}


void AttackComponent::Attack(uint64_t target) {
    b2BodyId targetBody = b2LoadBodyId(target);
    if (Enable && b2Body_IsValid(targetBody)) {
        targetID = target;
        ComponentManager* manager = GameWorld::GetComponentManager(targetID);
        auto attackSpeed = manager->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
        nextPoint = std::chrono::steady_clock::now() + std::chrono::duration<double>(attackSpeed->GetAttackSpeed());
        isShooting = true;
        ComponentManager* self = GameWorld::GetComponentManager(id);
        auto moving = self->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
        moving->Interrupt();
    }
}


void AttackComponent::Interrupt() {
    isShooting = false;
}

