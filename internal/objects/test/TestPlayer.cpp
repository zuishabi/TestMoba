//
// Created by 张俏悦 on 2025/11/26.
//

#include "../objects.h"
#include "../../core/core.h"
#include "../../core/components/components.h"

TestPlayer::TestPlayer(uint64_t id,GroupType groupType):Object(id),objectSyncer(std::make_shared<ObjectSyncer>(id)) {
    ComponentManager* manager = GameWorld::GetComponentManager(id);
    manager->groupType = groupType;
    manager->AddComponent<HitComponent>(id);
    manager->AddComponent<AttributeComponent>(id);
    auto body = b2LoadBodyId(id);
    b2Vec2 pos = b2Body_GetPosition(body);
    objectSyncer->SetInfo(ObjectInfo(3,id,pos.x,pos.y,1));
    manager->SyncerManager->AddSyncer(objectSyncer);
}


uint64_t TestPlayer::CreateTestPlayer(b2Vec2 pos, GroupType type) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;
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
    GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Object));
    TestPlayer temp(id,type);
    return id;
}
