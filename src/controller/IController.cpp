/*
 * MIT License
 *
 * Copyright (c) 2020 Christian Tost
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <controller/IController.hpp>
#include <IDiscordClient.hpp>
#include "../commands/HelpCommand.hpp"
#include "../commands/RightsCommand.hpp"
#include "../commands/PrefixCommand.hpp"
#include "JSONCmdsConfig.hpp"

namespace DiscordBot
{
    std::string AccessModeToString(AccessMode Mode)
    {
        switch (Mode)
        {
            case AccessMode::OWNER:
            {
                return "OWNER";
            }break;

            case AccessMode::ROLE:
            {
                return "ROLE";
            }break;

            case AccessMode::EVERYBODY:
            {
                return "EVERYBODY";
            }break;
        }

        return "UNKNOWN";
    }

    //Trims a string.
    inline std::string trim(std::string str)
    {
        size_t Pos = str.find_first_not_of("\t\n\v\f\r ");
        if(Pos != std::string::npos)
            str = str.erase(0, Pos);

        Pos = str.find_last_not_of("\t\n\v\f\r ");
        if(Pos != std::string::npos)
            str = str.erase(Pos + 1);

        return str;
    }

    IController::IController(IDiscordClient *client) : Client(client), Prefix("!")
    {
        //Default config
        CmdsConfig = CommandsConfig(new CJSONCmdsConfig());

        RegisterCommand<CHelpCommand>({"h", "Prints a help dialog", 0, "", AccessMode::EVERYBODY}, this, Client);
        RegisterCommand<CHelpCommand>({"help", "Prints a help dialog", 0, "", AccessMode::EVERYBODY}, this, Client);

        RegisterCommand<CRightsCommand>({"setr", "Sets roles which can use a given command. E.g. Commandname Rolename/-s", -1, " ", AccessMode::OWNER}, this, Client);
        RegisterCommand<CRightsCommand>({"remover", "Removes roles from a command. E.g. Commandname Rolename/-s", -1, " ", AccessMode::OWNER}, this, Client);
        RegisterCommand<CRightsCommand>({"resetr", "Removes all roles of a command.", 1, " ", AccessMode::OWNER}, this, Client);
        RegisterCommand<CRightsCommand>({"getr", "Gets all roles for the command.", 1, " ", AccessMode::OWNER}, this, Client);

        RegisterCommand<CPrefixCommand>({"setp", "Sets the guild command prefix.", 1, " ", AccessMode::OWNER}, this, Client);
        RegisterCommand<CPrefixCommand>({"removep", "Removes the guild command prefix.", 0, "", AccessMode::OWNER}, this, Client);
        RegisterCommand<CPrefixCommand>({"showp", "Shows the guild command prefix.", 0, "", AccessMode::EVERYBODY}, this, Client);
    }

    /**
     * @return Gets all commands for a member. Only commands which the member can execute will be return.
     */
    std::vector<SCommandDescription> IController::GetCommands(Guild guild, GuildMember member)
    {
        std::vector<SCommandDescription> Ret;
        if(!CmdsConfig)
            return Ret;

        for (auto &&e : m_CommandDescs)
        {
            if(CanAccess(guild, member, e.first))
                Ret.push_back(e.second);
        }

        return Ret;
    }

    std::string IController::GetPrefix(Guild g)
    {
        return CmdsConfig->GetPrefix((std::string)g->ID, Prefix);
    }

    /**
     * @return Returns true if the user can access the command.
     */
    bool IController::CanAccess(Guild guild, GuildMember member, const std::string &Cmd)
    {
        //TODO: I don't like this mess.

        if(!guild)
            return m_CommandDescs[Cmd].Mode == AccessMode::EVERYBODY;

        if (guild->Owner->UserRef->ID == member->UserRef->ID)
            return true;        

        std::vector<std::string> RoleIDs = CmdsConfig->GetRoles((std::string)guild->ID, Cmd);
        if(RoleIDs.empty())
            return m_CommandDescs[Cmd].Mode == AccessMode::EVERYBODY;
        else
        {
            for (auto &&Id : RoleIDs)
            {
                auto IT = std::find_if(member->Roles->begin(), member->Roles->end(), [Id](Role r)
                {
                    return Id == (std::string)r->ID;
                });

                if(IT != member->Roles->end())
                    return true;
            }
        }

        return false;
    }

    /**
     * @brief Called if a new message was sended. Process the message and call associated commands.
     * 
     * @note Integrated help command "Prefix h" or "Prefix help".
     * 
     * @param msg: Message object. @see CMessage for more informations.
     * 
     * @note The GUILD_MESSAGES intent needs to be set to receive this event. This intent is set by default. @see Intent
     */
    void IController::OnMessage(Message msg)
    {
        bool Handled = false;
        OnMessage(msg, Handled);

        if(!Handled)
        {
            std::string GuildPrefix = GetPrefix(msg->GuildRef);

            //Checks if the command start with the prefix.
            if(msg->Content.compare(0, GuildPrefix.size(), GuildPrefix) == 0)
            {
                size_t Pos = msg->Content.find_first_of(' ', GuildPrefix.size());
                std::string Cmd = msg->Content.substr(GuildPrefix.size(), Pos - GuildPrefix.size());

                auto IT = m_CommandDescs.find(Cmd);
                if(IT != m_CommandDescs.end())
                {
                    if(!CanAccess(msg->GuildRef, msg->Member, Cmd))
                        return;

                    CommandContext ctx = CommandContext(new CCommandContext());
                    ctx->Msg = msg;
                    ctx->Command = Cmd;

                    if(Pos != std::string::npos)
                    {
                        std::string Params = trim(msg->Content.substr(Pos));
                        if(IT->second.ParamDelimiter.empty())
                            ctx->Params.push_back(Params);
                        else
                        {
                            Pos = 0;
                            size_t Beg = 0;
                            while (true)                        
                            {
                                Pos = Params.find(IT->second.ParamDelimiter, Pos);
                                ctx->Params.push_back(trim(Params.substr(Beg, Pos - Beg)));

                                if(Pos == std::string::npos)
                                    break;

                                Beg = Pos = Params.find_first_not_of(IT->second.ParamDelimiter, Pos + 1);
                            }   
                        }
                    }

                    //Executes the command.
                    if(ctx->Params.size() == IT->second.ParamCount || IT->second.ParamCount == -1)
                    {
                        auto Command = m_Commands[Cmd]->Create();
                        Command->OnExecute(ctx);
                    }
                }
            }
        }
    }
} // namespace DiscordBot
