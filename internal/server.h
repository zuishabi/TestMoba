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
#include "objects/worldManager.h"



struct ConnectTask {
    std::shared_ptr<olc::net::connection>client;
};


struct DisconnectTask {
    std::shared_ptr<olc::net::connection>client;
};


class CustomServer:public olc::net::server_interface{
public:
    explicit CustomServer(uint16_t nPort):
        server_interface(nPort) {
        Initialize();
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
    std::unordered_map<uint32_t,std::shared_ptr<Player>> playerMap;
    std::unordered_map<int32_t,std::shared_ptr<Player>> BodyMap;
    std::unordered_map<int32_t,std::shared_ptr<ComponentManager>> ManagerMap;
    SaveQueue<ConnectTask> ConnectTaskQueue;
    SaveQueue<DisconnectTask>DisconnectTaskQueue;
};

inline CustomServer MainServer = CustomServer(8888);

#endif //TESTSERVER_INTERNAL_H

