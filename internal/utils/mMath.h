//
// Created by 张俏悦 on 2025/10/27.
//

#ifndef TESTMOBA_MMATH_H
#define TESTMOBA_MMATH_H
#include <box2d/math_functions.h>
#include <cmath>

constexpr float PI_F = 3.14159265358979323846f;

inline float DegToRad(float deg) {
    return deg * PI_F / 180.0f;
}

// 从弧度返回单位方向向量
inline b2Vec2 DirectionFromRadians(float rad) {
    return b2Vec2(std::cos(rad), std::sin(rad));
}

#endif //TESTMOBA_MMATH_H