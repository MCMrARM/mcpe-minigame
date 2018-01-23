#pragma once

#include <functional>
#include <minecraft/math/Vec3.h>
#include <minecraft/math/AABB.h>

class Minigame;

struct PlayerData {

    /* MinigameManager */
    Minigame* currentMinigame = nullptr;

    /* MinigameDimension */
    int tpTargetDimension = -1;
    Vec3 tpTargetPos;

    /* PlayerPosLimitHelper */
    bool hasPosLimitBox = false;
    AABB posLimitBox;
    std::function<Vec3 (Vec3 const&)> posLimitBoxEscapeFunc;

};