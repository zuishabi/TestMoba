//
// Created by 张俏悦 on 2025/10/21.
//

#ifndef TESTSERVER_SYNCER_H
#define TESTSERVER_SYNCER_H

#include <box2d/box2d.h>
#include <box2d/id.h>
#include "../../../protos/test.pb.h"
#include "../core.h"

class HealthSyncer:public Syncer {
public:
    explicit HealthSyncer(uint64_t id):Syncer(id,SyncerType::HealthSyncer){}
public:
    void SetHealth(int health) {
        this->health = health;
        this->updated = true;
    }

    int GetHealth() const {
        return this->health;
    }

    void SetMaxHealth(int maxH) {
        this->updated = true;
        this->maxHealth = maxH;
        if (health > maxHealth) {
            health = maxHealth;
        }
    }

    [[nodiscard]] int GetMaxHealth() const {
        return maxHealth;
    }

    std::shared_ptr<Packet> getSync() override {
        this->updated = false;
        auto packet = std::make_shared<Packet>();
        HealthSyncMessage *sync = packet->mutable_health_sync();
        sync->set_health(health);
        sync->set_max_health(maxHealth);
        sync->set_uid(id);
        std::cout << "sync health:" << id << std::endl;
        return packet;
    };
private:
    int health;
    int maxHealth;
};


class MoveSyncer: public Syncer {
public:
    explicit MoveSyncer(uint64_t id):Syncer(id,SyncerType::MoveSyncer){}
public:
    void SetPos(b2Vec2 pos) {
        this->updated = true;
        this->pos = pos;
    }

    [[nodiscard]] b2Vec2 GetPos() const {
        return pos;
    }

    std::shared_ptr<Packet> getSync() override {
        this->updated = false;
        auto packet = std::make_shared<Packet>();
        PositionSyncMessage *sync = packet->mutable_position_sync();
        sync->set_pos_x(pos.x);
        sync->set_pos_y(pos.y);
        sync->set_id(id);
        return packet;
    };
private:
    b2Vec2 pos{};
};


class ManaSyncer:public Syncer {
public:
    explicit ManaSyncer(uint64_t id):Syncer(id,SyncerType::ManaSyncer){}
public:
    void SetMana(int m) {
        this->updated = true;
        this->mana = m;
    }

    [[nodiscard]] int GetMana() const {
        return mana;
    }

    void SetMaxMana(int maxM) {
        this->updated = true;
        this->maxMana = maxM;
        if (mana > maxM) {
            mana = maxM;
        }
    }

    [[nodiscard]] int GetMaxMana() const {
        return maxMana;
    }

    std::shared_ptr<Packet>getSync() override {
        this->updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto mana = packet->mutable_mana_sync();
        mana->set_uid(id);
        mana->set_mana(this->mana);
        mana->set_max_mana(maxMana);
        return packet;
    }
private:
    int mana{};
    int maxMana{};
};


class AttackSpeedSyncer:public Syncer {
public:
    AttackSpeedSyncer(uint64_t id):Syncer(id,SyncerType::AttackSpeedSyncer) {}
public:
    void SetSpeed(float speed) {
        coolDown = speed;
    }

    [[nodiscard]] float GetSpeed() const{
        return coolDown;
    }

    std::shared_ptr<Packet>getSync() override {
        this->updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto attackSpeed = packet->mutable_attack_speed_sync();
        attackSpeed->set_uid(id);
        attackSpeed->set_speed(coolDown);
        return packet;
    }
private:
    float coolDown;
};


// 用于同步技能状态
class StraightBulletSkillSyncer:public Syncer {
public:
    // pos:代表技能的位置，如一技能二技能
    StraightBulletSkillSyncer(uint64_t id,uint64_t from,int pos):
    Syncer(id,SyncerType::StraightBulletSkillSyncer),pos(pos),from(from),id(id) {
        updated = true;
        bodyID = b2LoadBodyId(id);
    }
public:
    std::shared_ptr<Packet>getSync() override {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto sync = packet->mutable_sync_skill();
        sync->set_pos(pos);
        sync->set_uid(from);
        auto skills = sync->mutable_straight_bullet_skill_info();
        skills->set_id(id);
        skills->set_angle(b2Rot_GetAngle(b2Body_GetRotation(bodyID)));
        updated = false;
        return packet;
    }
private:
    int pos;
    uint64_t from;
    uint64_t id;
    b2BodyId bodyID;
};


#endif //TESTSERVER_SYNCER_H