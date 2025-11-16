//
// Created by 张俏悦 on 2025/10/21.
//

#ifndef TESTSERVER_SYNCER_H
#define TESTSERVER_SYNCER_H

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
class SkillInfoSyncer:public Syncer {
public:
    // pos:代表技能的位置，如一技能二技能
    SkillInfoSyncer(uint64_t from,SkillInfo info):
    Syncer(from,SyncerType::SkillInfoSyncer),from(from),info(info){}
public:
    // 执行一次技能，会向周围玩家进行更新。
    void Execute() {
        updated = true;
    }

    void SetSkillInfo(SkillInfo skillInfo) {
        info = skillInfo;
    }

    std::shared_ptr<Packet>getSync() override {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto sync = packet->mutable_sync_skill();
        sync->set_pos(info.pos);
        sync->set_uid(from);
        sync->set_angle(info.rotate);
        sync->set_left_time(info.timeLeft);
        updated = false;
        return packet;
    }
private:
    uint64_t from;
    SkillInfo info;
};


class StateSyncer:public Syncer {
public:
    explicit StateSyncer(uint64_t id):Syncer(id,SyncerType::StateSyncer){}
public:
    std::shared_ptr<Packet>getSync()override {
        updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto state = packet->mutable_state_sync();
        state->set_id(id);
        std::cout << "sync state = " << static_cast<uint32_t>(currentState) << std::endl;
        state->set_state(static_cast<uint32_t>(currentState));
        return packet;
    }

    void SetState(State state) {
        if (state != currentState) {
            updated = true;
            currentState = state;
        }
    }

    [[nodiscard]] State GetState() const {
        return currentState;
    }

    State currentState;
};


class SkillAttributeSyncer:public Syncer {
public:
    SkillAttributeSyncer(uint64_t id):Syncer(id,SyncerType::SkillAttributeSyncer) {}
public:
    std::shared_ptr<Packet> getSync() override {
        updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto sync = packet->mutable_skill_attribute_sync();
        sync->set_uid(id);
        sync->set_efficiency(attribute.efficiency);
        sync->set_scale(attribute.scale);
        sync->set_strength(attribute.strength);
        return packet;
    }

    void SetAttribute(SkillAttribute newAttribute) {
        attribute = newAttribute;
        updated = true;
    }

    SkillAttribute GeAttribute() {
        return attribute;
    }
private:
    SkillAttribute attribute;
};


class SpeedSyncer:public Syncer {
public:
    SpeedSyncer(uint64_t id):Syncer(id,SyncerType::SpeedSyncer) {}
public:
    std::shared_ptr<Packet> getSync()override {
        updated = false;
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        auto sync = packet->mutable_speed_sync();
        sync->set_id(id);
        sync->set_speed(speed);
        return packet;
    }

    void SetSpeed(int speed) {
        this->speed = speed;
        updated = true;
    }

    int GetSpeed() {
        return speed;
    }
private:
    int speed;
};


// 同步实体信息
class ObjectSyncer:public Syncer {
public:
    ObjectSyncer(uint64_t id):Syncer(id,SyncerType::ObjectSyncer) {
        updated = true;
    }

    void SetInfo(ObjectInfo info) {
        this->info = info;
        updated = true;
    }

    // 通过加载body获得body信息
    void LoadObject(b2BodyId body) {

    }


    const ObjectInfo& GetInfo() {
        return info;
    }

    std::shared_ptr<Packet>getSync() override{
        updated = false;
        std::shared_ptr<Packet> res = std::make_shared<Packet>();
        auto sync = res->mutable_object_sync();
        sync->set_angle(info.angle);
        sync->set_id(info.id);
        sync->set_scale(info.scale);
        sync->set_type_id(info.type_id);
        sync->set_x(info.x);
        sync->set_y(info.y);
        return res;
    }
private:
    ObjectInfo info;
};


#endif //TESTSERVER_SYNCER_H