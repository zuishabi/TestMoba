#include <iostream>
#include "internal/server.h"

// TODO 完善客户端同步服务器创建人物

[[noreturn]] int main() {
    GameWorld::World = GameWorld::Initialize();
    CustomServer MainServer = CustomServer(7777);
    MainServer.Start();
    std::cout << "Start" << std::endl;
    std::thread logic_thread(&CustomServer::GameLogic, &MainServer);
    while (true) {
        MainServer.Update(5,true);
    }
}