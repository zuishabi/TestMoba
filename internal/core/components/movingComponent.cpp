//
// Created by 张俏悦 on 2025/10/20.
//

#include <box2d/box2d.h>
#include <box2d/id.h>
#include "components.h"
#include "../../worldManager.h"



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
    b2BodyId body = b2LoadBodyId(id);
    b2Vec2 vec = b2Body_GetLinearVelocity(body);
    if (!b2Body_IsValid(body)) {
        std::cout << id << "not valid" << std::endl;
    }
    if (vec.x != 0 || vec.y != 0) {
        moveSyncer->SetPos(b2Body_GetPosition(body));
    }

    int currentSpeed = OverrideSpeed == 0 ? speed->GetSpeed() : OverrideSpeed;
    b2Vec2 playerPosition = b2Body_GetPosition(body);
    // 阈值按速度和时间步计算，保证 fast moving 能在一帧内到达的不抖动
    float stopThreshold = std::max(0.01f, currentSpeed * fixed_dt * 1.5f);

    if (Distance(playerPosition, target) <= stopThreshold) {
        if (b2Body_GetLinearVelocity(body) != b2Vec2(0,0)) {
            // 贴合到目标并停止，避免小幅震荡
            b2Body_SetLinearVelocity(body, b2Vec2{0.0f, 0.0f});
        }
    } else {
        b2Vec2 direction = GetDirection(playerPosition, target);
        b2Body_SetLinearVelocity(body,b2Vec2(direction.x * currentSpeed,direction.y * currentSpeed));
    }
}


void MovingComponent::SetTargetDirection(b2Vec2 target) {
    this->target = target;
}


void MovingComponent::ProcessInput(b2Vec2 target) {
    if (CanMove) {
        SetTargetDirection(target);
        MovingSignal.emit(target);
    }
}
