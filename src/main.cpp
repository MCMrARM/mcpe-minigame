#include <minecraft/command/Command.h>
#include <minecraft/command/CommandMessage.h>
#include <minecraft/command/CommandOutput.h>
#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOrigin.h>
#include <minecraft/server/ServerInstance.h>
#include <minecraft/game/Minecraft.h>
#include <minecraft/game/GameMode.h>
#include <minecraft/level/LevelStorageSource.h>
#include <minecraft/level/LevelStorage.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/BlockSource.h>
#include <minecraft/entity/Player.h>
#include <minecraft/entity/PlayerInventoryProxy.h>
#include <minecraft/resource/SkinPackKeyProvider.h>
#include <sstream>
#include <string>
#include "statichook.h"
#include "main.h"
#include "minigame/Minigame.h"
#include "minigame/MinigameDimension.h"
#include "minigame/MinigameManager.h"
#include "minigame/skywars/SkyWarsMinigame.h"
#include "minigame/commands/ForceStartMinigameCommand.h"
#include "minigame/commands/JoinMinigameCommand.h"
#include "minigame/commands/ReturnToLobbyCommand.h"
#include "lobby/LobbyManager.h"
#include "util/Log.h"

ServerInstance* serverInstance;

static SkinPackKeyProvider skinPackKeyProvider;

static std::map<std::string, int> dimensions;

class TestCommand : public Command {

public:
    CommandMessage test;

    ~TestCommand() override = default;

    static void setup(CommandRegistry& registry) {
        registry.registerCommand("test", "command.test.description", (CommandPermissionLevel) 0, (CommandFlag) 0, (CommandFlag) 0);
        registry.registerOverload<TestCommand>("test", CommandVersion(1, INT_MAX), CommandParameterData(CommandMessage::type_id(), &CommandRegistry::parse<CommandMessage>, "message", (CommandParameterDataType) 0, nullptr, offsetof(TestCommand, test), false, -1));
    }

    void execute(CommandOrigin const& origin, CommandOutput& outp) override {
        std::string map = test.getMessage(origin);
        if (map.empty())
            map = "swtest";
        outp.addMessage("§aThis absolutely §dworked! §e" + map);
        outp.success();

        int dimenId = dimensions[map];
        if (dimenId == 0) {
            auto levelStorage = serverInstance->minecraft->getLevelSource().createLevelStorage(map, std::string(), skinPackKeyProvider);
            dimenId = MinigameDimension::defineDimension(std::move(levelStorage));
            dimensions[map] = dimenId;
        }

        Dimension* dimension = origin.getLevel()->createDimension((DimensionId) dimenId);
        MapConfig mapConfig;
        mapConfig.minPlayers = 2;
        mapConfig.tryGetMinPlayers = 4;
        mapConfig.maxPlayers = 8;
        mapConfig.spawnPositions = {{87, 10, 8}, {108, 10, 8}};
        std::shared_ptr<SkyWarsMinigame> minigame(new SkyWarsMinigame(&MinigameManager::instance, "SW-1", dimension, mapConfig));
        MinigameManager::instance.addGame(minigame);
        minigame->addPlayer((Player*) origin.getEntity());
    }

};

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
    TestCommand::setup(registry);
    ForceStartMinigameCommand::setup(registry);
    JoinMinigameCommand::setup(registry);
    ReturnToLobbyCommand::setup(registry);
    original(registry);
}

extern "C" void mod_set_server(ServerInstance* instance) {
    serverInstance = instance;
    LobbyManager::instance.setDimension(instance->minecraft->getLevel()->createDimension((DimensionId) 0));
}