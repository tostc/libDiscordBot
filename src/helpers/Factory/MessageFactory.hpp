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

#ifndef MESSAGEFACTORY_HPP
#define MESSAGEFACTORY_HPP

#include "../../controller/DiscordClient.hpp"

#include "ISerializeFactory.hpp"
#include <JSON.hpp>
#include <DiscordBot/Models/Message.hpp>

namespace DiscordBot
{
    class CMessageFactory : public TSerializeFactory<CMessage>
    {
        public:
            CMessageFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            std::shared_ptr<CMessage> Deserialize(CJSON &json) override
            {
                Message Ret = Message(new CMessage(m_Client, m_Client->GetMessageManager()));

                Ret->ID = json.GetValue<std::string>("id");
                Ret->GuildRef = m_Client->GetGuild(json.GetValue<std::string>("guild_id"));
                if(Ret->GuildRef)
                {
                    auto CIT = Ret->GuildRef->Channels->find(json.GetValue<std::string>("channel_id"));
                    if (CIT != Ret->GuildRef->Channels->end() && CIT->second->Type == ChannelTypes::GUILD_TEXT)
                        Ret->ChannelRef = std::dynamic_pointer_cast<CTextChannel>(CIT->second);
                }

                //Creates a dummy object for DMs.
                if (!Ret->ChannelRef)
                {
                    // TODO: BROKEN
                    /*Ret->ChannelRef = Channel(new IChannel(m_Client));
                    Ret->ChannelRef->ID = json.GetValue<std::string>("channel_id");
                    Ret->ChannelRef->Type = ChannelTypes::DM;*/
                }

                std::string UserJson = json.GetValue<std::string>("author");
                if (!UserJson.empty())
                {
                    User user = m_Client->GetUserOrAdd(UserJson);
                    Ret->Author = user;

                    //Gets the guild member, if this message is not a dm.
                    if (Ret->GuildRef)
                    {
                        auto MIT = Ret->GuildRef->Members->find(Ret->Author->ID);
                        if (MIT != Ret->GuildRef->Members->end())
                            Ret->Member = MIT->second;
                        else
                            Ret->Member = m_Client->GetMember(Ret->GuildRef, Ret->Author->ID);
                    }
                }

                Ret->Content = json.GetValue<std::string>("content");
                Ret->Timestamp = json.GetValue<std::string>("timestamp");
                Ret->EditedTimestamp = json.GetValue<std::string>("edited_timestamp");
                Ret->Mention = json.GetValue<bool>("mention_everyone");

                std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("mentions");
                for (auto &&e : Array)
                {
                    User user = m_Client->GetUserOrAdd(e);
                    bool Found = false;

                    if (Ret->GuildRef)
                    {
                        auto MIT = Ret->GuildRef->Members->find(Ret->Author->ID);
                        if (MIT != Ret->GuildRef->Members->end())
                        {
                            Found = true;
                            Ret->Mentions.push_back(MIT->second);
                        }
                    }

                    //Create a fake Guildmember for DMs.
                    if (!Found)
                    {
                        Ret->Mentions.push_back(GuildMember(new CGuildMember(m_Client, m_Client->GetMessageManager())));
                        Ret->Mentions.back()->UserRef = user;
                    }
                }

                return Ret;
            }

            ~CMessageFactory() {}
    };
} // namespace DiscordBot


#endif //MESSAGEFACTORY_HPP