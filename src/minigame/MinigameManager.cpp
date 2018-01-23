#include "MinigameManager.h"

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