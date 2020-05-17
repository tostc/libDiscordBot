#include <HelloCommand.hpp>

CHelloCommand::CHelloCommand(IDiscordClient *client) : m_Client(client)
{
    //REgisters the function which handels a command.
    RegisterCommandHandler("hello", std::bind(&CHelloCommand::Hello, this, std::placeholders::_1));
}

void CHelloCommand::Hello(CommandContext ctx)
{
    m_Client->SendMessage(ctx->Msg->ChannelRef, "Hello World!");
}