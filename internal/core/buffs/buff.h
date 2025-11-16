//
// Created by 张俏悦 on 2025/10/29.
//

#ifndef TESTMOBA_BUFF_H
#define TESTMOBA_BUFF_H

#include "../core.h"
#include "../../utils/timer.h"

enum class BUFF {
    FIRE,
    CC,
};


class Fire:public Buff {
public:
    Fire(uint64_t from,uint64_t owner):Buff(from,owner,0,BUFF::FIRE){}
public:
    void Update() override;

    void OnLoad() override;

    void Upgrade(int level) override;
private:
    Timer timer;
    int life = 10;
};


class CCBuff:public Buff {
public:
    CCBuff(uint64_t from,uint64_t owner,double len):Buff(from,owner,0,BUFF::CC),len(len){}
public:
    void Update() override;

    void OnLoad() override;

    void Upgrade(int level) override;
private:
    Timer timer;
    double len;
};


#endif //TESTMOBA_BUFF_H