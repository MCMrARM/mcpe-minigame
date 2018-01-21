#include "MinigameDimension.h"

#include <minecraft/level/BiomeSource.h>
#include <minecraft/level/LevelStorage.h>
#include <minecraft/level/Level.h>
#include <minecraft/entity/Player.h>
#include <minecraft/net/Packet.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/net/NetworkIdentifier.h>
#include <minecraft/server/ServerNetworkHandler.h>
#include <cstring>
#include "statichook.h"

std::vector<MinigameDimension::DefinedDimension> MinigameDimension::dimensions;
std::map<Player*, int> MinigameDimension::sendPlayerToDimension;

MinigameDimension::MinigameDimension(Level& level, DimensionId dimensionId, short maxHeight) :
        Dimension(level, (DimensionId) 3, maxHeight) {
    id = dimensionId;
    dimensionName = "MinigameDimension";
    maxBrightness = 0xff;
    unk_76 = true;
    unk_14 = 0x3F;
    biomeSource = ThreadLocal<BiomeSource>([]() { return std::unique_ptr<BiomeSource>(new BiomeSource(7, (GeneratorType) 1)); });
}

int MinigameDimension::defineDimension(std::unique_ptr<LevelStorage> storage) {
    dimensions.push_back({ std::move(storage) });
    return dimensions.size() - 1 + 4;
}

std::unique_ptr<Dimension> MinigameDimension::createDimension(Level& level, int index) {
    return std::unique_ptr<Dimension>(new MinigameDimension(level, (DimensionId) index, (short) 0x100));
}

void MinigameDimension::sendPlayer(Player* player, int dimension) {
    sendPlayerToDimension[player] = dimension;

    ChangeDimensionPacket pkDimen;
    pkDimen.dimension = 1;
    pkDimen.pos = {1, 1, 1};
    pkDimen.b = false;
    player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), pkDimen, player->getClientSubId());

    short hmap[16*16];
    memset(hmap, 0, sizeof(hmap));
    std::string emptyChunkData = std::string("\0", 1) + std::string((char*)hmap, sizeof(hmap)) + std::string("\0\0", 2);
    for (int x = -1; x <= 1; x++) {
        for (int z = -1; z <= 1; z++) {
            FullChunkDataPacket pkChunk;
            pkChunk.chunkX = x;
            pkChunk.chunkZ = z;
            pkChunk.data = emptyChunkData;
            player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), pkChunk, player->getClientSubId());
        }
    }
    PlayStatusPacket pkStatus;
    pkStatus.status = 3;
    player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), pkStatus, player->getClientSubId());
}

THook(std::unique_ptr<Dimension>, _ZN9Dimension9createNewE11DimensionIdR5Level, DimensionId d, Level& l) {
    printf("CreateDimension %i\n", d);
    if ((int) d >= 4)
        return MinigameDimension::createDimension(l, (int) d);
    return original((DimensionId) 0, l);
}

TInstanceHook(void, _ZNK21ChangeDimensionPacket5writeER12BinaryStream, ChangeDimensionPacket, void* stream) {
    if (dimension >= 4)
        dimension = 0;
    original(this, stream);
}

TInstanceHook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK18PlayerActionPacket, ServerNetworkHandler, NetworkIdentifier const& nid, PlayerActionPacket const& ap) {
    if (ap.action == 14) {
        Player* foundPlayer = nullptr;
        for (auto const& player : ServerNetworkHandler::level->getUsers()) {
            if (player->getClientId().equalsTypeData(nid)) {
                foundPlayer = player.get();
                break;
            }
        }
        if (foundPlayer == nullptr)
            return;
        if (MinigameDimension::sendPlayerToDimension.count(foundPlayer) <= 0)
            return;
        DimensionId dimen = (DimensionId) MinigameDimension::sendPlayerToDimension.at(foundPlayer);
        MinigameDimension::sendPlayerToDimension.erase(foundPlayer);
        ((ServerPlayer*) foundPlayer)->changeDimension(dimen, false);
    }
    original(this, nid, ap);
}