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

#include <models/Guild.hpp>
#include <models/exceptions/PermissionException.hpp>
#include <models/exceptions/DiscordException.hpp>
#include <tinyformat.h>
#include "../controller/DiscordClient.hpp"
#include "../helpers/Helper.hpp"

namespace DiscordBot
{
    Channel CGuild::CreateChannel(const CChannelProperties &ChannelInfo)
    {
        Guild guild = dynamic_cast<CDiscordClient*>(m_Client)->GetGuild(ID);
        if(!dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::MANAGE_CHANNELS))
            throw CPermissionException("Missing permission: 'MANAGE_CHANNELS'");

        CJSON js;

        auto values = ChannelInfo.GetValues();
        bool HasOverwrites = ChannelInfo.HasOverwrites();

        if(values.empty() && !HasOverwrites)    //Nothing to do here.
            throw CDiscordClientException("No channel informations given");

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
            auto overwrites = ChannelInfo.GetOverwrites();
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

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post(tfm::format("/guilds/%s/channels", ID), js.Serialize());
        if(res->statusCode != 201)
            throw CDiscordClientException("Can't create channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    
        // Returns the new created channel.
        js.ParseObject(res->body);
        auto IT = Channels->find(js.GetValue<std::string>("id"));
        return IT->second;
    }

    void CGuild::Ban(GuildMember Member, const std::string &Reason, int DeleteMsgDays)
    {
        BanMembersCheck();

        CJSON js;
        if(!Reason.empty())
            js.AddPair("reason", Reason);

        if(DeleteMsgDays != -1)
            js.AddPair("delete_message_days", DeleteMsgDays);

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Put(tfm::format("/guilds/%s/bans/%s", ID, Member->UserRef->ID), js.Serialize());
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't ban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuild::Unban(User user)
    {
        BanMembersCheck();
        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/guilds/%s/bans/%s", ID, user->ID));
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't unban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuild::Kick(GuildMember Member)
    {
        Guild guild = dynamic_cast<CDiscordClient*>(m_Client)->GetGuild(ID);
        if(!dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::KICK_MEMBERS))
            throw CPermissionException("Missing permission: 'KICK_MEMBERS'");

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/guilds/%s/members/%s", ID, Member->UserRef->ID));
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't kick user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    CGuild::BanList CGuild::GetBanList()
    {
        BanMembersCheck();
        BanList Ret;

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Get(tfm::format("/guilds/%s/bans", ID));

        if(res->statusCode != 200)
            throw CDiscordClientException("Unable to get ban list. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));

        CJSON js;
        auto list = js.Deserialize<std::vector<std::string>>(res->body);

        for (auto e : list)
        {
            js.ParseObject(e);

            User user = dynamic_cast<CDiscordClient*>(m_Client)->GetUserOrAdd(js.GetValue<std::string>("user"));
            Ret.push_back({js.GetValue<std::string>("reason"), user});
        }

        return Ret;
    }

    void CGuild::BanMembersCheck()
    {
        Guild guild = dynamic_cast<CDiscordClient*>(m_Client)->GetGuild(ID);
        if(!dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::BAN_MEMBERS))
            throw CPermissionException("Missing permission: 'BAN_MEMBERS'");
    }
} // namespace DiscordBot
