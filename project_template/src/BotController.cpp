#include <BotController.hpp>
#include <HelloCommand.hpp>

CBotController::CBotController()
{
    Prefix = "!";   //Prefix for the command. THIS IS NECESSARY.
}

void CBotController::OnReady()
{
    //Registers a new command, with description for the builtin help command. THE REGISTRATION MUST INSIDE THE OnReady EVENT.
    RegisterCommand<CHelloCommand>({"hello", "Writes \"Hello World!\" into the channel", 0, ""}, Client);
}