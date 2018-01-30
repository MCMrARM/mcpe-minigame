#include "MinigameDimension.h"

#include <minecraft/level/BiomeSource.h>
#include <minecraft/level/LevelStorage.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/LevelData.h>
#include <minecraft/level/MainChunkSource.h>
#include <minecraft/level/BlockSource.h>
#include <minecraft/level/RuntimeLightingManager.h>
#include <minecraft/entity/Player.h>
#include <minecraft/net/Packet.h>
#include <minecraft/net/PacketSender.h>
#include <minecraft/net/NetworkIdentifier.h>
#include <minecraft/server/ServerNetworkHandler.h>
#include <minecraft/level/ChangeDimensionRequest.h>
#include <minecraft/level/DBChunkStorage.h>
#include <minecraft/level/LevelChunk.h>
#include <cstring>
#include "../statichook.h"
#include "../patchhook.h"
#include "../util/PlayerHelper.h"
#include "../util/Log.h"
#include "FakeNetherDimension.h"
#include "EmptyWorldGenerator.h"

std::unordered_map<int, MinigameDimension::DefinedDimension> MinigameDimension::dimensions;
int MinigameDimension::nextDimensionId = 4;

MinigameDimension::MinigameDimension(Level& level, LevelStorage* storage, DimensionId dimensionId, short maxHeight) :
        Dimension(level, (DimensionId) 3, maxHeight), storage(storage) {
    id = dimensionId;
    dimensionName = "MinigameDimension";
    maxBrightness = 0xff;
    unk_76 = true;
    unk_14 = 0x3F;
    biomeSource = ThreadLocal<BiomeSource>([]() { return std::unique_ptr<BiomeSource>(new BiomeSource(7, (GeneratorType) 1)); });
}

int MinigameDimension::defineDimension(std::shared_ptr<LevelStorage> storage) {
    int dimenId = nextDimensionId++;
    dimensions[dimenId] = { std::move(storage) };
    return dimenId;
}

void MinigameDimension::undefineDimension(int dimension) {
    dimensions.erase(dimension);
}

void MinigameDimension::init() {
    EmptyWorldGenerator* generator = new EmptyWorldGenerator(this);
    this->generator = generator;
    std::unique_ptr<ChunkSource> chunkSource (generator);
    chunkSource = storage->createChunkStorage(std::move(chunkSource), level->getLevelData()->getStorageVersion());
    //chunkSource = level->getLevelStorage()->createChunkStorage(std::move(chunkSource), level->getLevelData()->getStorageVersion());
    chunkSource = std::unique_ptr<ChunkSource>(new MainChunkSource(std::move(chunkSource)));
    this->chunkSource = std::move(chunkSource);
    this->blockSource = std::unique_ptr<BlockSource>(new BlockSource(*level, *this, *this->chunkSource, false, true));
    this->runtimeLightingManager = std::unique_ptr<RuntimeLightingManager>(new RuntimeLightingManager(*this));
}

void MinigameDimension::requestDeletion() {
    shouldDelete = true;
    startLeaveGame();
}

std::unique_ptr<Dimension> MinigameDimension::createDimension(Level& level, int index) {
    return std::unique_ptr<Dimension>(new MinigameDimension(level, dimensions[index].storage.get(), (DimensionId) index, (short) 0x100));
}

void MinigameDimension::sendPlayerToDimension(Player* player, int dimension, Vec3 targetPos) {
    PlayerData& playerData = PlayerHelper::instance.getPlayerData(*player);
    playerData.tpTargetDimension = dimension;
    playerData.tpTargetPos = targetPos;
    /*ChangeDimensionPacket pkDimen;
    pkDimen.dimension = 1;
    pkDimen.pos = Vec3(1, 1, 1);
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
    player->getLevel()->getPacketSender()->sendToClient(player->getClientId(), pkStatus, player->getClientSubId());*/
    player->changeDimension((DimensionId) 1, false);
}

THook(std::unique_ptr<Dimension>, _ZN9Dimension9createNewE11DimensionIdR5Level, DimensionId d, Level& l) {
    printf("CreateDimension %i\n", d);
    if ((int) d == 1)
        return std::unique_ptr<Dimension>(new FakeNetherDimension(l));
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
    original(this, nid, ap);
    if (ap.action == 14) {
        Player* foundPlayer = nullptr;
        for (auto const& player : ServerNetworkHandler::level->getUsers()) {
            if (player->getClientId().equalsTypeData(nid) && player->getClientSubId() == ap.playerSubIndex) {
                foundPlayer = player.get();
                break;
            }
        }
        if (foundPlayer == nullptr)
            return;
        printf("And there we go!\n");
        PlayerData& playerData = PlayerHelper::instance.getPlayerData(*foundPlayer);
        if (playerData.tpTargetDimension == -1)
            return;
        int dimen = playerData.tpTargetDimension;
        playerData.tpTargetDimension = -1;

        //((ServerPlayer*) foundPlayer)->changeDimension((DimensionId) dimen, false);
        std::unique_ptr<ChangeDimensionRequest> request (new ChangeDimensionRequest());
        request->unknown = 0;
        request->sourceDimension = foundPlayer->getDimensionId();
        request->targetDimension = (DimensionId) dimen;
        request->position = playerData.tpTargetPos;
        request->unknown2 = false; // that's the bool from Player::changeDimension
        request->unknown3 = false; // ? has target pos maybe
        request->unknown4 = 0;
        foundPlayer->_usePortal(request->sourceDimension, request->targetDimension, 0x12C);
        foundPlayer->getLevel()->requestPlayerChangeDimension(*foundPlayer, std::move(request));

    }
}

void MinigameDimension::deleteUnusedDimensions(Level& level) {
    for (auto it = level.dimensions.begin(); it != level.dimensions.end(); ) {
        if (it->first >= 4 && ((MinigameDimension*) it->second.get())->shouldDelete && it->second->isLeaveGameDone()) {
            it = level.dimensions.erase(it);
        } else {
            it++;
        }
    }
}

TClasslessInstanceHook(void, _ZN14DBChunkStorage13saveLiveChunkER10LevelChunk, LevelChunk* chunk) {
    Log::trace("MinigameDimension", "saveLiveChunk - stubbed");
    chunk->setSaved();
}

TClasslessInstanceHook(bool, _ZN14DBChunkStorage9_hasChunkERK17DBChunkStorageKey, DBChunkStorageKey const& key) {
    DBChunkStorageKey newKey = { key.x, key.z, (int) key.dimension >= 4 ? (DimensionId) 0 : key.dimension };
    return original(this, newKey);
}

static DimensionId loadChunkFromDimensionIdHook(LevelChunk* self) {
    DimensionId ret = self->getDimensionId();
    if ((int) ret >= 4)
        return (DimensionId) 0;
    return ret;
}

__attribute__((constructor))
static void _patchDimensionId() {
    size_t patchOff = (size_t) dlsym(MinecraftHandle(), "_ZN14DBChunkStorage16_loadChunkFromDBER10LevelChunk") + 0x753 - 0x6F0;
    patchCallInstruction((void*) patchOff, (void*) loadChunkFromDimensionIdHook, false);

    patchOff = (size_t) dlsym(MinecraftHandle(), "_ZN9DimensionC2ER5Level11DimensionIds") + 0x6E1 - 0x3B0;
    ((char*) (void*) patchOff)[0] = 0x7D;
}