#include <minecraft/net/Packet.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/entity/Player.h>
#include <minecraft/level/Level.h>
#include "../statichook.h"
#include "../util/PlayerHelper.h"
#include "Minigame.h"
#include "../util/Log.h"

TClasslessInstanceHook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK10TextPacket,
                       NetworkIdentifier const& nid, TextPacket const& packet) {
    Player* player = PlayerHelper::instance.findNetPlayer(nid, packet.playerSubIndex);
    if (player == nullptr)
        return;
    std::string senderName = /*packet.sender*/ player->getNameTag();
    if (senderName == "MrARMDev")
        senderName = "§2@§a" + senderName + "§r";
    TextPacket broadcastPacket {(TextPacketType) 1, senderName, packet.message, {}, false, packet.xuid};
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
    const char* gameName = "Lobby";
    if (playerData.currentMinigame != nullptr) {
        gameName = playerData.currentMinigame->getName().c_str();
        playerData.currentMinigame->broadcast(broadcastPacket);
    } else {
        for (auto const& otherPlayer : player->getLevel()->getUsers()) {
            PlayerData& otherPlayerData = PlayerHelper::instance.getPlayerData(*otherPlayer);
            if (otherPlayerData.currentMinigame == nullptr)
                player->getLevel()->getPacketSender()->sendToClient(otherPlayer->getClientId(), broadcastPacket,
                                                                    otherPlayer->getClientSubId());
        }
    }
    Log::info("Chat", "[%s] <%s> %s", gameName, player->getNameTag().c_str(), packet.message.c_str());
}