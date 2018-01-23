#include "PlayerPosLimitHelper.h"

#include <minecraft/server/ServerNetworkHandler.h>
#include <minecraft/level/Level.h>
#include <minecraft/net/Packet.h>
#include <minecraft/net/PacketSender.h>
#include "../statichook.h"
#include "PlayerHelper.h"

TInstanceHook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK16MovePlayerPacket, ServerNetworkHandler,
              NetworkIdentifier const& nid, MovePlayerPacket const& packet) {
    Player* player = PlayerHelper::instance.findNetPlayer(nid, packet.playerSubIndex);
    if (player != nullptr) {
        PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
        if (playerData.hasPosLimitBox && !playerData.posLimitBox.contains(packet.pos)) {
            printf("Player tried to escape the limit box!\n");
            printf("Box: [%f %f %f] - [%f %f %f]\n", playerData.posLimitBox.min.x, playerData.posLimitBox.min.y, playerData.posLimitBox.min.z, playerData.posLimitBox.max.x, playerData.posLimitBox.max.y, playerData.posLimitBox.max.z);
            printf("Player pos: %f %f %f\n", packet.pos.x, packet.pos.y, packet.pos.z);

            MovePlayerPacket reply;
            reply.entityId = packet.entityId;
            reply.pos = playerData.posLimitBoxEscapeFunc(packet.pos);
            reply.rot = packet.rot;
            reply.headRot = packet.headRot;
            reply.mode = 0;
            reply.onGround = false;
            level->getPacketSender()->sendToClient(nid, reply, packet.playerSubIndex);
            return;
        }
    }
    original(this, nid, packet);
}