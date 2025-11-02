//
// Created by 张俏悦 on 2025/10/29.
//

#ifndef TESTMOBA_BUFF_H
#define TESTMOBA_BUFF_H

#include "../core.h"
#include "../../utils/timer.h"


class Fire:public Buff {
public:
    Fire(uint64_t id,uint64_t owner):Buff(id,owner){}
public:
    void Update() override;

    void OnLoad() override;
private:
    Timer timer;
    int life = 10;
};


#endif //TESTMOBA_BUFF_H