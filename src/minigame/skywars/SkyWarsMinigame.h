#pragma once

#include "../Minigame.h"

class SkyWarsMinigame : public Minigame {

public:
    SkyWarsMinigame(MinigameManager* mgr, std::string name, Dimension* dimension, MapConfig mapConfig) : Minigame(mgr, name, dimension, mapConfig) {}

};