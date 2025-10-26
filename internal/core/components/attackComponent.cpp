//
// Created by 张俏悦 on 2025/10/20.
//

#include <iostream>
#include "components.h"


void AttackComponent::Update() {
    if (isShooting && std::chrono::steady_clock::now() >= nextPoint) {
        auto hit = target->GetComponent<HitComponent>(ComponentType::HitComponentType);
        if (hit != nullptr) {
            hit->Hit(target);
        }
        isShooting = false;
    }
}


void AttackComponent::Attack(const std::shared_ptr<ComponentManager>& targetComponent) {
    if (Enable && targetComponent != nullptr) {
        target = targetComponent;
        auto attackSpeed = Manager->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
        nextPoint = std::chrono::steady_clock::now() + std::chrono::duration<double>(attackSpeed->GetAttackSpeed());
        isShooting = true;
        auto moving = Manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
        moving->Interrupt();
    }
}


void AttackComponent::Interrupt() {
    isShooting = false;
}

