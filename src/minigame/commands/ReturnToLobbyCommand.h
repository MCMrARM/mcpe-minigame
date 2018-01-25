#pragma once

#include <minecraft/command/Command.h>

class ReturnToLobbyCommand : public Command {

public:
    std::string minigame;

    ~ReturnToLobbyCommand() override = default;

    static void setup(CommandRegistry& registry);

    void execute(CommandOrigin const& origin, CommandOutput& outp) override;

};