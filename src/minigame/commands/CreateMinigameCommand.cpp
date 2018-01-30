#include "CreateMinigameCommand.h"

#include <fstream>
#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOutput.h>
#include <minecraft/game/Minecraft.h>
#include <minecraft/server/ServerInstance.h>
#include <minecraft/level/Level.h>
#include <minecraft/level/LevelStorage.h>
#include <minecraft/level/LevelStorageSource.h>
#include <minecraft/resource/SkinPackKeyProvider.h>
#include <json/json.h>
#include "../../main.h"
#include "../MinigameManager.h"
#include "../MinigameDimension.h"
#include "MinigameCommandHelper.h"
#include "../skywars/SkyWarsMinigame.h"

void CreateMinigameCommand::setup(CommandRegistry& registry) {
    registry.registerCommand("create", "Create a minigame", (CommandPermissionLevel) 0, (CommandFlag) 0,
                             (CommandFlag) 0);
    registry.registerOverload<CreateMinigameCommand>("create", CommandVersion(1, INT_MAX), CommandParameterData(
            type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistrySsE8typeid_tIT_EvE2id"),
            &CommandRegistry::parse<std::string>, "map", (CommandParameterDataType) 0, nullptr,
            offsetof(CreateMinigameCommand, map), true, -1));
}

void CreateMinigameCommand::execute(CommandOrigin const& origin, CommandOutput& outp) {
    if (map.empty())
        map = "swmap";
    outp.addMessage("Creating minigame with map: " + map);

    static SkinPackKeyProvider skinPackKeyProvider;
    auto levelStorage = serverInstance->minecraft->getLevelSource().createLevelStorage(map, std::string(), skinPackKeyProvider);
    int dimenId = MinigameDimension::defineDimension(std::move(levelStorage));
    Dimension* dimension = origin.getLevel()->createDimension((DimensionId) dimenId);
    std::ifstream mapConfigStream (serverInstance->minecraft->getLevelSource().getBasePath() + "/" + map + "/config.json");
    Json::Reader reader;
    Json::Value mapConfigJson;
    reader.parse(mapConfigStream, mapConfigJson);
    MapConfig mapConfig;
    mapConfig.loadFromJSON(mapConfigJson);
    std::string name = MinigameManager::instance.getMinigameNameForPrefix("SW");
    std::shared_ptr<SkyWarsMinigame> minigame(new SkyWarsMinigame(&MinigameManager::instance, name, dimension, mapConfig));
    MinigameManager::instance.addGame(minigame);
    outp.addMessage("Successfully created minigame: " + name);
    if (origin.getOriginType() == CommandOriginType::PLAYER)
        minigame->addPlayer((Player*) origin.getEntity());
    outp.success();
}