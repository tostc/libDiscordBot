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
        CheckPermissions(Permission::MANAGE_CHANNELS);

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

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::POST, CreateHttpMessage(tfm::format("/guilds/%s/channels", ID), js.Serialize()));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 201)
            throw CDiscordClientException("Can't create channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    
        // Returns the new created channel.
        js.ParseObject(res->body);
        auto IT = Channels->find(js.GetValue<std::string>("id"));
        return IT->second;
    }

    void CGuild::Ban(GuildMember Member, const std::string &Reason, int DeleteMsgDays)
    {
        CheckPermissions(Permission::BAN_MEMBERS);

        CJSON js;
        if(!Reason.empty())
            js.AddPair("reason", Reason);

        if(DeleteMsgDays != -1)
            js.AddPair("delete_message_days", DeleteMsgDays);

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::PUT, CreateHttpMessage(tfm::format("/guilds/%s/bans/%s", ID, Member->UserRef->ID), js.Serialize()));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't ban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuild::Unban(User user)
    {
        CheckPermissions(Permission::BAN_MEMBERS);

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::DELETE, CreateHttpMessage(tfm::format("/guilds/%s/bans/%s", ID, user->ID), ""));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't unban user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CGuild::Kick(GuildMember Member)
    {   
        CheckPermissions(Permission::KICK_MEMBERS);

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::DELETE, CreateHttpMessage(tfm::format("/guilds/%s/members/%s", ID, Member->UserRef->ID), ""));
        auto res = req->Value<ix::HttpResponsePtr>();
        if(res->statusCode != 204)
            throw CDiscordClientException("Can't kick user. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    CGuild::BanList CGuild::GetBanList()
    {
        CheckPermissions(Permission::BAN_MEMBERS);
        BanList Ret;

        auto req = m_MsgMgr->RequestMessage(Internal::Requests::GET, CreateHttpMessage(tfm::format("/guilds/%s/bans", ID), ""));
        auto res = req->Value<ix::HttpResponsePtr>();
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
} // namespace DiscordBot
