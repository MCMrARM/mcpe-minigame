#pragma once

#include <unordered_map>
#include <memory>
#include "Minigame.h"
#include "../util/PlayerHelper.h"

class MinigameManager : public PlayerEventListener {

private:
    std::unordered_map<std::string, std::shared_ptr<Minigame>> minigames;
    std::unordered_map<std::string, int> prefixIndex;

public:
    static MinigameManager instance;

    void addGame(std::shared_ptr<Minigame> minigame);
    void removeGame(std::shared_ptr<Minigame> minigame);

    std::shared_ptr<Minigame> getMinigame(std::string const& name);

    std::string getMinigameNameForPrefix(std::string const& prefix);

    void tickMinigames();

    virtual void onPlayerLeft(Player& player);

};