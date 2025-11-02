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

#include "../worldManager.h"
#include "../core/skills/skill.h"

class Player {
public:
    Player(std::shared_ptr<olc::net::connection> client,uint64_t id) :client(std::move(client)),id(id){
    }

    [[nodiscard]] uint64_t GetID() const { return id; }

    std::shared_ptr<olc::net::connection> client;
private:
    uint64_t id;
public:
    std::queue<InputPacket> InputList;
    std::mutex InputLock;
};


class Barbarian:public Player {
public:
    Barbarian(std::shared_ptr<olc::net::connection> client,uint64_t id):Player(std::move(client),id) {
        ComponentManager* manager = GameWorld::GetComponentManager(id);
        manager->AddComponent<MovingComponent>(id,b2Vec2(200,200));

        manager->AddComponent<AttackComponent>(id,50);

        manager->AddComponent<AttributeComponent>(id);

        manager->AddComponent<HitComponent>(id);

        manager->AddComponent<StateMachineComponent>(id);

        auto skill = manager->AddComponent<SkillComponent>(id);
        std::array<std::unique_ptr<Skill>,4> skills = {std::make_unique<meleeSpin>(id)};
        skill->LoadSkills(std::move(skills));

        manager->AddComponent<BuffComponent>(id);
    }
};


class Shooter:public Player {
    Shooter(std::shared_ptr<olc::net::connection> client,uint64_t id):Player(std::move(client),id) {
        ComponentManager* manager = GameWorld::GetComponentManager(id);
        b2Vec2 startPos = {200,200};
        manager->AddComponent<MovingComponent>(id,b2Vec2(200,200));
    }
};


#endif //TESTSERVER_PLAYER_H