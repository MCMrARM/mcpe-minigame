#pragma once

#include <minecraft/command/Command.h>

class ForceStartMinigameCommand : public Command {

public:
    std::string minigame;
    int time = -1;

    ~ForceStartMinigameCommand() override = default;

    static void setup(CommandRegistry& registry);

    void execute(CommandOrigin const& origin, CommandOutput& outp) override;

};