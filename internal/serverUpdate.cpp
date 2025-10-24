//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"

void CustomServer::UpdatePlayer() {
    for (auto& p :playerMap) {
        p.second->ComponentManager.Update();
    }
}
