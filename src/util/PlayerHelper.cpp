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
    Log::info("PlayerHelper", "Player joined: %s", player.getNameTag().c_str());
    PlayerData& data = playerData[&player];
    for (PlayerEventListener* listener : listeners)
        listener->onPlayerJoined(player);
}

void PlayerHelper::onPlayerLeft(Player& player) {
    Log::info("PlayerHelper", "Player left: %s", player.getNameTag().c_str());
    for (PlayerEventListener* listener : listeners)
        listener->onPlayerLeft(player);
}

void PlayerHelper::onPlayerConnected(NetworkIdentifier const& nid, Player& player) {
    Log::trace("PlayerHelper", "Player connected from %s", nid.toString().c_str());
    PlayerData& data = playerData[&player];
    playerFastLookupTable[nid].push_back(&player);
}

void PlayerHelper::onPlayerDisconnected(NetworkIdentifier const& nid) {
    playerFastLookupTable.erase(nid);
    Log::trace("PlayerHelper", "Player disconnected (%s)", nid.toString().c_str());
}

void PlayerHelper::onPlayerDestroyed(Player& player) {
    playerData.erase(&player);
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
    // original(this, player, nid); - do not call original so there are no join messages
    PlayerHelper::instance.onPlayerJoined(player);
}

TInstanceHook(void, _ZN20ServerNetworkHandler13_onPlayerLeftEP12ServerPlayer,
              ServerNetworkHandler, ServerPlayer* player) {
    // original(this, player); - do not call original so there are no quit messages
    player->disconnect();
    player->remove();

    if (player != nullptr)
        PlayerHelper::instance.onPlayerLeft(*player);
}

TInstanceHook(void, _ZN20ServerNetworkHandler12onDisconnectERK17NetworkIdentifierRKSsb,
              ServerNetworkHandler, NetworkIdentifier const& nid, std::string const& str, bool b) {
    original(this, nid, str, b);
    PlayerHelper::instance.onPlayerDisconnected(nid);
}

TInstanceHook(void, _ZN6PlayerD2Ev, Player) {
    PlayerHelper::instance.onPlayerDestroyed(*this);
    original(this);
}