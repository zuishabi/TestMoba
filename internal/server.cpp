//
// Created by 张俏悦 on 2025/9/25.
//
#include "server.h"
#include "worldManager.h"

void CustomServer::OnMessage(olc::net::message &msg,uint32_t id) {
    Packet p;
    p.ParseFromArray(msg.body.data(),msg.body.size());
    if (p.has_input() && playerMap.contains(id)) {
        auto player = GetPlayer(id);
        player->InputLock.lock();
        player->InputList.push(p.input());
        player->InputLock.unlock();
    }else if (p.has_create_player()) {
        waitLock.lock();
        if (waitCreateMap.contains(id)) {
            ConnectTask t = {waitCreateMap[id],p.create_player().hero_id()};
            ConnectTaskQueue.push_front(t);
            waitCreateMap.erase(id);
        }
        waitLock.unlock();
    }else {
        std::cout << "unknow message" << std::endl;
    }
}


void CustomServer::OnClientDisconnect(std::shared_ptr<olc::net::connection> client) {
    auto t = std::make_shared<DisconnectTask>(client);
    DisconnectTaskQueue.push_back(t);
}


bool CustomServer::OnClientConnect(std::shared_ptr<olc::net::connection> client) {
    waitLock.lock();
    waitCreateMap[client->id] = client;
    waitLock.unlock();
    return true;
}


void CustomServer::GameLogic() {
    using clock = std::chrono::steady_clock;
    const auto frameDuration = std::chrono::microseconds(1000000 / 30); // ~33.333 ms
    auto next = clock::now() + frameDuration;
    InitializeWorld(); // 初始化世界的地形

    const float timeStep = 1.0f / 30.0f; // 物理子步长 60Hz
    const int subStepCount = 4;
    int64_t currentPoint = 0;

    while (true) {
        currentPoint += 1;
        // 检查是否需要创建玩家
        CreatePlayer();
        // 检查是否要移除玩家
        DeletePlayer();
        // 处理玩家输入
        ProcessInput();
        // 更新玩家
        UpdateObjects();
        b2World_Step(GameWorld::World,timeStep,subStepCount);
        // 同步玩家状态
        SyncPlayersStats();

        if (currentPoint % 8 == 0) {
            BigGameLogic();
        }
        std::this_thread::sleep_until(next);
        next += frameDuration;
    }
}


void CustomServer::BigGameLogic() {

}


std::vector<std::shared_ptr<Packet>> GetAllSyncers(uint64_t id) {
    auto syncers = GameWorld::objectsMap[id]->SyncerManager->GetSyncers();
    auto syncerMessages = std::vector<std::shared_ptr<Packet>>(syncers.size());
    int cur = 0;
    for (auto& p : syncers) {
        syncerMessages[cur] = p.second->getSync();
        cur++;
    }
    return syncerMessages;
}


void CustomServer::CreatePlayer() {
    while (!ConnectTaskQueue.empty()) {
        ConnectTask t = ConnectTaskQueue.pop_back();

        // 创建物理体
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = (b2Vec2){200,200};
        b2BodyId myBodyId = b2CreateBody(GameWorld::World,&bodyDef);
        b2Circle circle;
        circle.center = {0,0};
        circle.radius = 5.0f;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10;
        shapeDef.material.friction = 0.3f;
        shapeDef.material.restitution = 0;
        // 设置碰撞层级
        shapeDef.filter.categoryBits = CATEGORY_PLAYER;
        shapeDef.filter.maskBits = MASK_PLAYER;
        shapeDef.enableSensorEvents = true;
        b2CreateCircleShape(myBodyId, &shapeDef, &circle);


        uint64_t id = b2StoreBodyId(myBodyId);
        std::cout << "create id " << id <<std::endl;
        GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Player));

        std::shared_ptr<Player> player;
        switch (t.heroID) {
            case 0:
                player = std::make_shared<Barbarian>(t.client,id,GroupType::BLUE);
                break;
            case 1:
                player = std::make_shared<Shooter>(t.client,id,GroupType::BLUE);
                break;
            case 2:
                player = std::make_shared<Magician>(t.client,id,GroupType::BLUE);
                break;
            default:
                std::cout << "unknown hero id" << std::endl;
        }


        auto packet = std::make_shared<Packet>();
        PlayerSyncMessage *playerSync = packet->mutable_player_sync();
        // 向登录的玩家发送自身的登录消息以及已登录的玩家信息
        playerMap[player->client->GetID()] = player;
        for (auto& p:playerMap) {
            playerSync->set_uid(p.second->GetID());
            playerSync->set_self(p.first == t.client->GetID());
            playerSync->set_hero_id(p.second->GetHeroID());
            SendMessageA(t.client,packet);
            for (auto packs : GetAllSyncers(p.second->GetID())) {
                SendMessageA(t.client,packs);
            }
        }
        // 向其他玩家广播加入消息，并同步玩家状态
        playerSync->set_uid(player->GetID());
        playerSync->set_self(false);
        playerSync->set_hero_id(t.heroID);
        BroadcastMessage(packet,t.client->GetID());
        for (auto packs : GetAllSyncers(id)) {
            BroadcastMessage(packs);
        }
    }
}

void CustomServer::DeletePlayer() {
    while (!DisconnectTaskQueue.empty()) {
        auto t = DisconnectTaskQueue.back();
        std::cout << "Client Disconnect " << (t->client == nullptr) << std::endl;
        // 首先检查是否已经选择了角色
        if (playerMap.contains(t->client->id)) {
            GameWorld::objectsMap[playerMap[t->client->id]->GetID()]->destroyed = true;
            playerMap.erase(t->client->id);
            std::cout << "erase connect map" << std::endl;
        }else {
            // 处于等待队列中
            waitCreateMap.erase(t->client->id);
            std::cout << "erase wait map" << std::endl;
        }

        DisconnectTaskQueue.pop_back();
    }
}
