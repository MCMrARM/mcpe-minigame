#pragma once

class Minigame;

struct PlayerData {

    /* MinigameManager */
    Minigame* currentMinigame;

    /* MinigameDimension */
    int tpTargetDimension = -1;

};