#include "ForceStartMinigameCommand.h"

#include <minecraft/command/CommandParameterData.h>
#include <minecraft/command/CommandOutput.h>
#include "../MinigameManager.h"
#include "MinigameCommandHelper.h"

void ForceStartMinigameCommand::setup(CommandRegistry& registry) {
    registry.registerCommand("forcestart", "Force start a minigame", (CommandPermissionLevel) 2, (CommandFlag) 0,
                             (CommandFlag) 0);
    registry.registerOverload<ForceStartMinigameCommand>("forcestart", CommandVersion(1, INT_MAX), CommandParameterData(
            type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistrySsE8typeid_tIT_EvE2id"),
            &CommandRegistry::parse<std::string>, "minigame", (CommandParameterDataType) 0, nullptr,
            offsetof(ForceStartMinigameCommand, minigame), true, -1));
}

void ForceStartMinigameCommand::execute(CommandOrigin const& origin, CommandOutput& outp) {
    auto minigame = MinigameCommandHelper::findMinigame(this->minigame, origin, outp);
    if (!minigame)
        return;
    minigame->broadcast("The game was started forcibly");
    if (minigame->getCountdown() != -1)
        minigame->startCountdown(Minigame::TICKS_FULL_PLAYERS_COUNTDOWN);
    else
        minigame->startCountdown();
    outp.success();
}