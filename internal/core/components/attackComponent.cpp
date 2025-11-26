//
// Created by 张俏悦 on 2025/10/20.
//


#include <box2d/box2d.h>
#include <box2d/id.h>
#include "components.h"
#include "../../worldManager.h"
#include "../../utils/memoryPool.h"


class BulletComponent:public Component{
public:
    BulletComponent(uint64_t target,uint64_t from,uint64_t id,b2ShapeId shapeID):Component(ComponentType::BulletComponentType,id),
        _target(target),_from(from),_shapeID(shapeID),objectSyncer(std::make_shared<ObjectSyncer>(id)) {
        auto body = b2LoadBodyId(id);
        b2Vec2 pos = b2Body_GetPosition(body);
        objectSyncer->SetInfo(ObjectInfo{2,id,pos.x,pos.y,1});
        manager->SyncerManager->AddSyncer(objectSyncer);
    }

    // static void* operator new(std::size_t n);
    //
    // static void operator delete(void* p);

    static void* operator new(size_t size) {
        return memory_pool->allocate(size);
    }

    static void operator delete(void* ptr, size_t size) {
        memory_pool->deallocate(ptr, size);
    }
public:
    void Update() override {
        auto t = GameWorld::GetComponentManager(_target);
        if (t == nullptr) {
            manager->destroyed = true;
            return;
        }
        // 将移动方向转为目标位置
        b2Vec2 targetPos = b2Body_GetPosition(b2LoadBodyId(_target));
        b2Vec2 selfPos = b2Body_GetPosition(b2LoadBodyId(id));

        b2Vec2 direction = {
            targetPos.x - selfPos.x,
            targetPos.y - selfPos.y
        };

        // 检查距离，避免除零错误
        float distanceSquared = direction.x * direction.x + direction.y * direction.y;
        if (distanceSquared > 0.0001f) { // 设置一个小的阈值
            float distance = sqrtf(distanceSquared);
            direction.x /= distance;
            direction.y /= distance;

            // 设置速度
            b2Vec2 velocity = {direction.x * 1000.0f, direction.y * 1000.0f};
            b2Body_SetLinearVelocity(b2LoadBodyId(id), velocity);
        } else {
            // 位置非常接近，停止移动或保持原状
            b2Vec2 zero = {0.0f, 0.0f};
            b2Body_SetLinearVelocity(b2LoadBodyId(id), zero);
        }


        // 获取碰撞到的敌人
        int capacity = b2Shape_GetSensorCapacity( _shapeID );
        std::vector<b2ShapeId> overlaps;
        overlaps.resize( capacity );
        int damage = 10;

        int count = b2Shape_GetSensorOverlaps( _shapeID, overlaps.data(), capacity );
        overlaps.resize( count );

        for ( int j = 0; j < count; ++j )
        {
            b2ShapeId visitorId = overlaps[j];
            if ( b2Shape_IsValid( visitorId ) == false )
            {
                continue;
            }
            b2BodyId body = b2Shape_GetBody(visitorId);
            uint64_t _id = b2StoreBodyId(body);
            if (_id == _target) {
                ComponentManager* targetManager = GameWorld::GetComponentManager(_id);
                auto hit = targetManager->GetComponent<HitComponent>(ComponentType::HitComponentType);
                if (hit != nullptr) {
                    hit->Hit(_id,{damage});
                    manager->destroyed = true;
                    break;
                }
            }
        }
    }
private:
    uint64_t _target;
    uint64_t _from;
    b2ShapeId _shapeID;
    std::shared_ptr<ObjectSyncer> objectSyncer;
    static std::pmr::memory_resource* memory_pool;
};


std::pmr::memory_resource* BulletComponent::memory_pool = std::pmr::get_default_resource();

// 子弹的内存池
//static FixedSizePool bulletPool(sizeof(BulletComponent));


// void *BulletComponent::operator new(std::size_t n) {
//     return bulletPool.allocate();
// }
//
//
// void BulletComponent::operator delete(void *p) {
//     bulletPool.deallocate(p);
// }


void AttackComponent::Update() {
    if (isShooting && std::chrono::steady_clock::now() >= nextPoint) {
        ComponentManager* target = GameWorld::GetComponentManager(targetID);
        auto hit = target->GetComponent<HitComponent>(ComponentType::HitComponentType);
        if (hit != nullptr) {
            if (isBullet) {
                // 创建子弹
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_kinematicBody;
                bodyDef.isBullet = true;
                bodyDef.position = b2Body_GetPosition(b2LoadBodyId(id));
                b2BodyId myBodyId = b2CreateBody(GameWorld::World,&bodyDef);

                b2Circle circle;
                circle.radius = 1;
                circle.center = {0,0};

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.isSensor = true;
                shapeDef.density = 1.0f;
                shapeDef.material.friction = 0.3f;
                shapeDef.material.restitution = 0;
                shapeDef.filter.categoryBits = CATEGORY_BULLET;
                shapeDef.filter.maskBits = MASK_BULLET;
                shapeDef.enableSensorEvents = true;
                b2ShapeId bodyShape = b2CreateCircleShape(myBodyId, &shapeDef, &circle);

                auto id = b2StoreBodyId(myBodyId);
                auto manager = GameWorld::StoreComponentManager(id,std::make_unique<ComponentManager>(id,ManagerType::Bullet));

                manager->AddComponent<BulletComponent>(targetID, manager->id, id, bodyShape);

                manager->AddComponent<MovingComponent>(id,bodyDef.position);
            }else {
                hit->Hit(targetID,{10});
            }
            manager->GetComponent<StateMachineComponent>(ComponentType::StateMachineComponentType)->
                    SetStateNode(State::IDLE);
        }
        isShooting = false;
    }
}


void AttackComponent::Attack(uint64_t target) {
    b2BodyId targetBody = b2LoadBodyId(target);
    if (Enable && b2Body_IsValid(targetBody)) {
        targetID = target;
        ComponentManager* manager = GameWorld::GetComponentManager(targetID);
        auto attackSpeed = manager->GetComponent<AttributeComponent>(ComponentType::AttributeComponentType);
        nextPoint = std::chrono::steady_clock::now() + std::chrono::duration<double>(attackSpeed->GetAttackSpeed());
        isShooting = true;

        startAttackSignal.emit(target);
    }
}
