//
// Created by 张俏悦 on 2025/10/29.
//

#include "buff.h"
#include "../components/components.h"

void Fire::Update() {
    if (life == 0) {
        destroyed = true;
    }
    if (timer.IsExpired()) {
        // 执行逻辑
        auto fromManager = GameWorld::objectsMap[from].get();
        auto skill = fromManager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
        int damage = (1 + (1.0/skill->skillAttributeSyncer->GeAttribute().strength)) * 5;
        fromManager->GetComponent<HitComponent>(ComponentType::HitComponentType)->Hit(owner,{0,damage});
        life -= 1;
        timer.Reset(std::chrono::seconds(1));
    }
}


void Fire::OnLoad() {
    timer.Reset(std::chrono::seconds(1));
}


void Fire::Upgrade(int level) {

}
