//
// Created by 张俏悦 on 2025/10/24.
//

#ifndef TESTMOBA_SKILL_H
#define TESTMOBA_SKILL_H


#include"../core.h"

class ezQ:public Skill {
public:
    explicit ezQ(uint64_t from):Skill(from) {}
public:
    void Update() override;

    void Execute(SkillInfo info) override;
};


class meleeSpin:public Skill {
public:
    explicit meleeSpin(uint64_t from):Skill(from) {
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
    }
public:
    void Update() override;

    void Execute(SkillInfo info) override;
private:
    b2ShapeId shape;
    bool activated = false;
};

#endif //TESTMOBA_SKILL_H
