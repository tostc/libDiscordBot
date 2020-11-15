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

namespace DiscordBot
{
    void CGuildAdmin::ModifyMember(const CModifyMember &mod)
    {
        static const std::map<Modifications, std::pair<Permission, std::string>> MOD_PERMS = {
            {Modifications::NICK, {Permission::MANAGE_NICKNAMES, "MANAGE_NICKNAMES"}},
            {Modifications::ROLES, {Permission::MANAGE_ROLES, "MANAGE_ROLES"}},
            {Modifications::DEAF, {Permission::DEAFEN_MEMBERS, "DEAFEN_MEMBERS"}},
            {Modifications::MUTE, {Permission::MUTE_MEMBERS, "MUTE_MEMBERS"}},
            {Modifications::MOVE, {Permission::MOVE_MEMBERS, "MOVE_MEMBERS"}}
        };

        if(!mod.GetUserRef())
            throw CDiscordClientException("Error: A null user can't be modified", DiscordClientErrorType::MISSING_USER_REF);

        if(mod.GetMods() == Modifications::NONE)    //Nothing to do here.
            return;

        GuildMember member = m_Client->GetMember(m_Guild, mod.GetUserRef()->ID);
        GuildMember Bot;

        CJSON js;
        for (int i = 1; i <= (int)Modifications::MOVE; i *= 2)
        {
            Modifications m = mod.GetMods() & (Modifications)i;
            if(m == Modifications::NONE)
                continue;

            auto Perm = MOD_PERMS.at(m);
            Bot = CheckBotPermissions(Perm.first, "Missing right to modify user: '" + Perm.second + "'");

            switch (m)
            {
                case Modifications::NICK:
                {
                    js.AddPair("nick", mod.GetNick());
                    if(mod.GetUserRef()->ID == Bot->UserRef->ID)
                    {
                        CheckBotPermissions(Permission::CHANGE_NICKNAME, "Missing right to modify user: 'CHANGE_NICKNAME'");
                        RenameSelf(js.Serialize());    
                    }                    
                } break;

                case Modifications::ROLES:
                {
                    std::vector<std::string> IDs;
                    auto Roles = mod.GetRoles();
                    for (auto e : Roles)
                        IDs.push_back(e->ID);

                    js.AddPair("roles", IDs);
                } break;

                case Modifications::DEAF:
                {
                    if(!member->State)
                        throw CDiscordClientException("User can't be deafen, because he are not connected to a voice channel.", DiscordClientErrorType::MEMBER_NOT_IN_VC);

                    js.AddPair("deaf", mod.IsDeafed());
                } break;

                case Modifications::MUTE:
                {
                    if(!member->State)
                        throw CDiscordClientException("User can't be muted, because he are not connected to a voice channel.", DiscordClientErrorType::MEMBER_NOT_IN_VC);

                    js.AddPair("mute", mod.IsMuted());
                } break;

                case Modifications::MOVE:
                {
                    if(!member->State)
                        throw CDiscordClientException("User can't be moved, because he are not connected to a voice channel.", DiscordClientErrorType::MEMBER_NOT_IN_VC);

                    Channel c = mod.GetChannel();
                    if(c)
                        js.AddPair("channel_id", c->ID.load());
                    else
                        js.AddPair("channel_id", nullptr); //Kicks the user.
                } break;        
            }
        }

        //Modification is already done.
        if(mod.GetMods() == Modifications::NICK && mod.GetUserRef()->ID == Bot->UserRef->ID)
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
    
} // namespace DiscordBot
