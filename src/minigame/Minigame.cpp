#include "Minigame.h"

#include <algorithm>
#include <sstream>
#include <minecraft/entity/Player.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/Dimension.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/net/Packet.h>
#include "MinigameDimension.h"
#include "../util/Log.h"

void Minigame::tick() {
    if (countdown > 0) {
        countdown--;
        if ((countdown % 20) == 0 && countdown > 0) {
            std::stringstream ss;
            ss << "Game starting in " << (countdown / 20) << " seconds";
            broadcast(ss.str());
        }
        if (countdown == 0)
            start();
    }
}

void Minigame::start() {
    if (started)
        return;
    started = true;
    onStarted();
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
    auto pos = playerSpawnPos[player] = availableSpawnPos[availableSpawnPos.size() - 1];
    availableSpawnPos.pop_back();

    broadcast("Join: " + player->getFormattedNameTag());
    players.push_back(player);
    // MinigameDimension::sendPlayerToDimension(player, (int) dimension->id, {pos.x + 0.5f, pos.y, pos.z + 0.5f});
    return true;
}

void Minigame::removePlayer(Player* player) {
    if (playerSpawnPos.count(player) > 0) {
        availableSpawnPos.push_back(playerSpawnPos.at(player));
        playerSpawnPos.erase(player);
    }
    broadcast("Quit: " + player->getFormattedNameTag());
    players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

void Minigame::broadcast(Packet const& packet) {
    for (Player* player : players)
        player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), packet, player->getClientSubId());
}

void Minigame::broadcast(std::string const& text) {
    Log::info("Minigame", "[%s] %s", name.c_str(), text.c_str());
    broadcast(TextPacket::createRaw(text));
}