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

#include "GuildAdmin.hpp"
#include "DiscordClient.hpp"
#include <models/DiscordException.hpp>
#include <vector>
#include "../helpers/Helper.hpp"

namespace DiscordBot
{
    void CGuildAdmin::ModifyMember(const CModifyMember &mod)
    {
        static const std::map<size_t, std::pair<Permission, std::string>> MOD_PERMS = {
            {Adler32("nick"), {Permission::MANAGE_NICKNAMES, "MANAGE_NICKNAMES"}},
            {Adler32("roles"), {Permission::MANAGE_ROLES, "MANAGE_ROLES"}},
            {Adler32("deaf"), {Permission::DEAFEN_MEMBERS, "DEAFEN_MEMBERS"}},
            {Adler32("mute"), {Permission::MUTE_MEMBERS, "MUTE_MEMBERS"}},
            {Adler32("channel_id"), {Permission::MOVE_MEMBERS, "MOVE_MEMBERS"}}
        };

        if(!mod.GetUserRef())
            throw CDiscordClientException("Error: A null user can't be modified", DiscordClientErrorType::MISSING_USER_REF);

        auto values = mod.GetValues();
        bool HasRoles = mod.HasRoles();

        if(values.empty() && !HasRoles)    //Nothing to do here.
            return;

        GuildMember member = m_Client->GetMember(m_Guild, mod.GetUserRef()->ID);
        GuildMember Bot;

        CJSON js;

        for (auto &&e : values)
        {
            size_t Adler = Adler32(e.first.c_str());

            auto Perm = MOD_PERMS.at(Adler);
            Bot = CheckBotPermissions(Perm.first, "Missing right to modify user: '" + Perm.second + "'");

            if(Adler == Adler32("nick") && mod.GetUserRef()->ID == Bot->UserRef->ID)
            {
                CJSON tmp;
                tmp.AddPair(e.first, e.second);

                CheckBotPermissions(Permission::CHANGE_NICKNAME, "Missing right to modify user: 'CHANGE_NICKNAME'");
                RenameSelf(tmp.Serialize());    

                continue;
            }

            if(Adler == Adler32("nick") || (Adler == Adler32("channel_id") && e.second != "null"))
                js.AddPair(e.first, e.second);
            else
                js.AddJSON(e.first, e.second);
        }

        if(HasRoles)
        {
            auto Perm = MOD_PERMS.at(Adler32("roles"));
            Bot = CheckBotPermissions(Perm.first, "Missing right to modify user: '" + Perm.second + "'");

            std::vector<std::string> IDs;
            auto Roles = mod.GetRoles();
            
            for (auto &&e : Roles)
                IDs.push_back(e->ID);

            js.AddPair("roles", IDs);         
        }
        
        //Modification is already done.
        if(values.size() == 1 && values.find("nick") != values.end() && mod.GetUserRef()->ID == Bot->UserRef->ID)
            return;

        auto res = m_Client->Patch("/guilds/" + m_Guild->ID + "/members/" + mod.GetUserRef()->ID, js.Serialize());
        if(res->statusCode != 204)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::BanMember(User member, const std::string &Reason, int DeleteMsgDays)
    {
        CheckBotPermissions(Permission::BAN_MEMBERS, "Missing right to ban users: 'BAN_MEMBERS'");
        CJSON js;
        if(!Reason.empty())
            js.AddPair("reason", Reason);

        if(DeleteMsgDays != -1)
            js.AddPair("delete_message_days", DeleteMsgDays);

        auto res = m_Client->Put("/guilds/" + m_Guild->ID + "/bans/" + member->ID, js.Serialize());
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't ban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::UnbanMember(User user)    
    {
        CheckBotPermissions(Permission::BAN_MEMBERS, "Missing right to unban users: 'BAN_MEMBERS'");
        auto res = m_Client->Delete("/guilds/" + m_Guild->ID + "/bans/" + user->ID);
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't unban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    std::vector<std::pair<std::string, User>> CGuildAdmin::GetGuildBans()    
    {
        std::vector<std::pair<std::string, User>> ret;

        CheckBotPermissions(Permission::BAN_MEMBERS, "Missing right to see the ban list: 'BAN_MEMBERS'");
        auto res = m_Client->Get("/guilds/" + m_Guild->ID + "/bans");

        if(res->statusCode != 200)
            throw CDiscordClientException("Unable to get ban list. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);

        CJSON js;
        auto list = js.Deserialize<std::vector<std::string>>(res->body);

        for (auto e : list)
        {
            js.ParseObject(e);

            User user = m_Client->GetUserOrAdd(js.GetValue<std::string>("user"));
            ret.push_back({js.GetValue<std::string>("reason"), user});
        }

        return ret;        
    }

    void CGuildAdmin::KickMember(User member)    
    {
        CheckBotPermissions(Permission::KICK_MEMBERS, "Missing right to kick a user: 'KICK_MEMBERS'");
        auto res = m_Client->Delete("/guilds/" + m_Guild->ID + "/members/" + member->ID);
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't kick user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::CreateChannel(const CModifyChannel &channel)
    {
        std::string js = ModifyChannelToJS(channel);
        auto res = m_Client->Post("/guilds/" + m_Guild->ID + "/channels", js);
        if(res->statusCode != 201)
            throw CDiscordClientException("Can't create channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::ModifyChannel(const CModifyChannel &channel)
    {
        if(!channel.GetChannelRef())
            return;

        std::string js = ModifyChannelToJS(channel);

        auto res = m_Client->Patch("/channels/" + channel.GetChannelRef()->ID, js);
        if(res->statusCode != 200)
            throw CDiscordClientException("Can't modify channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::DeleteChannel(Channel channel, const std::string &reason)
    {
        CheckBotPermissions(Permission::MANAGE_CHANNELS, "Missing right to manage channels: 'MANAGE_CHANNELS'");
        if(!channel)
            return;

        CJSON js;
        js.AddPair("reason", reason);

        auto res = m_Client->Delete("/channels/" + channel->ID, js.Serialize());
        if(res->statusCode != 200)
            throw CDiscordClientException("Can't delete channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    void CGuildAdmin::AddChannelAction(Channel channel, Action action)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::string ID;
        if(channel)
            ID = channel->ID;

        ActionType Types = action->GetTypes();
        for (uint32_t i = 1; i < (uint32_t)ActionType::TOTAL_ACTIONS; i <<= 1)
        {
            ActionType Type = Types & (ActionType)i;
            if(Type != ActionType::NONE)
            {
                auto IT = m_Actions.find(ID);
                if(IT == m_Actions.end())
                    m_Actions.insert({ID, {{Type, action}}});
                else
                {
                    auto InnerIT = IT->second.find(Type);
                    if(InnerIT != IT->second.end())
                        throw CDiscordClientException("Action is already registered!", DiscordClientErrorType::ACTION_ALREADY_REG);

                    IT->second.insert({Type, action});
                }
            }
        }
    }

    void CGuildAdmin::RemoveChannelAction(Channel channel, ActionType types) 
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::string ID;
        if(channel)
            ID = channel->ID;

        for (uint32_t i = 1; i < (uint32_t)ActionType::TOTAL_ACTIONS; i <<= 1)
        {
            ActionType Type = types & (ActionType)i;
            auto IT = m_Actions.find(ID);
            if(IT != m_Actions.end())
                IT->second.erase(Type);
        }
    }

    void CGuildAdmin::OnUserVoiceStateChanged(Channel c, GuildMember m)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::vector<std::string> IDs = {
            "",
            c->ID
        };

        for (auto ID : IDs)
        {
            auto IT = m_Actions.find(ID);
            if(IT != m_Actions.end())
            {
                ActionType Type = ActionType::NONE;
                if(m->State)
                    Type = ActionType::USER_JOIN;
                else
                    Type = ActionType::USER_LEAVE;

                auto InnerIT = IT->second.find(Type);
                if(InnerIT != IT->second.end())
                    FireAction(Type, InnerIT->second, c, m);
            }
        }
    }

    void CGuildAdmin::OnMessageEvent(ActionType Type, Channel c, Message m)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::vector<std::string> IDs = {
            "",
            c->ID
        };

        for (auto ID : IDs)
        {
            auto IT = m_Actions.find(ID);
            if(IT != m_Actions.end())
            {
                auto InnerIT = IT->second.find(Type);
                if(InnerIT != IT->second.end())
                    FireAction(Type, InnerIT->second, c, m);
            }
        }
    }

    //--------------------------Private--------------------------//

    GuildMember CGuildAdmin::CheckBotPermissions(Permission p, const std::string &errMsg)
    {
        auto bot = m_Client->GetBotMember(m_Guild);
        if(!HasPermission(bot, p))
            throw CDiscordClientException(errMsg, DiscordClientErrorType::MISSING_PERMISSION);

        return bot;
    }

    bool CGuildAdmin::HasPermission(GuildMember member, Permission perm)
    {
        bool ret = false;
        for (auto e : member->Roles.load())
        {
            if((e->Permissions & perm) == perm)
            {
                ret = true;
                break;
            }
        }
        
        return ret;
    }

    void CGuildAdmin::RenameSelf(const std::string &js)
    {
        auto res = m_Client->Patch("/guilds/" + m_Guild->ID + "/members/@me/nick", js);
        if(res->statusCode != 200)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode), DiscordClientErrorType::HTTP_ERROR);
    }

    std::string CGuildAdmin::ModifyChannelToJS(const CModifyChannel &channel)
    {
        CheckBotPermissions(Permission::MANAGE_CHANNELS, "Missing right to manage channels: 'MANAGE_CHANNELS'");
        CJSON js;

        auto values = channel.GetValues();
        bool HasOverwrites = channel.HasOverwrites();

        if(values.empty() && !HasOverwrites)    //Nothing to do here.
            return "";

        for (auto &&e : values)
        {
            size_t Adler = Adler32(e.first.c_str());
            if(Adler == Adler32("name") ||
               Adler == Adler32("topic") ||
               Adler == Adler32("parent_id"))
                js.AddPair(e.first, e.second);
            else
                js.AddJSON(e.first, e.second);
        }
        
        if(HasOverwrites)
        {
            std::vector<std::string> OWJS;
            auto overwrites = channel.GetOverwrites();
            for (auto &&e : overwrites)
            {
                CJSON tmp;
                tmp.AddPair("id", e->ID.load());
                tmp.AddPair("type", e->Type.load());
                tmp.AddPair("allow", std::to_string((int)e->Allow));
                tmp.AddPair("deny", std::to_string((int)e->Deny));

                OWJS.push_back(tmp.Serialize());
            } 

            CJSON tmp;
            js.AddJSON("permission_overwrites", tmp.Serialize(OWJS));
        }

        return js.Serialize();
    }
    
} // namespace DiscordBot
