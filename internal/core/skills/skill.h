//
// Created by 张俏悦 on 2025/10/24.
//

#ifndef TESTMOBA_SKILL_H
#define TESTMOBA_SKILL_H

#include <utility>

#include"../core.h"

class ezQ:public Skill {
public:
    explicit ezQ(uint64_t from):Skill(from) {}
public:
    void Update() override;

    void Execute(SkillInfo info) override;
private:
};


#endif //TESTMOBA_SKILL_H
