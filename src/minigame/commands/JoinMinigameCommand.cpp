#include "JoinMinigameCommand.h"

#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOutput.h>
#include "../MinigameManager.h"
#include "MinigameCommandHelper.h"

void JoinMinigameCommand::setup(CommandRegistry& registry) {
    registry.registerCommand("join", "Joins a minigame", (CommandPermissionLevel) 0, (CommandFlag) 0, (CommandFlag) 0);
    registry.registerOverload<JoinMinigameCommand>("join", CommandVersion(1, INT_MAX), CommandParameterData(
            type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistrySsE8typeid_tIT_EvE2id"),
            &CommandRegistry::parse<std::string>, "join", (CommandParameterDataType) 0, nullptr,
            offsetof(JoinMinigameCommand, minigame), false, -1));
}

void JoinMinigameCommand::execute(CommandOrigin const& origin, CommandOutput& outp) {
    if (origin.getOriginType() != CommandOriginType::PLAYER)
        return;
    auto minigame = MinigameCommandHelper::findMinigame(this->minigame, origin, outp);
    if (minigame->addPlayer((Player*) origin.getEntity()))
        outp.success();
    else
        outp.error("Failed to join the specified minigame");
}