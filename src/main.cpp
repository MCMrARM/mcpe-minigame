#include <minecraft/command/Command.h>
#include <minecraft/command/CommandMessage.h>
#include <minecraft/command/CommandOutput.h>
#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOrigin.h>
#include <minecraft/server/ServerInstance.h>
#include <minecraft/game/Minecraft.h>
#include <minecraft/level/LevelStorageSource.h>
#include <minecraft/level/LevelStorage.h>
#include <minecraft/resource/SkinPackKeyProvider.h>
#include "statichook.h"
#include "main.h"
#include "minigame/Minigame.h"
#include "minigame/MinigameDimension.h"
#include "minigame/MinigameCommands.h"
#include "minigame/MinigameManager.h"

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
        outp.addMessage("§aThis absolutely §dworked! §e" + test.getMessage(origin));
        outp.success();

        int dimenId = dimensions[test.getMessage(origin)];
        if (dimenId == 0) {
            auto levelStorage = serverInstance->minecraft->getLevelSource().createLevelStorage(test.getMessage(origin), std::string(), skinPackKeyProvider);
            dimenId = MinigameDimension::defineDimension(std::move(levelStorage));
            dimensions[test.getMessage(origin)] = dimenId;
        }
        MapConfig mapConfig;
        mapConfig.minPlayers = 2;
        mapConfig.tryGetMinPlayers = 4;
        mapConfig.maxPlayers = 8;
        mapConfig.spawnPositions = {{87, 8, 8}, {108, 8, 8}};
        MinigameDimension::sendPlayerToDimension((Player*) origin.getEntity(), dimenId);
    }

};

THook(void, _ZN9OpCommand5setupER15CommandRegistry, CommandRegistry& registry) {
    TestCommand::setup(registry);
    ForceStartMinigameCommand::setup(registry);
    original(registry);
}

extern "C" void mod_set_server(ServerInstance* instance) {
    serverInstance = instance;
}