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

#include <DiscordBot/IDiscordClient.hpp>
#include <DiscordBot/Models/Role.hpp>
#include <DiscordBot/Models/Guild.hpp>
#include "RightsCommand.hpp"

namespace DiscordBot
{
    CRightsCommand::CRightsCommand(IController *controller, IDiscordClient *client) : m_Controller(controller), m_Client(client) 
    {
        RegisterCommandHandler("setr", std::bind(&CRightsCommand::SetRoles, this, std::placeholders::_1));
        RegisterCommandHandler("remover", std::bind(&CRightsCommand::RemoveRoles, this, std::placeholders::_1));
        RegisterCommandHandler("resetr", std::bind(&CRightsCommand::RemoveAllRoles, this, std::placeholders::_1));
        RegisterCommandHandler("getr", std::bind(&CRightsCommand::GetRoles, this, std::placeholders::_1));
    }

    void CRightsCommand::SetRoles(CommandContext ctx)
    {
        if(!ctx->Msg->GuildRef)
            return;

        std::string Cmd;
        std::vector<std::string> RoleIDs;
        if(!SplitParams(ctx, Cmd, RoleIDs))
            return;

        CommandsConfig cfg = m_Controller->GetCmdConfig();
        if(cfg)
            cfg->AddRoles((std::string)ctx->Msg->GuildRef->ID, Cmd, RoleIDs);
    }

    void CRightsCommand::RemoveRoles(CommandContext ctx)
    {
        if(!ctx->Msg->GuildRef)
            return;

        std::string Cmd;
        std::vector<std::string> RoleIDs;
        if(!SplitParams(ctx, Cmd, RoleIDs))
            return;

        CommandsConfig cfg = m_Controller->GetCmdConfig();
        if(cfg)
            cfg->RemoveRoles((std::string)ctx->Msg->GuildRef->ID, Cmd, RoleIDs);
    }

    void CRightsCommand::RemoveAllRoles(CommandContext ctx)
    {
        if(!ctx->Msg->GuildRef)
            return;

        std::string Cmd = ctx->Params.front();
        if(!m_Controller->CommandExists(Cmd))
        {
            ctx->Msg->ChannelRef->SendMessage("Command '" + Cmd + "' doesn't exists! Can't remove rights!");
            return;
        }

        CommandsConfig cfg = m_Controller->GetCmdConfig();
        if(cfg)
            cfg->DeleteCommand((std::string)ctx->Msg->GuildRef->ID, Cmd);
    }

    void CRightsCommand::GetRoles(CommandContext ctx)
    {
        if(!ctx->Msg->GuildRef)
            return;

        std::string Cmd = ctx->Params.front();
        if(!m_Controller->CommandExists(Cmd))
        {
            ctx->Msg->ChannelRef->SendMessage("Command '" + Cmd + "' doesn't exists! Can't get rights!");
            return;
        }

        CommandsConfig cfg = m_Controller->GetCmdConfig();
        if(cfg)
        {
            std::vector<std::string> Roles = cfg->GetRoles((std::string)ctx->Msg->GuildRef->ID, Cmd);
            std::string Msg;
            for (auto &&e : Roles)
            {
                if(!Msg.empty())
                    Msg += ", ";

                auto IT = ctx->Msg->GuildRef->Roles->find(e);
                if(IT != ctx->Msg->GuildRef->Roles->end())
                    Msg += IT->second->Name;
            }

            if(Msg.empty())
                Msg = AccessModeToString(m_Controller->GetAccessMode(Cmd));

            ctx->Msg->ChannelRef->SendMessage(Cmd + " = [" + Msg + "]");
        }
    }

    std::string CRightsCommand::GetRoleID(Guild guild, const std::string &RoleName)
    {
        auto IT = guild->Roles->find(RoleName);
        if(IT != guild->Roles->end())
            return RoleName;

        for (auto &&e : guild->Roles.load())
        {
            if(e.second->Name == RoleName)
                return (std::string)e.second->ID;
        }

        return "";
    }

    bool CRightsCommand::SplitParams(CommandContext ctx, std::string &Cmd, std::vector<std::string> &RoleIDs)
    {
        Cmd = ctx->Params.front();
        if(!m_Controller->CommandExists(Cmd))
        {
            ctx->Msg->ChannelRef->SendMessage("Command '" + Cmd + "' doesn't exists! Can't set rights!");
            return false;
        }

        for (size_t i = 1; i < ctx->Params.size(); i++)
        {
            std::string RoleID = GetRoleID(ctx->Msg->GuildRef, ctx->Params[i]);
            if(RoleID.empty())
            {
                ctx->Msg->ChannelRef->SendMessage("Role '" + ctx->Params[i] + "' doesn't exists! Can't set rights!");
                return false;
            }
            else
                RoleIDs.push_back(RoleID);
        }

        if(RoleIDs.empty())
        {
            ctx->Msg->ChannelRef->SendMessage("No given roles for command '" + Cmd + "'! Can't set rights!");
            return false;
        }

        return true;
    }
} // namespace DiscordBot