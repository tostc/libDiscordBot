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

#ifndef CHANNELFACTORY_HPP
#define CHANNELFACTORY_HPP

#include "ISerializeFactory.hpp"
#include <models/Channel.hpp>
#include "../../controller/DiscordClient.hpp"

namespace DiscordBot
{
    class CChannelFactory : public TSerializeFactory<CChannel>
    {
        public:
            CChannelFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            Channel Deserialize(CJSON &json) override
            {
                Channel Ret = Channel(new CChannel(m_Client));

                Ret->ID = json.GetValue<std::string>("id");
                Ret->Type = (ChannelTypes)json.GetValue<int>("type");
                Ret->GuildID = json.GetValue<std::string>("guild_id");
                Ret->Position = json.GetValue<int>("position");

                std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("permission_overwrites");
                for (auto &&e : Array)
                {
                    PermissionOverwrites ov = PermissionOverwrites(new CPermissionOverwrites());
                    CJSON jov;
                    jov.ParseObject(e);

                    ov->ID = jov.GetValue<std::string>("id");
                    ov->Type = jov.GetValue<std::string>("type");
                    ov->Allow = (Permission)jov.GetValue<int>("allow");
                    ov->Deny = (Permission)jov.GetValue<int>("deny");

                    Ret->Overwrites->push_back(ov);
                }

                Ret->Name = json.GetValue<std::string>("name");
                Ret->Topic = json.GetValue<std::string>("topic");
                Ret->NSFW = json.GetValue<bool>("nsfw");
                Ret->LastMessageID = json.GetValue<std::string>("last_message_id");
                Ret->Bitrate = json.GetValue<int>("bitrate");
                Ret->UserLimit = json.GetValue<int>("user_limit");
                Ret->RateLimit = json.GetValue<int>("rate_limit_per_user");

                Array = json.GetValue<std::vector<std::string>>("recipients");
                for (auto &&e : Array)
                {
                    User user = m_Client->GetUserOrAdd(e);
                    Ret->Recipients->push_back(user);
                }

                Ret->Icon = json.GetValue<std::string>("icon");
                Ret->OwnerID = json.GetValue<std::string>("owner_id");
                Ret->AppID = json.GetValue<std::string>("application_id");
                Ret->ParentID = json.GetValue<std::string>("parent_id");
                Ret->LastPinTimestamp = json.GetValue<std::string>("last_pin_timestamp");

                return Ret;
            }

            ~CChannelFactory() {}
    };
} // namespace DiscordBot


#endif //CHANNELFACTORY_HPP