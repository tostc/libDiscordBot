#include <BotController.hpp>
#include <HelloCommand.hpp>

void CBotController::OnReady()
{
    //Registers a new command, with description for the builtin help command.
    RegisterCommand<CHelloCommand>({"hello", "Writes \"Hello World!\" into the channel", 0, "", AccessMode::EVERYBODY}, Client);
}