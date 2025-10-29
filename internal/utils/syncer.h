//
// Created by 张俏悦 on 2025/10/21.
//

#ifndef TESTSERVER_SYNCER_H
#define TESTSERVER_SYNCER_H

#include <box2d/box2d.h>
#include <box2d/id.h>
#include "../../protos/test.pb.h"

enum class SyncerType {
    HealthSyncer,
    MoveSyncer,
    ManaSyncer,
    SpeedSyncer,
    AttackSpeedSyncer,
    StraightBulletSkillSyncer,
};


class Syncer{
public:
    virtual ~Syncer() = default;

    explicit Syncer(uint64_t id,SyncerType type) : id(id), type(type) {
    }

public:
    virtual std::shared_ptr<Packet> getSync() = 0;

    [[nodiscard]] SyncerType GetType() const {
        return type;
    }

    [[nodiscard]] bool IsUpdated() const{
        return updated;
    }
protected:
    uint64_t id;
    bool updated = false;
    SyncerType type;
};


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
        return packet;
    };
private:
    int health{};
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


class ManaSyncer:Syncer {
public:
    ManaSyncer(uint64_t id):Syncer(id,SyncerType::ManaSyncer){}
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
    int mana;
    int maxMana;
};


class AttackSpeedSyncer:Syncer {
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
class SkillSyncer:public Syncer {
public:
    // pos:代表技能的位置，如一技能二技能
    SkillSyncer(uint64_t id,uint64_t from,int pos):
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
        auto skills = sync->mutable_skill_info();
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


class SyncerManager {
public:
    void AddSyncer(const std::shared_ptr<Syncer>& syncer) {
        syncers[syncer->GetType()] = syncer;
    }

    std::shared_ptr<Syncer> GetSyncer(SyncerType type) {
        auto it = syncers.find(type);
        if (it != syncers.end()) {
            return std::dynamic_pointer_cast<Syncer>(it->second);
        }
        return nullptr;
    }

    std::unordered_map<SyncerType,std::shared_ptr<Syncer>> GetSyncers() {
        return syncers;
    }
private:
    std::unordered_map<SyncerType, std::shared_ptr<Syncer>> syncers;
};

#endif //TESTSERVER_SYNCER_H