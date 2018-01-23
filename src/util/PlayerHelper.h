#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <minecraft/net/NetworkIdentifier.h>
#include "PlayerData.h"

class Player;

class PlayerEventListener {

public:
    PlayerEventListener();

    virtual ~PlayerEventListener();
    virtual void onPlayerJoined(Player& player) { }
    virtual void onPlayerLeft(Player& player) { }

};

class PlayerHelper {

private:
    friend class PlayerEventListener;

    std::unordered_map<Player*, PlayerData> playerData;
    std::unordered_map<NetworkIdentifier, std::vector<Player*>> playerFastLookupTable;
    std::unordered_set<PlayerEventListener*> listeners;

    void registerListener(PlayerEventListener* listener) {
        listeners.insert(listener);
    }
    void unregisterListener(PlayerEventListener* listener) {
        listeners.erase(listener);
    }

public:
    static PlayerHelper instance;

    PlayerData& getPlayerData(Player& player) {
        return playerData.at(&player);
    }

    Player* findNetPlayer(NetworkIdentifier const& nid, unsigned char subId);

    void onPlayerJoined(Player& player);
    void onPlayerLeft(Player& player);

    void onPlayerConnected(NetworkIdentifier const& nid, Player& player);
    void onPlayerDisconnected(NetworkIdentifier const& nid);

};