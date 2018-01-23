#pragma once

#include "PlayerData.h"

class PlayerPosLimitHelper {

public:
    static void setPlayerLimitBox(PlayerData& data, AABB const& box, std::function<Vec3 (Vec3 const&)> escapeFunc) {
        data.hasPosLimitBox = true;
        data.posLimitBox = box;
        data.posLimitBoxEscapeFunc = escapeFunc;
    }

    static void clearPlayerLimit(PlayerData& data) {
        data.hasPosLimitBox = false;
        data.posLimitBoxEscapeFunc = nullptr;
    }

};