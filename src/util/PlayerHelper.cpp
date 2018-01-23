#include "PlayerHelper.h"

#include <minecraft/server/ServerNetworkHandler.h>
#include <minecraft/entity/Player.h>
#include "../statichook.h"
#include "Log.h"

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

void PlayerHelper::onPlayerConnected(NetworkIdentifier const& nid, Player& player) {
    Log::trace("PlayerHelper", "Player connected from %s", nid.toString().c_str());
    playerFastLookupTable[nid].push_back(&player);
}

void PlayerHelper::onPlayerDisconnected(NetworkIdentifier const& nid) {
    playerFastLookupTable.erase(nid);
    Log::trace("PlayerHelper", "Player disconnected (%s)", nid.toString().c_str());
}

Player* PlayerHelper::findNetPlayer(NetworkIdentifier const& nid, unsigned char subId) {
    if (playerFastLookupTable.count(nid) < 0)
        return nullptr;
    auto const& players = playerFastLookupTable.at(nid);
    for (Player* player : players) {
        if (player->getClientSubId() == subId)
            return player;
    }
    return nullptr;
}

class ConnectionRequest;
TInstanceHook(ServerPlayer*, _ZN20ServerNetworkHandler16_createNewPlayerERK17NetworkIdentifierRK17ConnectionRequest,
              ServerNetworkHandler, NetworkIdentifier const& nid, ConnectionRequest const& req) {
    ServerPlayer* ret = original(this, nid, req);
    PlayerHelper::instance.onPlayerConnected(nid, *ret);
    return ret;
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

TInstanceHook(void, _ZN20ServerNetworkHandler12onDisconnectERK17NetworkIdentifierRKSsb,
              ServerNetworkHandler, NetworkIdentifier const& nid, std::string const& str, bool b) {
    original(this, nid, str, b);
    PlayerHelper::instance.onPlayerDisconnected(nid);
}