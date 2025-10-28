//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"

void CustomServer::UpdateObjects() {
    for (auto& p :GameWorld::objectsMap) {
        ComponentManager* manager = GameWorld::GetComponentManager(p.second->id);
        if (manager->destroyed) {
            b2BodyId body = b2LoadBodyId(manager->id);
            b2DestroyBody(body);
            GameWorld::objectsMap.erase(manager->id);
            auto packet = std::make_shared<Packet>();
            ObjectsDestroyedSyncMessage* destroy = packet->mutable_objects_destroyed_sync();
            destroy->set_id(manager->id);
            BroadcastMessage(packet);
            continue;
        }
        manager->Update();
    }
}
