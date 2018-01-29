#pragma once

#include <memory>
#include "../util/PlayerHelper.h"

class Dimension;
class Player;
class Level;

class LobbyManager : public PlayerEventListener {

private:
    Dimension* dimension;

public:
    static LobbyManager instance;

    void setDimension(Dimension* dimension);

    void onPlayerArrivedInLobby(Player& player);

    void sendPlayerToLobby(Player& player);

    void onPlayerMoved(Player& player, Vec3 const& pos);

    virtual void onPlayerJoined(Player& player) {
        onPlayerArrivedInLobby(player);
    }

};