#include <minecraft/command/Command.h>
#include <minecraft/command/CommandMessage.h>
#include <minecraft/command/CommandOutput.h>
#include <minecraft/command/CommandParameterData.h>
#include "statichook.h"

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
    }

};

THook(void, _ZN9OpCommand5setupER15CommandRegistry, CommandRegistry& registry) {
    TestCommand::setup(registry);
    original(registry);
}