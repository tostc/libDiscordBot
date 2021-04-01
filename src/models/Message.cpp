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

#include <IDiscordClient.hpp>
#include <tinyformat.h>

#include "../controller/DiscordClient.hpp"
#include "../helpers/MultipartFormData.hpp"
#include <models/Message.hpp>
#include <Log.hpp>

namespace DiscordBot
{
    std::vector<SMention> CMessage::ParseParam(const std::string &Param)
    {
        std::vector<SMention> Ret;
        std::string ID;

        const char *beg = Param.c_str();
        const char *end = beg + Param.size();

        bool BeginTagFound = false;
        MentionTypes Type;

        while (beg != end)
        {
            switch (*beg)
            {
                case '<':
                {
                    BeginTagFound = true;
                }break;

                case '>':
                {
                    if(BeginTagFound)
                        Ret.push_back({Type, ID});

                    BeginTagFound = false;
                }break;

                default:
                {
                    if(BeginTagFound)
                        ID += beg;
                }break;
            }

            beg++;
        }

        return Ret;
    }

    void CMessage::CreateReaction(const std::string &Emoji)
    {
        if(!ChannelRef || ChannelRef->Type != ChannelTypes::GUILD_TEXT && ChannelRef->Type != ChannelTypes::DM)
            return;

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Put(tfm::format("/channels/%s/messages/%s/reactions/%s/@me", ChannelRef->ID.load(), ID, UriEscape(Emoji)), "");
        llog << linfo << res->headers["X-RateLimit-Remaining"] << lendl;
        if (res->statusCode != 204)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;
    }

    void CMessage::DeleteAllReactions()
    {
        if(!ChannelRef || ChannelRef->Type != ChannelTypes::GUILD_TEXT && ChannelRef->Type != ChannelTypes::DM)
            return;

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/channels/%s/messages/%s/reactions", ChannelRef->ID.load(), ID));
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;
    }

    std::vector<User> CMessage::GetReactions(const std::string &Emoji)
    {
        std::vector<User> Ret;
        if(!ChannelRef || ChannelRef->Type != ChannelTypes::GUILD_TEXT && ChannelRef->Type != ChannelTypes::DM)
            return Ret;

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Get(tfm::format("/channels/%s/messages/%s/reactions/%s", ChannelRef->ID.load(), ID, UriEscape(Emoji)));
        llog << linfo << res->headers["X-RateLimit-Remaining"] << lendl;
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;
        else
        {
            CJSON js;
            auto arr = js.Deserialize<std::vector<std::string>>(res->body);

            for (auto &&u : arr)
            {
                User tmp;
                u >> tmp;

                Ret.push_back(tmp);
            }
        }

        return Ret;
    }

    void CMessage::Edit(const std::string &Text, Embed embed, bool TTS)
    {
        if(!ChannelRef || ChannelRef->Type != ChannelTypes::GUILD_TEXT && ChannelRef->Type != ChannelTypes::DM)
            return;

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
            json.AddJSON("embed", embed | Serialize);

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Patch(tfm::format("/channels/%s/messages/%s", ChannelRef->ID.load(), ID), json.Serialize());
        llog << linfo << res->headers["X-RateLimit-Remaining"] << lendl;

        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;
    }

    void CMessage::Delete()
    {
        if(!ChannelRef || ChannelRef->Type != ChannelTypes::GUILD_TEXT && ChannelRef->Type != ChannelTypes::DM)
            return;

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/channels/%s/messages/%s", ChannelRef->ID.load(), ID));
        if (res->statusCode != 204)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << " Body: " << res->body << lendl;
    }

    std::string CMessage::UriEscape(const std::string &Uri)
    {
        std::string Ret;
        for (auto &&c : Uri)
            Ret += "%" + tfm::format("%X", (uint8_t)c);

        return Ret;
    }
} // namespace DiscordBot