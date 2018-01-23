#include "PlayerHelper.h"

#include <minecraft/server/ServerNetworkHandler.h>
#include <minecraft/entity/Player.h>
#include "../statichook.h"

PlayerHelper PlayerHelper::instance;

class NetworkIdentifier;

PlayerEventListener::PlayerEventListener() {
    PlayerHelper::instance.registerListener(this);
}

PlayerEventListener::~PlayerEventListener() {
    PlayerHelper::instance.unregisterListener(this);
}

void PlayerHelper::onPlayerJoined(Player& player) {
    PlayerData& data = playerData[&player];
    for (PlayerEventListener* listener : listeners)
        listener->onPlayerJoined(player);
}

void PlayerHelper::onPlayerLeft(Player& player) {
    for (PlayerEventListener* listener : listeners)
        listener->onPlayerLeft(player);
    playerData.erase(&player);
}

TInstanceHook(void, _ZN20ServerNetworkHandler24onReady_ClientGenerationER6PlayerRK17NetworkIdentifier,
              ServerNetworkHandler, Player& player, NetworkIdentifier const& nid) {
    original(this, player, nid);
    PlayerHelper::instance.onPlayerJoined(player);
}

TInstanceHook(void, _ZN20ServerNetworkHandler13_onPlayerLeftEP12ServerPlayer,
              ServerNetworkHandler, ServerPlayer* player) {
    original(this, player);
    if (player != nullptr)
        PlayerHelper::instance.onPlayerLeft(*player);
}
