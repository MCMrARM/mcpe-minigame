#include <minecraft/net/Packet.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/entity/Player.h>
#include <minecraft/level/Level.h>
#include "../statichook.h"
#include "../util/PlayerHelper.h"
#include "Minigame.h"

TClasslessInstanceHook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK10TextPacket,
                       NetworkIdentifier const& nid, TextPacket const& packet) {
    Player* player = PlayerHelper::instance.findNetPlayer(nid, packet.playerSubIndex);
    if (player == nullptr)
        return;
    TextPacket broadcastPacket {(TextPacketType) 1, /*packet.sender*/ player->getNameTag(), packet.message, {}, false, packet.xuid};
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
    if (playerData.currentMinigame != nullptr) {
        playerData.currentMinigame->broadcast(broadcastPacket);
    } else {
        for (auto const& otherPlayer : player->getLevel()->getUsers()) {
            PlayerData& otherPlayerData = PlayerHelper::instance.getPlayerData(*otherPlayer);
            if (otherPlayerData.currentMinigame == nullptr)
                player->getLevel()->getPacketSender()->sendToClient(otherPlayer->getClientId(), broadcastPacket,
                                                                    otherPlayer->getClientSubId());
        }
    }
}