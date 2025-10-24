#include <iostream>
#include "internal/server.h"
#include "protos/test.pb.h"


[[noreturn]] int main() {
    MainServer.Start();
    std::cout << "Start" << std::endl;
    std::thread logic_thread(&CustomServer::GameLogic, &MainServer);
    while (true) {
        MainServer.Update(5,true);
    }
}