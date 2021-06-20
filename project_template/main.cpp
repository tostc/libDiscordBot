#include <DiscordBot/IDiscordClient.hpp>
#include <BotController.hpp>

using namespace DiscordBot;

int main(int argc, char const *argv[])
{
    DiscordClient client = IDiscordClient::Create("API-TOKEN");
    client->RegisterController<CBotController>();

    //Runs the loop until the Quit() method of the client is called.
    client->Run();
    return 0;
}
