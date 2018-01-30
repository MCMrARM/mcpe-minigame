#include "MapConfig.h"

#include <json/json.h>

void MapConfig::loadFromJSON(Json::Value const& val) {
    minPlayers = val["minPlayers"].asInt();
    maxPlayers = val["maxPlayers"].asInt();
    tryGetMinPlayers = val["tryGetMinPlayers"].asInt();
    for (auto const& pos : val["spawnPositions"]) {
        if (pos.isArray())
            spawnPositions.push_back({ pos[0].asInt(), pos[1].asInt(), pos[2].asInt() });
        else
            spawnPositions.push_back({ pos["x"].asInt(), pos["y"].asInt(), pos["z"].asInt() });
    }
}