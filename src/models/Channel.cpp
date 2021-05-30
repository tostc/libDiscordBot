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

#include <JSON.hpp>
#include "../helpers/Factory/ObjectFactory.hpp"
#include "../helpers/MultipartFormData.hpp"

#include <models/PermissionException.hpp>
#include <models/DiscordException.hpp>
#include <models/ChannelProperties.hpp>
#include <models/Channel.hpp>
#include <models/Message.hpp>
#include <IDiscordClient.hpp>
#include <Log.hpp>
#include <tinyformat.h>

#include "../controller/DiscordClient.hpp"

namespace DiscordBot
{
    Message CChannel::SendMessage(CFile &File, const std::string &Text, Embed embed, bool TTS)
    {
        BasicSendMessageCheck(TTS);

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
            json.AddJSON("embed", CObjectFactory::Serialize(dynamic_cast<CDiscordClient*>(m_Client), embed));

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post(tfm::format("/channels/%s/messages", ID), CMultipartFormData::CreateFormData(File, json.Serialize()), "multipart/form-data; boundary=libDiscordBot");
        llog << linfo << res->headers["X-RateLimit-Remaining"] << lendl;
        
        if (res->statusCode != 200)
            throw CDiscordClientException("Failed to send message. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));

        return CObjectFactory::Deserialize<CMessage>(dynamic_cast<CDiscordClient*>(m_Client), res->body);
    }

    Message CChannel::SendMessage(const std::string &Text, Embed embed, bool TTS)
    {
        BasicSendMessageCheck(TTS);

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
            json.AddJSON("embed", CObjectFactory::Serialize(dynamic_cast<CDiscordClient*>(m_Client), embed));

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post(tfm::format("/channels/%s/messages", ID), json.Serialize());
        if (res->statusCode != 200)
            throw CDiscordClientException("Failed to send message. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));

        return CObjectFactory::Deserialize<CMessage>(dynamic_cast<CDiscordClient*>(m_Client), res->body);
    }

    void CChannel::Modify(const CChannelProperties &Modifications)
    {
        ManageChannelsCheck();
        CJSON js;

        auto values = Modifications.GetValues();
        bool HasOverwrites = Modifications.HasOverwrites();

        if(values.empty() && !HasOverwrites)    //Nothing to do here.
            return;

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
            auto overwrites = Modifications.GetOverwrites();
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

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Patch(tfm::format("/channels/%s", ID), js.Serialize());
        if(res->statusCode != 200)
            throw CDiscordClientException("Can't modify channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CChannel::Delete(const std::string &Reason)
    {
        ManageChannelsCheck();

        CJSON js;
        js.AddPair("reason", Reason);

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/channels/%s", ID), js.Serialize());
        if (res->statusCode != 200)
            throw CDiscordClientException("Can't delete channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void CChannel::BasicSendMessageCheck(bool NeedTTS)
    {
        if(Type != ChannelTypes::GUILD_TEXT && Type != ChannelTypes::DM)
            throw CDiscordClientException("Channel is not a text channel!");

        Guild guild = dynamic_cast<CDiscordClient*>(m_Client)->GetGuild(GuildID);
        if(!dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::SEND_MESSAGES, Overwrites.load()))
            throw CPermissionException("Missing permission: 'SEND_MESSAGES'");

        if(NeedTTS && !dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::SEND_TTS_MESSAGES, Overwrites.load()))
            throw CPermissionException("Missing permission: 'SEND_TTS_MESSAGES'");
    }

    void CChannel::ManageChannelsCheck()
    {
        Guild guild = dynamic_cast<CDiscordClient*>(m_Client)->GetGuild(GuildID);
        if(!dynamic_cast<CDiscordClient*>(m_Client)->CheckPermissions(guild, Permission::MANAGE_CHANNELS, Overwrites.load()))
            throw CPermissionException("Missing permission: 'MANAGE_CHANNELS'");
    }
} // namespace DiscordBot
