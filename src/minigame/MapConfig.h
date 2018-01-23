#pragma once

#include <vector>
#include <minecraft/level/BlockPos.h>

struct MapConfig {

    int minPlayers;
    int maxPlayers;
    int tryGetMinPlayers;
    std::vector<BlockPos> spawnPositions;

};