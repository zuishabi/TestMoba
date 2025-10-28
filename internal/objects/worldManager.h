//
// Created by 张俏悦 on 2025/10/24.
//

#ifndef TESTMOBA_WORLDMANAGER_H
#define TESTMOBA_WORLDMANAGER_H

class ComponentManager;

class GameWorld {
public:
    static inline b2WorldId World;
    static inline std::unordered_map<uint64_t,std::unique_ptr<ComponentManager>> objectsMap;

    static b2WorldId Initialize() {
        b2WorldDef def = b2DefaultWorldDef();
        def.gravity = {0.0f, 0.0f};
        def.workerCount = 0;  // 单线程
        def.enableSleep = true;
        def.enableContinuous = false;
        def.restitutionThreshold = 5.0f; //定义触发反弹的最小碰撞速度,避免微小碰撞产生反弹
        def.contactHertz = 30.0f;  // 接触处理频率
        return b2CreateWorld(&def);
    }

    static ComponentManager* GetComponentManager(uint64_t id) {
        auto data =  b2Body_GetUserData(b2LoadBodyId(id));
        if (!data) {
            return nullptr;
        };
        auto sp_ptr = static_cast<ComponentManager*>(data);
        return sp_ptr;
    }

    static ComponentManager* StoreComponentManager(uint64_t id,std::unique_ptr<ComponentManager> manager) {
        b2BodyId body = b2LoadBodyId(id);
        b2Body_SetUserData(body,manager.get());
        objectsMap[id] = std::move(manager);
        return objectsMap[id].get();
    }

    static void DestroyBodyAndClearUserData(b2BodyId id) {
        void* data = b2Body_GetUserData(id);
        if (data) {
            auto sp_ptr = static_cast<std::shared_ptr<ComponentManager>*>(data);
            delete sp_ptr; // 释放堆上指针（shared_ptr 内部会正确管理对象生命周期）
            b2Body_SetUserData(id, nullptr);
        }
        b2DestroyBody(id);
    }
};

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

#endif //TESTMOBA_WORLDMANAGER_H