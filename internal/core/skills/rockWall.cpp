//
// Created by 张俏悦 on 2025/11/17.
//

#include "skill.h"
#include "../buffs/buff.h"
#include "../../utils/mMath.h"

class rockWallComponent:public SkillObjectComponent{
public:
    rockWallComponent(uint64_t id,uint64_t from,b2ShapeId shapeID):
    SkillObjectComponent(id,from),shapeID(shapeID),objectSyncer(std::make_shared<ObjectSyncer>(id)) {
        auto body = b2LoadBodyId(id);
        b2Vec2 pos = b2Body_GetPosition(body);
        objectSyncer->SetInfo(ObjectInfo(1,id,pos.x,pos.y,1,b2Rot_GetAngle(b2Body_GetRotation(body))));
        manager->SyncerManager->AddSyncer(objectSyncer);
        expireAt = std::chrono::steady_clock::now() + std::chrono::seconds(4);
        attribute = GameWorld::objectsMap[from].get()->GetComponent<SkillComponent>(ComponentType::SkillComponentType)->skillAttributeSyncer;
    }
public:
    void Update() override;
private:
    b2ShapeId shapeID;
    std::chrono::steady_clock::time_point expireAt;
    std::shared_ptr<SkillAttributeSyncer> attribute; // 保存来自玩家的技能信息
    std::shared_ptr<ObjectSyncer> objectSyncer;
};


void rockWallComponent::Update() {
    if (std::chrono::steady_clock::now() > expireAt) {
        manager->destroyed = true;
        return;
    }

    int capacity = b2Shape_GetSensorCapacity( shapeID );
    std::vector<b2ShapeId> overlaps;
    overlaps.resize( capacity );

    // Now get all overlaps and record the actual count
    int count = b2Shape_GetSensorOverlaps( shapeID, overlaps.data(), capacity );
    overlaps.resize( count );

    std::cout << count << std::endl;

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
        if (targetManager && targetManager->Type == ManagerType::Skill) {
            targetManager->destroyed = true;
        }
    }
}

void rockWall::Execute(ExecuteSkillInfo info) {
    this->info = info;
    skillInfoSyncer->Execute();
    skillComponent->UseNormalSkill.emit(this);
    activated = true;
    waitTimer.Reset(std::chrono::duration<double>(0.2));
}


void rockWall::Update() {
    if (activated && waitTimer.IsExpired()) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        b2BodyId myBodyId = b2CreateBody(GameWorld::World,&bodyDef);
        b2Body_SetTransform(myBodyId,info.pos,b2MakeRot(info.rotate));

        b2Polygon box = b2MakeBox(20,1);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.3f;
        shapeDef.material.restitution = 0;
        shapeDef.filter.categoryBits = CATEGORY_BLOCK_SKILL;
        shapeDef.filter.maskBits = MASK_BLOCK_SKILL;
        shapeDef.enableSensorEvents = true;
        b2ShapeId bodyShape = b2CreatePolygonShape(myBodyId, &shapeDef, &box);

        auto id = b2StoreBodyId(myBodyId);
        auto manager = GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Skill));
        manager->AddComponent<rockWallComponent>(id,from,bodyShape);
        activated = false;

        auto selfManager = GameWorld::objectsMap[from].get();
        selfManager->GetComponent<StateMachineComponent>(ComponentType::StateMachineComponentType)->SetStateNode(State::IDLE);
    }
}
