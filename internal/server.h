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
#include "player/player.h"
#include "worldManager.h"


struct ConnectTask {
    std::shared_ptr<olc::net::connection>client;
    uint32_t heroID;
};


struct DisconnectTask {
    std::shared_ptr<olc::net::connection>client;
};


class CustomServer:public olc::net::server_interface{
public:
    explicit CustomServer(uint16_t nPort):
        server_interface(nPort) {
        GameWorld::Initialize();
    };

    std::shared_ptr<Player> GetPlayer(uint32_t uid) {
        auto it = playerMap.find(uid);
        if (it == playerMap.end()) return nullptr;
        return it->second;
    }

    void SetPlayer(const std::shared_ptr<Player>& player) {
        playerMap[player->client->GetID()] = player;
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

    void UpdateObjects();

    void ProcessInput();

protected:
    void OnMessage(olc::net::message &msg,uint32_t id) override;

    void OnClientDisconnect(std::shared_ptr<olc::net::connection> client) override;

    bool OnClientConnect(std::shared_ptr<olc::net::connection> client) override;

private:
    std::unordered_map<uint32_t,std::shared_ptr<Player>> playerMap;
    SaveQueue<ConnectTask> ConnectTaskQueue;
    SaveQueue<std::shared_ptr<DisconnectTask>>DisconnectTaskQueue; // 这里修改喂shared_map
    std::mutex waitLock;
    std::unordered_map<uint32_t,std::shared_ptr<olc::net::connection>> waitCreateMap;
};

#endif //TESTSERVER_INTERNAL_H

