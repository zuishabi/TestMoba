//
// Created by 张俏悦 on 2025/9/25.
//

#ifndef TESTSERVER_INTERNAL_H
#define TESTSERVER_INTERNAL_H


#include <olc_net.h>
#include <queue>
#include <utility>
#include <box2d/box2d.h>
#include "containers/containers.h"
#include "../protos/test.pb.h"
#include "objects/player.h"

enum CollisionCategory {
    CATEGORY_PLAYER = 0x0001,    // 玩家：0000 0000 0000 0001
    CATEGORY_OBSTACLE = 0x0002,  // 障碍物：0000 0000 0000 0010
    CATEGORY_BOUNDARY = 0x0004,  // 边界：0000 0000 0000 0100
    CATEGORY_SKILL = 0x0008,// 子弹：0000 0000 0000 1000
};

// 定义碰撞掩码（谁和谁碰撞）
enum CollisionMask {
    // 玩家：只和障碍物、边界、道具碰撞（不和玩家、子弹碰撞）
    MASK_PLAYER = CATEGORY_OBSTACLE | CATEGORY_BOUNDARY | CATEGORY_SKILL | CATEGORY_PLAYER,

    // 障碍物：和所有东西碰撞
    MASK_OBSTACLE = 0xFFFF,

    // 边界：只和玩家、障碍物碰撞
    MASK_BOUNDARY = CATEGORY_PLAYER | CATEGORY_OBSTACLE,

    // 技能，只和玩家碰撞
    MASK_SKILL = CATEGORY_PLAYER,
};

struct ConnectTask {
    std::shared_ptr<olc::net::connection>client;
};


struct DisconnectTask {
    std::shared_ptr<olc::net::connection>client;
};


class CustomServer:public olc::net::server_interface{
public:
    explicit CustomServer(uint16_t nPort):
        server_interface(nPort){
        b2WorldDef def = b2DefaultWorldDef();
        def.gravity = {0.0f, 0.0f};
        def.workerCount = 0;  // 单线程
        def.enableSleep = true;
        def.enableContinuous = false;
        def.restitutionThreshold = 5.0f; //定义触发反弹的最小碰撞速度,避免微小碰撞产生反弹
        def.contactHertz = 30.0f;  // 接触处理频率
        world_ = b2CreateWorld(&def);
    };

    std::shared_ptr<Player> GetPlayer(uint32_t uid) {
        auto it = playerMap.find(uid);
        if (it == playerMap.end()) return nullptr;
        return it->second;
    }

    void SetPlayer(std::shared_ptr<Player> player) {
        playerMap[player->GetUID()] = player;
    }

    std::unordered_map<uint32_t,std::shared_ptr<Player>>& GetPlayerMap() {
        return playerMap;
    }

    void BroadcastMessage(std::shared_ptr<Packet> message,uint32_t self = 0) const {
        size_t size = message->ByteSizeLong();
        std::vector<uint8_t> buf(size);
        message->SerializeToArray(buf.data(), static_cast<int>(size));
        olc::net::message msg;
        msg.PackMessage(buf);
        for (auto &p : playerMap) {
            if (self != 0 && self == p.first) {
                continue;
            }
            p.second->client->Send(msg);
        }
    }

    void SendMessageA(std::shared_ptr<olc::net::connection> client,std::shared_ptr<Packet> message) {
        size_t size = message->ByteSizeLong();
        std::vector<uint8_t> buf(size);
        message->SerializeToArray(buf.data(), static_cast<int>(size));
        olc::net::message msg;
        msg.PackMessage(buf);
        client->Send(msg);
    }

    void CreatePlayer();

    void DeletePlayer();

    void GameLogic();

    void BigGameLogic();

    void InitializeWorld();

    void SyncPlayersStats(); // 同步玩家的状态

    void UpdatePlayer();

    void ProcessInput();

protected:
    void OnMessage(olc::net::message &msg,uint32_t id) override;

    void OnClientDisconnect(std::shared_ptr<olc::net::connection> client) override;

    bool OnClientConnect(std::shared_ptr<olc::net::connection> client) override;

private:
    b2WorldId world_{0};
    std::unordered_map<uint32_t,std::shared_ptr<Player>> playerMap;
    std::unordered_map<int32_t,std::shared_ptr<Player>> BodyMap;
    SaveQueue<ConnectTask> ConnectTaskQueue;
    SaveQueue<DisconnectTask>DisconnectTaskQueue;
};

inline CustomServer MainServer = CustomServer(8888);

#endif //TESTSERVER_INTERNAL_H

