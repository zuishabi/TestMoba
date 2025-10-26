//
// Created by 张俏悦 on 2025/10/24.
//

#ifndef TESTMOBA_SKILL_H
#define TESTMOBA_SKILL_H

#include <utility>

#include"../core.h"

class ezQ:public Skill {
public:
    explicit ezQ(std::shared_ptr<ComponentManager> mgr,std::shared_ptr<StraightBulletSkillSyncer> skills):
    Skill(std::move(mgr)),skills(std::move(skills)) {
        from->SyncerManager.AddSyncer(this->skills);
    }
public:
    void Update() override;

    void Execute(SkillInfo info) override;
private:
    std::shared_ptr<StraightBulletSkillSyncer> skills;
};


#endif //TESTMOBA_SKILL_H
