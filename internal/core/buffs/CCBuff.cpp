//
// Created by 张俏悦 on 2025/11/14.
//

#include "buff.h"
#include "../components/components.h"

void CCBuff::OnLoad() {
    timer.Reset(std::chrono::duration<double>(len));
    auto ownerManager = GameWorld::objectsMap[owner].get();
    ownerManager->GetComponent<StateMachineComponent>(ComponentType::StateMachineComponentType)->SetStateNode(State::CC);
}


void CCBuff::Update() {
    if (timer.IsExpired()) {
        auto ownerManager = GameWorld::objectsMap[owner].get();
        ownerManager->GetComponent<StateMachineComponent>(ComponentType::StateMachineComponentType)->SetStateNode(State::IDLE);
        destroyed = true;
    }
}

void CCBuff::Upgrade(int level) {

}
