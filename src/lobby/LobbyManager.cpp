#include "LobbyManager.h"

#include <minecraft/entity/Player.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/LevelData.h>
#include "../minigame/MinigameDimension.h"
#include "../statichook.h"

LobbyManager LobbyManager::instance;

void LobbyManager::setDimension(Dimension* dimension) {
    this->dimension = dimension;
    dimension->level->getDefaultAbilities().setPlayerPermissions(PlayerPermissionLevel::VIEWER);
    dimension->level->getDefaultAbilities().setAbility(Abilities::MAYFLY, false);
    dimension->level->getDefaultAbilities().setAbility(Abilities::WALKSPEED, 0.15f); // default is 0.05f
    dimension->level->getLevelData()->setGameType(GameType::CREATIVE);
}

void LobbyManager::sendPlayerToLobby(Player& player) {
    MinigameDimension::sendPlayerToDimension(&player, (int) dimension->id, player.getLevel()->getSharedSpawnPos());
    onPlayerArrivedInLobby(player);
}

void LobbyManager::onPlayerArrivedInLobby(Player& player) {
    auto cmdPermissionLevel = player.getCommandPermissionLevel();
    player.setPlayerGameType(dimension->level->getLevelData()->getGameType());
    player.abilities = dimension->level->getDefaultAbilities();
    player.abilities.setAbility(Abilities::MAYFLY, false);
    player.setPermissions(cmdPermissionLevel); // revert the cmd permission level & send the abilities to the client
}