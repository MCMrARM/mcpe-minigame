#pragma once

#include <string>
#include <minecraft/command/CommandOrigin.h>
#include <minecraft/command/CommandOutput.h>
#include "../MinigameManager.h"
#include "../../util/PlayerHelper.h"

class MinigameCommandHelper {

public:
    static Minigame* findMinigame(std::string const& name, CommandOrigin const& origin, CommandOutput& outp) {
        if (name.empty() && origin.getOriginType() == CommandOriginType::PLAYER) {
            PlayerData& playerData = PlayerHelper::instance.getPlayerData(*((Player*) origin.getEntity()));
            if (playerData.currentMinigame != nullptr)
                return playerData.currentMinigame;
        }
        Minigame* minigame = MinigameManager::instance.getMinigame(name).get();
        if (minigame == nullptr) {
            outp.error("Can't find minigame with the specified name");
            return nullptr;
        }
        return minigame;
    }

};