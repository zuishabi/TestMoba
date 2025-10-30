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
            uint64_t id = p.second->GetID();
            ComponentManager* manager = GameWorld::GetComponentManager(id);


            if (input.has_move()) {
                auto moving = manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
                moving->ProcessInput(b2Vec2{input.move().x(),input.move().y()});
            }else if (input.has_player_attack()) {
                // 处理玩家攻击
                auto attack = manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
                b2BodyId body = b2LoadBodyId(p.second->GetID());
                b2BodyId targetBody = b2LoadBodyId(input.player_attack().uid());
                if (!b2Body_IsValid(targetBody)) {
                    continue;
                }
                b2Vec2 distance = b2Body_GetPosition(body) - b2Body_GetPosition(targetBody);

                if (distance.x * distance.x + distance.y * distance.y <= attack->scale * attack->scale) {
                    attack->Attack(input.player_attack().uid());
                    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
                    auto sync = packet->mutable_attack_sync();
                    sync->set_to(input.player_attack().uid());
                    sync->set_from(player->GetID());
                    BroadcastMessage(packet);
                }
            }else if (input.has_execute_skill()) {
                auto skill = manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
                auto info = input.execute_skill();
                skill->ExecuteSkill(0,{b2Vec2{info.pos_x(),info.pos_y()},info.rotate()});
            }


            p.second->InputList.pop();
        }
    }
}
