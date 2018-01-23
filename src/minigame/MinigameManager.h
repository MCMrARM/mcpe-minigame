#pragma once

#include <unordered_map>
#include <memory>
#include "Minigame.h"

class MinigameManager {

private:
    std::unordered_map<std::string, std::shared_ptr<Minigame>> minigames;

public:
    static MinigameManager instance;

    void addGame(std::shared_ptr<Minigame> minigame);
    void removeGame(std::shared_ptr<Minigame> minigame);

    std::shared_ptr<Minigame> getMinigame(std::string const& name);

    void tickMinigames();

};