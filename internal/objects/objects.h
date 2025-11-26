//
// Created by 张俏悦 on 2025/11/26.
//

#ifndef TESTMOBA_OBJECTS_H
#define TESTMOBA_OBJECTS_H
#include <box2d/math_functions.h>

#include "../core/core.h"
#include "../core/syncers/syncer.h"

class Object {
public:
    explicit Object(uint64_t id):id(id) {}
private:
    uint64_t id;
};


class TestPlayer:public Object{
public:
    explicit TestPlayer(uint64_t id,GroupType groupType);

    static uint64_t CreateTestPlayer(b2Vec2 pos,GroupType type);
private:
    std::shared_ptr<ObjectSyncer> objectSyncer;
};

#endif //TESTMOBA_OBJECTS_H