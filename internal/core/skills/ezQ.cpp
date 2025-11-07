//
// Created by 张俏悦 on 2025/10/24.
//

#include <box2d/box2d.h>
#include "../../worldManager.h"
#include "../../utils/mMath.h"
#include "skill.h"
#include "../../player/player.h"


class ezQComponent:public SkillObjectComponent{
public:
    ezQComponent(uint64_t id,uint64_t from,b2ShapeId shapeID):
    SkillObjectComponent(id,from),shapeID(shapeID),positionSyncer(std::make_shared<MoveSyncer>(id)),
    objectSyncer(std::make_shared<ObjectSyncer>(id)) {
        auto body = b2LoadBodyId(id);
        b2Vec2 pos = b2Body_GetPosition(body);
        objectSyncer->SetInfo(ObjectInfo(1,id,pos.x,pos.y,1,b2Rot_GetAngle(b2Body_GetRotation(body))));
        manager->SyncerManager->AddSyncer(positionSyncer);
        expireAt = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        attribute = GameWorld::objectsMap[from].get()->GetComponent<SkillComponent>(ComponentType::SkillComponentType)->skillAttributeSyncer;
    }
public:
    void Update() override;
private:
    b2ShapeId shapeID;
    std::chrono::steady_clock::time_point expireAt;
    std::shared_ptr<MoveSyncer> positionSyncer;
    std::shared_ptr<SkillAttributeSyncer> attribute; // 保存来自玩家的技能信息
    std::shared_ptr<ObjectSyncer> objectSyncer;
};


void ezQComponent::Update() {
    if (std::chrono::steady_clock::now() > expireAt) {
        manager->destroyed = true;
        return;
    }

    positionSyncer->SetPos(b2Body_GetPosition(b2LoadBodyId(id)));
    int capacity = b2Shape_GetSensorCapacity( shapeID );
    std::vector<b2ShapeId> overlaps;
    overlaps.resize( capacity );
    int damage = 10 * (1+1.0/static_cast<double>(attribute->GeAttribute().strength));

    // Now get all overlaps and record the actual count
    int count = b2Shape_GetSensorOverlaps( shapeID, overlaps.data(), capacity );
    overlaps.resize( count );

    for ( int j = 0; j < count; ++j )
    {
        b2ShapeId visitorId = overlaps[j];
        // Ensure the visitorId is valid
        if ( b2Shape_IsValid( visitorId ) == false )
        {
            continue;
        }
        b2BodyId body = b2Shape_GetBody(visitorId);
        uint64_t _id = b2StoreBodyId(body);
        if (_id == from) {
            continue;
        }
        ComponentManager* targetManager = GameWorld::GetComponentManager(_id);
        if (targetManager && targetManager->Type == ManagerType::Player) {
            auto hit = targetManager->GetComponent<HitComponent>(ComponentType::HitComponentType);
            if (hit != nullptr) {
                hit->Hit(_id,{damage});
                manager->destroyed = true;
                break;
            }
        }
    }
}



void ezQ::Update() {

}


void ezQ::Execute(ExecuteSkillInfo info) {

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_kinematicBody;
    bodyDef.isBullet = true;
    b2BodyId myBodyId = b2CreateBody(GameWorld::World,&bodyDef);
    b2Vec2 dir = DirectionFromRadians(info.rotate);
    b2Body_SetLinearVelocity(myBodyId,dir*200);
    b2Body_SetTransform(myBodyId,info.pos,b2MakeRot(info.rotate));

    b2Capsule capsule;
    capsule.center1 = {0,4};
    capsule.center2 = {0,-4};
    capsule.radius = 3;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.isSensor = true;
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0;
    shapeDef.filter.categoryBits = CATEGORY_SKILL;
    shapeDef.filter.maskBits = MASK_SKILL;
    shapeDef.enableSensorEvents = true;
    b2ShapeId bodyShape = b2CreateCapsuleShape(myBodyId, &shapeDef, &capsule);

    auto id = b2StoreBodyId(myBodyId);
    auto manager = GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Skill));
    std::cout << "create skill:" << id << std::endl;
    manager->AddComponent<ezQComponent>(id,from,bodyShape);

    skillInfoSyncer->Execute();
}
