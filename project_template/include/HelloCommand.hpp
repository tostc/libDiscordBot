#ifndef HELLOCOMMAND_HPP
#define HELLOCOMMAND_HPP

#include <DiscordBot/Commands/ICommand.hpp>
#include <DiscordBot/IDiscordClient.hpp>

using namespace DiscordBot;

class CHelloCommand : public ICommand
{
    public:
        CHelloCommand(IDiscordClient *client);
        virtual ~CHelloCommand() {}
    private:
        IDiscordClient *m_Client;

        void Hello(CommandContext ctx);
};

#endif