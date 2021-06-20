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
#include <DiscordBot/Exceptions/DiscordException.hpp>
#include <DiscordBot/Models/GuildMember.hpp>
#include <DiscordBot/Exceptions/PermissionException.hpp>
#include <tinyformat.h>
#include "../helpers/Helper.hpp"

namespace DiscordBot
{
    void CGuildMember::Modify(const CGuildMemberProperties &Modifications)
    {
        static const std::map<size_t, Permission> MOD_PERMS = {
            {Adler32("nick"), Permission::MANAGE_NICKNAMES},
            {Adler32("roles"), Permission::MANAGE_ROLES},
            {Adler32("deaf"), Permission::DEAFEN_MEMBERS},
            {Adler32("mute"), Permission::MUTE_MEMBERS}
        };

        Guild guild = m_Client->GetGuild(GuildID);

        auto values = Modifications.GetValues();
        bool HasRoles = Modifications.HasRoles();

        if(values.empty() && !HasRoles)    //Nothing to do here.
            return;

        GuildMember Bot = m_Client->GetBotMember(guild);

        CJSON js;

        for (auto &&e : values)
        {
            size_t Adler = Adler32(e.first.c_str());

            auto Perm = MOD_PERMS.at(Adler);
            CheckPermissions(Perm);

            if(Adler == Adler32("nick") && UserRef->ID == Bot->UserRef->ID)
            {
                CJSON tmp;
                tmp.AddPair(e.first, e.second);

                CheckPermissions(Permission::CHANGE_NICKNAME);

                auto req = m_MsgMgr->RequestMessage(Internal::Requests::PATCH, CreateHttpMessage(tfm::format("/guilds/%s/members/@me/nick", GuildID), tmp.Serialize()));
                auto res = req->Value<ix::HttpResponsePtr>();
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
            CheckPermissions(Perm);

            std::vector<std::string> IDs;
            auto Roles = Modifications.GetRoles();
            
            for (auto &&e : Roles)
                IDs.push_back((std::string)e->ID);

            js.AddPair("roles", IDs);         
        }
        
        //Modification is already done.
        if(values.size() == 1 && values.find("nick") != values.end() && UserRef->ID == Bot->UserRef->ID)
            return;

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::PATCH, CreateHttpMessage(tfm::format("/guilds/%s/members/%s", GuildID, UserRef->ID), js.Serialize()));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 204)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuildMember::Move(Channel channel)
    {
        CJSON js;
        CheckPermissions(Permission::MOVE_MEMBERS);

        if(channel)
        {
            if(channel->Type != ChannelTypes::GUILD_VOICE)
                throw CDiscordClientException("Given channel is not a voice channel!");

            js.AddPair("channel_id", (std::string)channel->ID);
        }
        else
            js.AddPair("channel_id", nullptr); // Kicks the user.

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::PATCH, CreateHttpMessage(tfm::format("/guilds/%s/members/%s", GuildID, UserRef->ID), js.Serialize()));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 204)
            throw CDiscordClientException("Error during member modification. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }
} // namespace DiscordBot

#endif //GUILDMEMBER_CPP