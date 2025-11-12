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
    Player(std::shared_ptr<olc::net::connection> client,uint64_t id,uint32_t heroID) :
    client(std::move(client)),id(id),heroID(heroID){
    }

    [[nodiscard]] uint64_t GetID() const { return id; }

    [[nodiscard]] uint32_t GetHeroID() const {
        return heroID;
    }

    std::shared_ptr<olc::net::connection> client;
private:
    uint64_t id;
    uint32_t heroID;
public:
    std::queue<InputPacket> InputList;
    std::mutex InputLock;
};


class Barbarian:public Player {
public:
    Barbarian(std::shared_ptr<olc::net::connection> client,uint64_t id):Player(std::move(client),id,0) {
        ComponentManager* manager = GameWorld::GetComponentManager(id);
        auto move = manager->AddComponent<MovingComponent>(id,b2Vec2(200,200));

        manager->AddComponent<MoveTargetComponent>(id,b2Vec2(200,200),move->GetSpeed());

        manager->AddComponent<AttackComponent>(id,50);

        manager->AddComponent<AttributeComponent>(id);

        manager->AddComponent<HitComponent>(id);

        auto skill = manager->AddComponent<SkillComponent>(id);
        std::array<std::unique_ptr<Skill>,4> skills = {std::make_unique<meleeSpin>(id)};
        skill->LoadSkills(std::move(skills));

        manager->AddComponent<BuffComponent>(id);

        manager->AddComponent<StateMachineComponent>(id);
    }
};


class Shooter:public Player {
public:
    Shooter(std::shared_ptr<olc::net::connection> client,uint64_t id):Player(std::move(client),id,1) {
        ComponentManager* manager = GameWorld::GetComponentManager(id);
        auto move = manager->AddComponent<MovingComponent>(id,b2Vec2(200,200));

        manager->AddComponent<MoveTargetComponent>(id,b2Vec2(200,200),move->GetSpeed());

        manager->AddComponent<AttackComponent>(id,100);

        manager->AddComponent<AttributeComponent>(id);

        manager->AddComponent<HitComponent>(id);

        auto skill = manager->AddComponent<SkillComponent>(id);
        std::array<std::unique_ptr<Skill>,4> skills = {std::make_unique<ezQ>(id)};
        skill->LoadSkills(std::move(skills));

        manager->AddComponent<BuffComponent>(id);

        manager->AddComponent<StateMachineComponent>(id);
    }
};


#endif //TESTSERVER_PLAYER_H