//
// Created by 张俏悦 on 2025/10/24.
//

#include "components.h"

void SkillComponent::ExecuteSkill(int pos,SkillInfo info) {
    if (skills[pos] == nullptr) {
        return;
    }
    skills[pos]->Execute(info);
}

void SkillComponent::Update() {
    for (int i = 0;i < 4;i++) {
        if (skills[i] != nullptr) {
            skills[i]->Update();
        }
    }
}
