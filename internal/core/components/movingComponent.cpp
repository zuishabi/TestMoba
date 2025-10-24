//
// Created by 张俏悦 on 2025/10/20.
//

#include "components.h"


const float fixed_dt = 1.0f / 30.0f;


b2Vec2 GetDirection(const b2Vec2& from, const b2Vec2& to) {
    b2Vec2 v = to - from;
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 1e-6f) return b2Vec2(0.0f, 0.0f);
    return b2Vec2(v.x / len, v.y / len);
}


inline float Distance(const b2Vec2& a, const b2Vec2& b) {
    b2Vec2 v = b - a;
    return std::hypot(v.x, v.y); // 或者： return std::sqrt(v.x*v.x + v.y*v.y);
}


void MovingComponent::Update() {
    moveSyncer->SetPos(b2Body_GetPosition(BodyID));
    if (CanMove) {
        b2Vec2 playerPosition = b2Body_GetPosition(BodyID);
        // 阈值按速度和时间步计算，保证 fast moving 能在一帧内到达的不抖动
        float stopThreshold = std::max(0.01f, Speed * fixed_dt * 1.5f);

        if (Distance(playerPosition, target) <= stopThreshold) {
            // 贴合到目标并停止，避免小幅震荡
            b2Body_SetLinearVelocity(BodyID, b2Vec2{0.0f, 0.0f});
        } else {
            b2Vec2 direction = GetDirection(playerPosition, target);
            b2Body_SetLinearVelocity(BodyID,b2Vec2(direction.x * Speed,direction.y * Speed));
        }
    }
}


void MovingComponent::SetTargetDirection(b2Vec2 target) {
    this->target = target;
    auto attack = Manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
    attack->Interrupt();
}


void MovingComponent::Interrupt() {
    this->target = b2Body_GetPosition(BodyID);
}
