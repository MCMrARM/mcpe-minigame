#pragma once

#include <minecraft/command/Command.h>

class CreateMinigameCommand : public Command {

public:
    std::string map;

    ~CreateMinigameCommand() override = default;

    static void setup(CommandRegistry& registry);

    void execute(CommandOrigin const& origin, CommandOutput& outp) override;

};