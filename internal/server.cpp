//
// Created by 张俏悦 on 2025/9/25.
//
#include "server.h"
#include "objects/worldManager.h"

void CustomServer::OnMessage(olc::net::message &msg,uint32_t id) {
    Packet p;
    p.ParseFromArray(msg.body.data(),msg.body.size());
    if (p.has_input()) {
        auto player = GetPlayer(id);
        player->InputLock.lock();
        player->InputList.push(p.input());
        player->InputLock.unlock();
    }else {
        std::cout << "unknow message" << std::endl;
    }
}


void CustomServer::OnClientDisconnect(std::shared_ptr<olc::net::connection> client) {
    DisconnectTask t = {client};
    DisconnectTaskQueue.push_front(t);
}


bool CustomServer::OnClientConnect(std::shared_ptr<olc::net::connection> client) {
    ConnectTask t = {client};
    ConnectTaskQueue.push_front(t);
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
    auto syncers = GameWorld::objectsMap[id]->SyncerManager.GetSyncers();
    auto syncerMessages = std::vector<std::shared_ptr<Packet>>(syncers.size());
    int cur = 0;
    for (auto& p : syncers) {
        syncerMessages[cur] = p.second->getSync();
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
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.3f;
        shapeDef.material.restitution = 0;
        // 设置碰撞层级
        shapeDef.filter.categoryBits = CATEGORY_PLAYER;
        shapeDef.filter.maskBits = MASK_PLAYER;
        shapeDef.enableSensorEvents = true;
        b2CreateCircleShape(myBodyId, &shapeDef, &circle);


        std::cout << "Client Connect" << std::endl;
        uint64_t id = b2StoreBodyId(myBodyId);
        std::cout << "create id " << id <<std::endl;
        GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Player));
        std::shared_ptr<Player> player = std::make_shared<Player>(t.client,id);


        auto packet = std::make_shared<Packet>();
        PlayerSyncMessage *playerSync = packet->mutable_player_sync();
        // 向登录的玩家发送自身的登录消息以及已登录的玩家信息
        for (auto& p:playerMap) {
            playerSync->set_uid(p.second->GetID());
            playerSync->set_self(p.first == t.client->GetID());
            SendMessageA(t.client,packet);
            for (auto packs : GetAllSyncers(p.second->GetID())) {
                SendMessageA(t.client,packs);
            }
        }
        playerMap[player->client->GetID()] = player;
        // 向其他玩家广播加入消息，并同步玩家状态
        playerSync->set_uid(player->GetID());
        playerSync->set_self(false);
        BroadcastMessage(packet,t.client->GetID());
        for (auto packs : GetAllSyncers(id)) {
            BroadcastMessage(packs);
        }
    }
}

void CustomServer::DeletePlayer() {
    while (!DisconnectTaskQueue.empty()) {
        DisconnectTask t = DisconnectTaskQueue.back();
        std::cout << "Client Disconnect" << std::endl;

        // 清除玩家碰撞体
        DisconnectTaskQueue.pop_back();
        playerMap.erase(t.client->id);
    }
}
