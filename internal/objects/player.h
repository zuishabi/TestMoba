//
// Created by 张俏悦 on 2025/10/16.
//

#ifndef TESTSERVER_PLAYER_H
#define TESTSERVER_PLAYER_H

#include <olc_net.h>
#include <queue>
#include <utility>
#include "../../protos/test.pb.h"
#include "../core/core.h"
#include "../core/components/components.h"

class Player {
public:
    Player(uint32_t uid,std::shared_ptr<olc::net::connection> client,b2BodyId bodyID) :
        client(std::move(client)), uid(uid),BodyID(bodyID){
        std::shared_ptr<MoveSyncer> moveSyncer = std::make_shared<MoveSyncer>(uid);
        ComponentManager.AddComponent<MovingComponent>(100,bodyID,&ComponentManager,&ComponentManager.SyncerManager,moveSyncer);
        ComponentManager.AddComponent<AttackComponent>(&ComponentManager);
        std::shared_ptr<HealthSyncer> healthSyncer = std::make_shared<HealthSyncer>(uid);
        std::shared_ptr<ManaSyncer> manaSyncer = std::make_shared<ManaSyncer>(uid);
        std::shared_ptr<AttackSpeedSyncer> attackSpeedSyncer = std::make_shared<AttackSpeedSyncer>(uid);
        ComponentManager.AddComponent<AttributeComponent>(&ComponentManager,&ComponentManager.SyncerManager,
            healthSyncer,manaSyncer,attackSpeedSyncer);
    }

    [[nodiscard]] uint32_t GetUID() const { return uid; }

    std::shared_ptr<olc::net::connection> client;
private:
    uint32_t uid;
public:
    std::queue<InputPacket> InputList;
    std::mutex InputLock;
    ComponentManager ComponentManager{};
    b2BodyId BodyID;
};

#endif //TESTSERVER_PLAYER_H