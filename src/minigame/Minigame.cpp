#include "Minigame.h"

#include <algorithm>
#include <sstream>
#include <minecraft/entity/Player.h>
#include <minecraft/entity/PlayerInventoryProxy.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/Dimension.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/net/Packet.h>
#include <minecraft/game/GameType.h>
#include "../util/PlayerHelper.h"
#include "../util/Log.h"
#include "MinigameDimension.h"
#include "../util/PlayerPosLimitHelper.h"

void Minigame::tick() {
    if (countdown > 0) {
        countdown--;
        if ((countdown % 20) == 0 && countdown > 0) {
            /*std::stringstream ss;
            ss << "Game starting in " << (countdown / 20) << " seconds";
            broadcast(ss.str());*/
            char buf[4096];
            snprintf(buf, sizeof(buf), "Game starting in %i seconds", countdown / 20);
            broadcast(buf);
        }
        if (countdown == 0) {
            broadcast("The game has started!");
            start();
        }
    }
}

void Minigame::start() {
    if (started)
        return;
    started = true;
    onStarted();
}

void Minigame::onStarted() {
    for (Player* player : players) {
        PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
        PlayerPosLimitHelper::clearPlayerLimit(playerData);

        player->abilities.setPlayerPermissions(PlayerPermissionLevel::MEMBER);
        player->abilities.setAbility(Abilities::INVULNERABLE, false);
        player->setPermissions(player->getCommandPermissionLevel()); // send the abilities to the client
    }
}

bool Minigame::addPlayer(Player* player) {
    if (started) {
        Log::info("Minigame", "[%s] Player can't join - game has already started");
        player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), TextPacket::createRaw("Error: Game has already started!"), player->getClientSubId());
        return false;
    }
    if (players.size() >= mapConfig.maxPlayers && availableSpawnPos.empty()) {
        Log::info("Minigame", "[%s] Player can't join - max player count reached");
        player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), TextPacket::createRaw("Error: Game is full!"), player->getClientSubId());
        return false;
    }
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
    if (playerData.currentMinigame != nullptr) {
        Log::info("Minigame", "[%s] Player can't join - already in another game");
        player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), TextPacket::createRaw("Error: You are already in another game!"), player->getClientSubId());
        return false;
    }
    playerData.currentMinigame = this;

    auto pos = playerSpawnPos[player] = availableSpawnPos[availableSpawnPos.size() - 1];
    availableSpawnPos.pop_back();

    broadcast("Join: " + player->getNameTag());
    players.push_back(player);

    MinigameDimension::sendPlayerToDimension(player, (int) dimension->id, {pos.x + 0.5f, pos.y + 2.f, pos.z + 0.5f});
    PlayerPosLimitHelper::setPlayerLimitBox(playerData, AABB({pos.x, pos.y, pos.z},
                                                             {pos.x + 1.f, pos.y + 4.f, pos.z + 1.f}),
                                            [pos](Vec3 const&) { return Vec3 {pos.x + 0.5f, pos.y + 2.f, pos.z + 0.5f}; });

    auto permissionLevel = player->getCommandPermissionLevel();
    player->abilities = Abilities(); // reset the abilities
    player->abilities.setPlayerPermissions(PlayerPermissionLevel::VIEWER);
    player->setPlayerGameType(GameType::SURVIVAL);
    player->abilities.setAbility(Abilities::INVULNERABLE, true);
    player->setPermissions(permissionLevel);

    player->getSupplies().clearInventory(-1);
    player->clearEquipment();
    player->sendInventory(true);

    if (players.size() >= mapConfig.tryGetMinPlayers && (countdown == -1 || countdown > TICKS_ENOUGH_PLAYERS_COUNTDOWN))
        countdown = TICKS_ENOUGH_PLAYERS_COUNTDOWN;
    if (players.size() >= mapConfig.maxPlayers && (countdown == -1 || countdown > TICKS_FULL_PLAYERS_COUNTDOWN))
        countdown = TICKS_FULL_PLAYERS_COUNTDOWN;
    return true;
}

void Minigame::removePlayer(Player* player) {
    if (playerSpawnPos.count(player) > 0) {
        availableSpawnPos.push_back(playerSpawnPos.at(player));
        playerSpawnPos.erase(player);
    }
    broadcast("Quit: " + player->getNameTag());
    players.erase(std::remove(players.begin(), players.end(), player), players.end());
    if (players.size() == mapConfig.maxPlayers - 1 && countdown != -1 && countdown < TICKS_FULL_PLAYERS_COUNTDOWN)
        countdown = TICKS_ENOUGH_PLAYERS_COUNTDOWN;
    if (players.size() == mapConfig.tryGetMinPlayers - 1 && countdown != -1 && countdown < TICKS_ENOUGH_PLAYERS_COUNTDOWN)
        countdown = TICKS_INITIAL_COUNTDOWN;
    if (players.size() < mapConfig.minPlayers && countdown != -1)
        countdown = -1;
}

void Minigame::broadcast(Packet const& packet) {
    for (Player* player : players)
        player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), packet, player->getClientSubId());
}

void Minigame::broadcast(std::string const& text) {
    Log::info("Minigame", "[%s] %s", name.c_str(), text.c_str());
    broadcast(TextPacket::createRaw(text));
}