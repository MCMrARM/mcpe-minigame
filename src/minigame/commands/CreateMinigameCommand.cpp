#include "CreateMinigameCommand.h"

#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOutput.h>
#include "MinigameCommandHelper.h"
#include "../MinigameManager.h"
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

    std::string name = MinigameManager::instance.getMinigameNameForPrefix("SW");
    auto minigame = MinigameManager::instance.createGame<SkyWarsMinigame>(name, origin.getLevel(), name);
    if (!minigame) {
        outp.error("Failed to create minigame");
        return;
    }
    outp.addMessage("Successfully created minigame: " + name);
    if (origin.getOriginType() == CommandOriginType::PLAYER)
        minigame->addPlayer((Player*) origin.getEntity());
    outp.success();
}