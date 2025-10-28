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

#include "worldManager.h"
#include "../core/skills/skill.h"

class Player {
public:
    Player(std::shared_ptr<olc::net::connection> client,uint64_t id) :client(std::move(client)),id(id){
        ComponentManager* manager = GameWorld::GetComponentManager(id);

        b2Vec2 startPos = {200,200};
        manager->AddComponent<MovingComponent>(id,100,startPos);

        manager->AddComponent<AttackComponent>(id);

        manager->AddComponent<AttributeComponent>(id);

        manager->AddComponent<HitComponent>(id);

        std::array<std::unique_ptr<Skill>,4> skills = {std::make_unique<ezQ>(id)};
        manager->AddComponent<SkillComponent>(id,std::move(skills));
    }

    [[nodiscard]] uint64_t GetID() const { return id; }

    std::shared_ptr<olc::net::connection> client;
private:
    uint64_t id;
public:
    std::queue<InputPacket> InputList;
    std::mutex InputLock;
};

#endif //TESTSERVER_PLAYER_H