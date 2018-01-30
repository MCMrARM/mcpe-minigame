#include <minecraft/command/CommandParameterData.h>
#include <minecraft/server/ServerInstance.h>
#include <minecraft/game/Minecraft.h>
#include <minecraft/game/GameMode.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/BlockSource.h>
#include <minecraft/entity/Player.h>
#include <minecraft/entity/PlayerInventoryProxy.h>
#include "statichook.h"
#include "main.h"
#include "minigame/Minigame.h"
#include "minigame/MinigameDimension.h"
#include "minigame/MinigameManager.h"
#include "minigame/commands/ForceStartMinigameCommand.h"
#include "minigame/commands/JoinMinigameCommand.h"
#include "minigame/commands/ReturnToLobbyCommand.h"
#include "minigame/commands/CreateMinigameCommand.h"
#include "lobby/LobbyManager.h"
#include "util/Log.h"

ServerInstance* serverInstance;

THook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK23AdventureSettingsPacket) {
}

TInstanceHook(bool, _ZN8GameMode12destroyBlockERK8BlockPosa, GameMode, BlockPos const& bp, char c) {
    if (!player->getRegion()->checkBlockDestroyPermissions(*player, bp, *player->getSupplies().getSelectedItem(), false)) {
        Log::trace("GameMode", "destroyBlock: cheating attempt by %s", player->getNameTag().c_str());
        return false;
    }
    return original(this, bp, c);
}

THook(void, _ZN9OpCommand5setupER15CommandRegistry, CommandRegistry& registry) {
    CreateMinigameCommand::setup(registry);
    ForceStartMinigameCommand::setup(registry);
    JoinMinigameCommand::setup(registry);
    ReturnToLobbyCommand::setup(registry);
    original(registry);
}

extern "C" void mod_set_server(ServerInstance* instance) {
    serverInstance = instance;
    LobbyManager::instance.setDimension(instance->minecraft->getLevel()->createDimension((DimensionId) 0));
}