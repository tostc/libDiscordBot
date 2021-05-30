/*
 * MIT License
 *
 * Copyright (c) 2021 Christian Tost
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

#ifndef GUILDMEMBER_CPP
#define GUILDMEMBER_CPP

#include "../controller/DiscordClient.hpp"
#include <models/DiscordException.hpp>
#include <models/GuildMember.hpp>
#include <models/PermissionException.hpp>
#include <tinyformat.h>

namespace DiscordBot
{
    void CGuildMember::Modify(const CGuildMemberProperties &Modifications)
    {
        static const std::map<size_t, std::pair<Permission, std::string>> MOD_PERMS = {
            {Adler32("nick"), {Permission::MANAGE_NICKNAMES, "MANAGE_NICKNAMES"}},
            {Adler32("roles"), {Permission::MANAGE_ROLES, "MANAGE_ROLES"}},
            {Adler32("deaf"), {Permission::DEAFEN_MEMBERS, "DEAFEN_MEMBERS"}},
            {Adler32("mute"), {Permission::MUTE_MEMBERS, "MUTE_MEMBERS"}}
        };

        CDiscordClient *Client = dynamic_cast<CDiscordClient*>(m_Client);
        Guild guild = Client->GetGuild(GuildID.load());

        auto values = Modifications.GetValues();
        bool HasRoles = Modifications.HasRoles();

        if(values.empty() && !HasRoles)    //Nothing to do here.
            return;

        GuildMember Bot = Client->GetBotMember(guild);

        CJSON js;

        for (auto &&e : values)
        {
            size_t Adler = Adler32(e.first.c_str());

            auto Perm = MOD_PERMS.at(Adler);
            if(!Client->CheckPermissions(guild, Perm.first))
                throw CPermissionException(tfm::format("Missing permission: '%s'", Perm.second));

            if(Adler == Adler32("nick") && UserRef->ID == Bot->UserRef->ID)
            {
                CJSON tmp;
                tmp.AddPair(e.first, e.second);

                if(!Client->CheckPermissions(guild, Permission::CHANGE_NICKNAME))
                    throw CPermissionException("Missing permission: 'CHANGE_NICKNAME'");

                auto res = Client->Patch(tfm::format("/guilds/%s/members/@me/nick", GuildID), tmp.Serialize());
                if(res->statusCode != 200)
                    throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));

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
            if(!Client->CheckPermissions(guild, Perm.first))
                throw CPermissionException(tfm::format("Missing permission: '%s'", Perm.second));


            std::vector<std::string> IDs;
            auto Roles = Modifications.GetRoles();
            
            for (auto &&e : Roles)
                IDs.push_back(e->ID);

            js.AddPair("roles", IDs);         
        }
        
        //Modification is already done.
        if(values.size() == 1 && values.find("nick") != values.end() && UserRef->ID == Bot->UserRef->ID)
            return;

        auto res = Client->Patch(tfm::format("/guilds/%s/members/%s", GuildID, UserRef->ID), js.Serialize());
        if(res->statusCode != 204)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuildMember::Move(Channel channel)
    {
        if(channel->Type != ChannelTypes::GUILD_VOICE)
            throw CDiscordClientException("Given channel is not a voice channel!");

        CDiscordClient *Client = dynamic_cast<CDiscordClient*>(m_Client);
        Guild guild = Client->GetGuild(GuildID.load());

        if(!Client->CheckPermissions(guild, Permission::MOVE_MEMBERS))
            throw CPermissionException("Missing permission: 'MOVE_MEMBERS'");

        CJSON js;
        js.AddPair("channel_id", channel->ID.load());

        auto res = Client->Patch(tfm::format("/guilds/%s/members/%s", GuildID, UserRef->ID), js.Serialize());
        if(res->statusCode != 204)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }
} // namespace DiscordBot

#endif //GUILDMEMBER_CPP