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

#include <models/channels/TextChannel.hpp>
#include <JSON.hpp>
#include <Log.hpp>
#include <models/exceptions/DiscordException.hpp>
#include "../../helpers/Factory/ObjectFactory.hpp"
#include "../../helpers/MultipartFormData.hpp"
#include "../../controller/DiscordClient.hpp"
#include <tinyformat.h>

namespace DiscordBot
{
    Message CTextChannel::SendMessage(CFile &File, const std::string &Text, Embed embed, bool TTS)
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

    Message CTextChannel::SendMessage(const std::string &Text, Embed embed, bool TTS)
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

    void CTextChannel::BasicSendMessageCheck(bool NeedTTS)
    {
        if(Type != ChannelTypes::GUILD_TEXT && Type != ChannelTypes::DM)
            throw CDiscordClientException("Channel is not a text channel!");

        CheckPermissions(Permission::SEND_MESSAGES, Overwrites.load());

        if(NeedTTS)
            CheckPermissions(Permission::SEND_TTS_MESSAGES, Overwrites.load());
    }
} // namespace DiscordBot
