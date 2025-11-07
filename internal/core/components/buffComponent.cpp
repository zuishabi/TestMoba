//
// Created by 张俏悦 on 2025/10/29.
//

#include "components.h"

void BuffComponent::Update() {
    for (int i = 0;i < buffs.size();i++) {
        if (buffs[i] == nullptr) {
            continue;
        }
        if (buffs[i]->destroyed) {
            buffs[i].reset();
            continue;
        }
        buffs[i]->Update();
    }
}

void BuffComponent::AddBuff(std::shared_ptr<Buff> buff) {
    for (int i = 0;i < buffs.size();i ++) {
        if (buffs[i] == nullptr) {
            buffs[i] = std::move(buff);
            return;
        }
    }
    buff->OnLoad();
    buffs.emplace_back(buff);
}

