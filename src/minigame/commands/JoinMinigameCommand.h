#pragma once

#include <minecraft/command/Command.h>

class JoinMinigameCommand : public Command {

public:
    std::string minigame;

    ~JoinMinigameCommand() override = default;

    static void setup(CommandRegistry& registry);

    void execute(CommandOrigin const& origin, CommandOutput& outp) override;

};