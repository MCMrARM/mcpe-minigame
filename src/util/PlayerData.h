#pragma once

#include <minecraft/math/Vec3.h>

class Minigame;

struct PlayerData {

    /* MinigameManager */
    Minigame* currentMinigame = nullptr;

    /* MinigameDimension */
    int tpTargetDimension = -1;
    Vec3 tpTargetPos;

};