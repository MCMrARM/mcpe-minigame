#pragma once

#include <vector>
#include <minecraft/level/BlockPos.h>

namespace Json { class Value; }

struct MapConfig {

    int minPlayers;
    int maxPlayers;
    int tryGetMinPlayers;
    std::vector<BlockPos> spawnPositions;

    void loadFromJSON(Json::Value const& val);

};