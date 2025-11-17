//
// Created by 张俏悦 on 2025/10/30.
//

#include "skill.h"
#include "../buffs/buff.h"
#include "../components/components.h"


void meleeSpin::Update() {
    if (activated) {
        // 首先检查是否过了一秒
        if (startTimer.start && startTimer.IsExpired()) {
            // 结算伤害,然后启动后摇计时器
            int damage = 10 * (1+1.0/static_cast<double>(attribute->GeAttribute().strength));

            int capacity = b2Shape_GetSensorCapacity( shape );
            std::vector<b2ShapeId> overlaps;
            overlaps.resize( capacity );
            // Now get all overlaps and record the actual count
            int count = b2Shape_GetSensorOverlaps( shape, overlaps.data(), capacity );
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
                    }

                    auto buff = targetManager->GetComponent<BuffComponent>(ComponentType::BuffComponentType);
                    if (buff != nullptr) {
                        std::shared_ptr<Buff> ptr = std::make_shared<Fire>(from, _id);
                        buff->AddBuff(ptr);
                    }
                }
            }
            endTimer.Reset(std::chrono::duration<double>(1));
        }else if (endTimer.start && endTimer.IsExpired()) {
            // 进行后摇动画的处理
            auto stateMachine = GameWorld::GetComponentManager(from)->GetComponent<StateMachineComponent>(ComponentType::StateMachineComponentType);
            stateMachine->SetStateNode(State::MOVING);
            activated = false;
        }
    }
}


void meleeSpin::Execute(ExecuteSkillInfo info) {
    activated = true;
    startTimer.Reset(std::chrono::seconds(1));
    endTimer.Stop();
    skillInfoSyncer->Execute();
    skillComponent->UseMoveSkill.emit(this);
}
