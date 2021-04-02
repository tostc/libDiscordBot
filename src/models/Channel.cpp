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
        if(Type != ChannelTypes::GUILD_TEXT && Type != ChannelTypes::DM)
            return nullptr;

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
            json.AddJSON("embed", CObjectFactory::Serialize(dynamic_cast<CDiscordClient*>(m_Client), embed));

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post(tfm::format("/channels/%s/messages", ID), CMultipartFormData::CreateFormData(File, json.Serialize()), "multipart/form-data; boundary=libDiscordBot");
        llog << linfo << res->headers["X-RateLimit-Remaining"] << lendl;
        
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;

        return CObjectFactory::Deserialize<CMessage>(dynamic_cast<CDiscordClient*>(m_Client), res->body);
    }

    Message CChannel::SendMessage(const std::string &Text, Embed embed, bool TTS)
    {
        if(Type != ChannelTypes::GUILD_TEXT && Type != ChannelTypes::DM)
            return nullptr;

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
            json.AddJSON("embed", CObjectFactory::Serialize(dynamic_cast<CDiscordClient*>(m_Client), embed));

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post(tfm::format("/channels/%s/messages", ID), json.Serialize());
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;

        return CObjectFactory::Deserialize<CMessage>(dynamic_cast<CDiscordClient*>(m_Client), res->body);
    }
} // namespace DiscordBot
