#pragma once

#include <unordered_set>
#include <unordered_map>
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

    void onPlayerJoined(Player& player);
    void onPlayerLeft(Player& player);

};