#include "MinigameManager.h"

#include <minecraft/level/Level.h>
#include "../statichook.h"
#include "MinigameDimension.h"

MinigameManager MinigameManager::instance;

void MinigameManager::addGame(std::shared_ptr<Minigame> minigame) {
    if (minigames.count(minigame->getName()) > 0)
        throw std::runtime_error("A minigame with this name already exists");
    minigames[minigame->getName()] = minigame;
}

void MinigameManager::removeGame(std::shared_ptr<Minigame> minigame) {
    if (minigames.count(minigame->getName()) == 0 || minigames.at(minigame->getName()) != minigame)
        throw std::runtime_error("This minigame instance is not added to this manager");
    minigames.erase(minigame->getName());
}

std::shared_ptr<Minigame> MinigameManager::getMinigame(std::string const& name) {
    if (minigames.count(name) == 0)
        return nullptr;
    return minigames.at(name);
}

void MinigameManager::tickMinigames() {
    for (auto const& minigame : minigames)
        minigame.second->tick();
}

void MinigameManager::onPlayerLeft(Player& player) {
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(player);
    if (playerData.currentMinigame != nullptr)
        playerData.currentMinigame->removePlayer(&player);
}

std::string MinigameManager::getMinigameNameForPrefix(std::string const& prefix) {
    int index = ++prefixIndex[prefix];
    char buf[32];
    snprintf(buf, sizeof(buf), "%s-%i", prefix.c_str(), index);
    return std::string(buf);
}

TInstanceHook(void, _ZN5Level4tickEv, Level) {
    original(this);
    MinigameDimension::deleteUnusedDimensions(*this);
    MinigameManager::instance.tickMinigames();
}