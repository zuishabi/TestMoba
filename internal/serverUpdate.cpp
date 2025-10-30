//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"

void CustomServer::UpdateObjects() {
    std::vector<uint64_t> destroyedList;
    for (auto& p :GameWorld::objectsMap) {
        ComponentManager* manager = GameWorld::GetComponentManager(p.second->id);
        if (manager->destroyed) {
            destroyedList.push_back(p.second->id);
            continue;
        }
        manager->Update();
    }
    for (int i = 0;i < destroyedList.size();i++) {
        ComponentManager* manager = GameWorld::GetComponentManager(destroyedList[i]);
        b2BodyId body = b2LoadBodyId(manager->id);

        auto packet = std::make_shared<Packet>();
        ObjectsDestroyedSyncMessage* destroy = packet->mutable_objects_destroyed_sync();
        destroy->set_id(manager->id);
        destroy->set_is_player(manager->Type == ManagerType::Player);
        BroadcastMessage(packet);

        b2DestroyBody(body);
        GameWorld::objectsMap.erase(manager->id);
    }
}
