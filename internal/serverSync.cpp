//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"

void CustomServer::SyncPlayersStats() {
    for (auto& p : playerMap) {
        // p.second->position.x += p.second->currentDirection.x * 10;
        // p.second->position.y += p.second->currentDirection.y * 10;
        auto player = p.second;
        auto syncers = player->ComponentManager.SyncerManager.GetSyncers();
        for (auto& s : syncers) {
            if (s.second->IsUpdated()) {
                BroadcastMessage(s.second->getSync());
            }
        }
        // auto packet = std::make_shared<Packet>();
        // PlayerPositionSyncMessage *sync = packet->mutable_player_position_sync();
        // b2Vec2 position = b2Body_GetPosition(p.second->BodyID);
        // sync->set_pos_x(position.x);
        // sync->set_pos_y(position.y);
        // sync->set_uid(p.second->GetUID());
        // BroadcastMessage(packet);
        //
        // auto attribute = p.second->ComponentManager.GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
        // PlayerHealthSyncMessage *healthSync = packet->mutable_player_health_sync();
        // healthSync->set_health(attribute->GetHealth());
        // healthSync->set_uid(p.first);
        // BroadcastMessage(packet);
    }
}