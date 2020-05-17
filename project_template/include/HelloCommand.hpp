#ifndef HELLOCOMMAND_HPP
#define HELLOCOMMAND_HPP

#include <controller/ICommand.hpp>
#include <IDiscordClient.hpp>

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