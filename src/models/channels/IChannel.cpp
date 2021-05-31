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

#include <models/exceptions/PermissionException.hpp>
#include <models/exceptions/DiscordException.hpp>
#include <models/ChannelProperties.hpp>
#include <models/channels/IChannel.hpp>
#include <models/Message.hpp>
#include <IDiscordClient.hpp>
#include <Log.hpp>
#include <tinyformat.h>

#include "../controller/DiscordClient.hpp"

namespace DiscordBot
{
    void IChannel::Modify(const CChannelProperties &Modifications)
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

    void IChannel::Delete(const std::string &Reason)
    {
        ManageChannelsCheck();

        CJSON js;
        js.AddPair("reason", Reason);

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Delete(tfm::format("/channels/%s", ID), js.Serialize());
        if (res->statusCode != 200)
            throw CDiscordClientException("Can't delete channel. Error: " + res->body + " HTTP Code: " + std::to_string(res->statusCode));
    }

    void IChannel::ManageChannelsCheck()
    {
        CheckPermissions(Permission::MANAGE_CHANNELS, Overwrites.load());
    }
} // namespace DiscordBot
