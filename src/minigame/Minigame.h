#pragma once

#include <vector>
#include <string>
#include <map>
#include <minecraft/level/BlockPos.h>
#include <memory>
#include "MapConfig.h"

class Dimension;
class Player;
class Packet;
class MinigameManager;

class Minigame : public std::enable_shared_from_this<Minigame> {

private:
    MinigameManager* manager;
    std::string name;
    Dimension* dimension;
    int countdown = -1;
    bool started = false;
    std::vector<Player*> players;
    std::vector<BlockPos> availableSpawnPos;
    std::map<Player*, BlockPos> playerSpawnPos;
    MapConfig mapConfig;

protected:
    inline Dimension* getDimension() { return dimension; }
    inline MapConfig& getMapConfig() { return mapConfig; }

    void destroy();

public:
    static const int TICKS_INITIAL_COUNTDOWN = 20 * 60;
    static const int TICKS_ENOUGH_PLAYERS_COUNTDOWN = 20 * 15;
    static const int TICKS_FULL_PLAYERS_COUNTDOWN = 20 * 5;

    Minigame(MinigameManager* mgr, std::string name, Dimension* dimension, MapConfig mapConfig) :
            name(std::move(name)), dimension(dimension), manager(mgr), mapConfig(mapConfig) {
        availableSpawnPos = mapConfig.spawnPositions;
    }

    inline std::string const getName() const { return name; }

    void start();

    void startCountdown(int countdown = TICKS_INITIAL_COUNTDOWN) { this->countdown = countdown; }
    int getCountdown() const { return countdown; }

    virtual void onStarted();

    virtual void tick();

    virtual bool isJoinable() { return !started && players.size() < mapConfig.maxPlayers; }
    virtual bool addPlayer(Player* player);
    virtual void removePlayer(Player* player);

    virtual bool checkWinner();

    void broadcast(Packet const& packet);
    void broadcast(std::string const& text);

};