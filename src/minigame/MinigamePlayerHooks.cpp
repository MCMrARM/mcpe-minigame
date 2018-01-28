#include "../statichook.h"

#include <cstdlib>
#include <minecraft/entity/Player.h>
#include <minecraft/entity/EntityDamageSource.h>
#include <minecraft/lang/I18n.h>
#include "../lobby/LobbyManager.h"
#include "Minigame.h"

TInstanceHook(void, _ZN6Player3dieERK18EntityDamageSource, Player, EntityDamageSource const& src) {
    //original(this, src);
    serializationSetHealth(20);
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(*this);
    LobbyManager::instance.sendPlayerToLobby(*this);
    if (playerData.currentMinigame != nullptr) {
        auto msg = src.getDeathMessage(getNameTag(), this);
        playerData.currentMinigame->broadcast("§c" + I18n::get(msg.message, msg.args));
        playerData.currentMinigame->removePlayer(this);
        playerData.currentMinigame = nullptr;
    }
}