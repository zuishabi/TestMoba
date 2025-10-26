//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"

void CustomServer::ProcessInput() {
    for (auto &p : playerMap) {
        std::shared_ptr<Player> player = p.second;
        std::lock_guard<std::mutex> lock(p.second->InputLock);
        while (!p.second->InputList.empty()) {
            InputPacket& input = p.second->InputList.front();
            if (input.has_move()) {
                auto moving = player->ComponentManager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
                moving->SetTargetDirection(b2Vec2{input.move().x(),input.move().y()});
            }else if (input.has_player_attack()) {
                // 处理玩家攻击
                auto attack = player->ComponentManager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
                std::shared_ptr<Player> targetPlayer = playerMap[input.player_attack().uid()];
                b2Vec2 distance = b2Body_GetPosition(p.second->BodyID) - b2Body_GetPosition(targetPlayer->BodyID);
                if (distance.x * distance.x + distance.y * distance.y <= attack->scale * attack->scale) {
                    attack->Attack(targetPlayer->ComponentManager);
                    auto attribute = player->ComponentManager->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
                    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
                    auto sync = packet->mutable_player_attack_sync();
                    sync->set_to(targetPlayer->GetUID());
                    sync->set_from(player->GetUID());
                    float coolDown = attribute->GetAttackSpeed();
                    sync->set_cool_down(coolDown);
                    BroadcastMessage(packet);
                }
            }else if (input.has_execute_skill()) {
                auto skill = player->ComponentManager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
                auto info = input.execute_skill();
                skill->ExecuteSkill(0,{b2Vec2{info.pos_x(),info.pos_y()},b2Vec2{info.direction_x(),info.direction_y()}});
            }
            p.second->InputList.pop();
        }
    }
}
