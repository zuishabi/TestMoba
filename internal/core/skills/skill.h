//
// Created by 张俏悦 on 2025/10/24.
//

#ifndef TESTMOBA_SKILL_H
#define TESTMOBA_SKILL_H


#include"../core.h"
#include "../../utils/timer.h"
#include "../components/components.h"

class ezQ:public Skill {
public:
    explicit ezQ(uint64_t from):Skill(1,from) {
        auto manager = GameWorld::objectsMap[from].get();
        skillInfoSyncer = std::make_shared<SkillInfoSyncer>(from,SkillInfo());
        manager->SyncerManager->AddSyncer(skillInfoSyncer);
        skillComponent = manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
    }
public:
    void Update() override;

    void Execute(ExecuteSkillInfo info) override;
private:
    std::shared_ptr<SkillInfoSyncer> skillInfoSyncer;
    Timer waitTimer;
    SkillComponent* skillComponent;
    ExecuteSkillInfo info;
};


class meleeSpin:public Skill {
public:
    explicit meleeSpin(uint64_t from):Skill(2,from) {
        b2BodyId body = b2LoadBodyId(from);
        b2Circle circle;
        circle.center = {0,0};
        circle.radius = 40;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = CATEGORY_SKILL;
        shapeDef.filter.maskBits = MASK_SKILL;
        shapeDef.enableSensorEvents = true;
        b2ShapeId bodyShape = b2CreateCircleShape(body, &shapeDef, &circle);
        shape = bodyShape;

        auto manager = GameWorld::objectsMap[from].get();
        attribute = manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType)->skillAttributeSyncer;

        skillInfoSyncer = std::make_shared<SkillInfoSyncer>(from,SkillInfo());
        manager->SyncerManager->AddSyncer(skillInfoSyncer);

        skillComponent = manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
    }
public:
    void Update() override;

    void Execute(ExecuteSkillInfo info) override;
private:
    b2ShapeId shape;
    Timer startTimer;
    Timer endTimer;
    std::shared_ptr<SkillAttributeSyncer> attribute; // 保存玩家的技能属性信息
    std::shared_ptr<SkillInfoSyncer> skillInfoSyncer;
    SkillComponent* skillComponent;
};


class rockWall:public Skill {
public:
    rockWall(uint64_t from):Skill(3,from) {
        auto manager = GameWorld::objectsMap[from].get();
        skillInfoSyncer = std::make_shared<SkillInfoSyncer>(from,SkillInfo());
        manager->SyncerManager->AddSyncer(skillInfoSyncer);
        skillComponent = manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
    }
public:
    void Update() override;

    void Execute(ExecuteSkillInfo info) override;
private:
    std::shared_ptr<SkillInfoSyncer> skillInfoSyncer;
    Timer waitTimer;
    SkillComponent* skillComponent;
    ExecuteSkillInfo info;
};

#endif //TESTMOBA_SKILL_H
