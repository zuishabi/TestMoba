//
// Created by 张俏悦 on 2025/10/24.
//

#include "components.h"
#include "../../objects/worldManager.h"

void HitComponent::Hit(uint64_t targetID,AttackInfo info) {
    ComponentManager* target = GameWorld::GetComponentManager(targetID);
    auto targetAttribute = target->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
    targetAttribute->SetHealth(targetAttribute->GetHealth() - info.PhysicalDamage - info.MagicDamage);
}
