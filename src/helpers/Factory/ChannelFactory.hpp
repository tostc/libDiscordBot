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

#include "../../controller/DiscordClient.hpp"
#include "ISerializeFactory.hpp"
#include <models/channels/IChannel.hpp>
#include <models/channels/TextChannel.hpp>
#include <models/channels/VoiceChannel.hpp>

namespace DiscordBot
{
    class CChannelFactory : public TSerializeFactory<IChannel>
    {
        public:
            CChannelFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            Channel Deserialize(CJSON &json) override
            {
                ChannelTypes Type = (ChannelTypes)json.GetValue<int>("type");
                Channel Ret;

                switch (Type)
                {
                    case ChannelTypes::GUILD_TEXT:
                    {
                        Ret = ParseTextChannel(json);
                    }
                    break;
                
                    default:
                    {
                        // All unkown channels are simply IChannels.
                        Ret = Channel(new IChannel(m_Client, m_Client->GetMessageManager()));
                        ParseChannel(Ret, json);
                    } break;
                }

                return Ret;
            }

            ~CChannelFactory() {}

        private:
            Channel ParseTextChannel(CJSON &json)
            {
                TextChannel Ret = TextChannel(new CTextChannel(m_Client, m_Client->GetMessageManager()));

                ParseChannel(Ret, json);

                Ret->Topic = json.GetValue<std::string>("topic");
                Ret->NSFW = json.GetValue<bool>("nsfw");
                Ret->LastMessageID = json.GetValue<std::string>("last_message_id");
                Ret->RateLimit = json.GetValue<int>("rate_limit_per_user");
                Ret->LastPinTimestamp = json.GetValue<std::string>("last_pin_timestamp");

                return Ret;
            }

            Channel ParseVoiceChannel(CJSON &json)
            {
                VoiceChannel Ret = VoiceChannel(new CVoiceChannel(m_Client, m_Client->GetMessageManager()));

                ParseChannel(Ret, json);
                Ret->Bitrate = json.GetValue<int>("bitrate");
                Ret->UserLimit = json.GetValue<int>("user_limit");

                return Ret;
            }

            void ParseChannel(Channel c, CJSON &json)
            {
                c->ID = json.GetValue<std::string>("id");
                c->Type = (ChannelTypes)json.GetValue<int>("type");
                c->GuildID = json.GetValue<std::string>("guild_id");
                c->Position = json.GetValue<int>("position");

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

                    c->Overwrites->push_back(ov);
                }

                c->Name = json.GetValue<std::string>("name");

                Array = json.GetValue<std::vector<std::string>>("recipients");
                for (auto &&e : Array)
                {
                    User user = m_Client->GetUserOrAdd(e);
                    c->Recipients->push_back(user);
                }

                c->Icon = json.GetValue<std::string>("icon");
                c->OwnerID = json.GetValue<std::string>("owner_id");
                c->AppID = json.GetValue<std::string>("application_id");
                c->ParentID = json.GetValue<std::string>("parent_id");
            }
    };
} // namespace DiscordBot


#endif //CHANNELFACTORY_HPP