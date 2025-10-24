//
// Created by 张俏悦 on 2025/10/22.
//

#ifndef TESTSERVER_BULLET_H
#define TESTSERVER_BULLET_H

#include <unordered_map>
#include <memory>
#include <box2d/id.h>

class Bullet {
public:
    Bullet(b2BodyId target):targetID(target) {

    }

    virtual void Update() = 0;
public:
    uint32_t id;
private:
    int speed = 300;
    b2BodyId bodyID;
    b2BodyId targetID;
};


class BulletManager {
public:
    void AddBullet(const std::shared_ptr<Bullet>& bullet) {
        currentID += 1;
        bullet->id = currentID;
        bulletMap[currentID] = bullet;
    }
private:
    std::unordered_map<uint32_t,std::shared_ptr<Bullet>> bulletMap;
    uint32_t currentID = 0;
};

inline BulletManager BulletManager;

#endif //TESTSERVER_BULLET_H