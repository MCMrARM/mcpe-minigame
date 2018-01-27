#include "LobbyManager.h"

#include <minecraft/entity/Player.h>
#include <minecraft/entity/PlayerInventoryProxy.h>
#include <minecraft/level/BlockPos.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/LevelData.h>
#include <minecraft/level/BlockSource.h>
#include "../minigame/MinigameDimension.h"
#include "../statichook.h"

LobbyManager LobbyManager::instance;

void LobbyManager::setDimension(Dimension* dimension) {
    this->dimension = dimension;
    dimension->level->getDefaultAbilities().setPlayerPermissions(PlayerPermissionLevel::VIEWER);
    dimension->level->getDefaultAbilities().setAbility(Abilities::INVULNERABLE, true);
    dimension->level->getDefaultAbilities().setAbility(Abilities::WALKSPEED, 0.15f); // default is 0.05f
    dimension->level->getLevelData()->setGameType(GameType::SURVIVAL);
}

void LobbyManager::sendPlayerToLobby(Player& player) {
    auto pos = player.getLevel()->getSharedSpawnPos();
    MinigameDimension::sendPlayerToDimension(&player, (int) dimension->id, {pos.x, pos.y + 2.f, pos.z});
    onPlayerArrivedInLobby(player);
}

void LobbyManager::onPlayerArrivedInLobby(Player& player) {
    auto cmdPermissionLevel = player.getCommandPermissionLevel();
    player.abilities = dimension->level->getDefaultAbilities();
    player.setPlayerGameType(dimension->level->getLevelData()->getGameType());
    player.abilities.setAbility(Abilities::INVULNERABLE, true);
    player.setPermissions(cmdPermissionLevel); // revert the cmd permission level & send the abilities to the client
    player.getSupplies().clearInventory(-1);
    player.clearEquipment();
    player.sendInventory(true);
}