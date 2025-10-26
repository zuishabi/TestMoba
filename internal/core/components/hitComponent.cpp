//
// Created by 张俏悦 on 2025/10/24.
//

#include "components.h"

void HitComponent::Hit(std::shared_ptr<ComponentManager> target) {
    auto selfAttribute = Manager->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
    auto targetAttribute = target->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
    targetAttribute->SetHealth(targetAttribute->GetHealth() - 10);
}
