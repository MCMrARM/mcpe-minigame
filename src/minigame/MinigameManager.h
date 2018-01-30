#pragma once

#include <unordered_map>
#include <memory>
#include "Minigame.h"
#include "../util/PlayerHelper.h"

class Level;
class Dimension;

class MinigameManager : public PlayerEventListener {

private:
    std::unordered_map<std::string, std::shared_ptr<Minigame>> minigames;
    std::unordered_map<std::string, int> prefixIndex;

    Dimension* createGameDimension(Level* level, std::string const& map);
    MapConfig loadMapConfig(std::string const& map);

public:
    static MinigameManager instance;

    void addGame(std::shared_ptr<Minigame> minigame);
    void removeGame(std::shared_ptr<Minigame> minigame);

    std::unordered_map<std::string, std::shared_ptr<Minigame>> const& getGames() const { return minigames; }

    template <typename T>
    std::shared_ptr<T> createGame(std::string const& name, Level* level, std::string const& map) {
        Dimension* dimension = createGameDimension(level, map);
        MapConfig mapConfig = loadMapConfig(map);
        std::shared_ptr<T> minigame(new T(this, name, dimension, mapConfig));
        addGame(minigame);
        return minigame;
    }

    std::shared_ptr<Minigame> getMinigame(std::string const& name);

    std::string getMinigameNameForPrefix(std::string const& prefix);

    void tickMinigames();

    virtual void onPlayerLeft(Player& player);

};