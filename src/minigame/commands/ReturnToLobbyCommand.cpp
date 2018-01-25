#include "ReturnToLobbyCommand.h"

#include <minecraft/command/CommandOrigin.h>
#include <minecraft/command/CommandParameterData.h>
#include "../../lobby/LobbyManager.h"
#include "../Minigame.h"

void ReturnToLobbyCommand::setup(CommandRegistry& registry) {
    registry.registerCommand("lobby", "Returns to the lobby", (CommandPermissionLevel) 0, (CommandFlag) 0,
                             (CommandFlag) 0);
    registry.registerOverload<ReturnToLobbyCommand>("lobby", CommandVersion(1, INT_MAX));
}

void ReturnToLobbyCommand::execute(CommandOrigin const& origin, CommandOutput& outp) {
    if (origin.getOriginType() != CommandOriginType::PLAYER)
        return;
    Player& player = *((Player*) origin.getEntity());
    PlayerData& data = PlayerHelper::instance.getPlayerData(player);
    if (data.currentMinigame != nullptr) {
        data.currentMinigame->removePlayer(&player);
        data.currentMinigame = nullptr;
    }
    LobbyManager::instance.sendPlayerToLobby(player);
}