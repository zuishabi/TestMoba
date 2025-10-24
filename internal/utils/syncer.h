//
// Created by 张俏悦 on 2025/10/21.
//

#ifndef TESTSERVER_SYNCER_H
#define TESTSERVER_SYNCER_H
#include "../../protos/test.pb.h"

enum class SyncerType {
    HealthSyncer,
    MoveSyncer,
    ManaSyncer,
    SpeedSyncer,
    AttackSpeedSyncer,
};


class Syncer{
public:
    virtual ~Syncer() = default;

    explicit Syncer(uint32_t uid,SyncerType type) : uid(uid), type(type) {
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
    uint32_t uid;
    bool updated = false;
    SyncerType type;
};


class HealthSyncer:public Syncer {
public:
    explicit HealthSyncer(uint32_t uid):Syncer(uid,SyncerType::HealthSyncer){}
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
        PlayerHealthSyncMessage *sync = packet->mutable_player_health_sync();
        sync->set_health(health);
        sync->set_max_health(maxHealth);
        sync->set_uid(uid);
        return packet;
    };
private:
    int health{};
    int maxHealth;
};


class MoveSyncer: public Syncer {
public:
    explicit MoveSyncer(uint32_t uid):Syncer(uid,SyncerType::MoveSyncer){}
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
        PlayerPositionSyncMessage *sync = packet->mutable_player_position_sync();
        sync->set_pos_x(pos.x);
        sync->set_pos_y(pos.y);
        sync->set_uid(uid);
        return packet;
    };
private:
    b2Vec2 pos{};
};


class ManaSyncer:Syncer {
public:
    ManaSyncer(uint32_t uid):Syncer(uid,SyncerType::ManaSyncer){}
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
        auto mana = packet->mutable_player_mana_sync();
        mana->set_uid(uid);
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
    AttackSpeedSyncer(uint32_t uid):Syncer(uid,SyncerType::AttackSpeedSyncer) {}
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
        auto attackSpeed = packet->mutable_player_attack_speed_sync();
        attackSpeed->set_uid(uid);
        attackSpeed->set_speed(coolDown);
        return packet;
    }
private:
    float coolDown;
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