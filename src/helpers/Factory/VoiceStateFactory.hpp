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

#ifndef VOICESTATEFACTORY_HPP
#define VOICESTATEFACTORY_HPP

#include "../../controller/DiscordClient.hpp"
#include "ISerializeFactory.hpp"
#include <Log.hpp>
#include <models/voice/VoiceState.hpp>

namespace DiscordBot
{
    class CVoiceStateFactory : public TSerializeFactory<CVoiceState>
    {
        public:
            CVoiceStateFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            VoiceState Deserialize(CJSON &JS) override
            {
                VoiceState Ret = VoiceState(new CVoiceState());

                Guild guild = m_Client->GetGuild(JS.GetValue<std::string>("guild_id"));
                User user = m_Client->GetUser(JS.GetValue<std::string>("user_id"));

                Ret->GuildRef = guild;

                if (user)
                    Ret->UserRef = user;

                if (Ret->GuildRef)
                {
                    auto CIT = Ret->GuildRef->Channels->find(JS.GetValue<std::string>("channel_id"));
                    if (CIT != Ret->GuildRef->Channels->end())
                        Ret->ChannelRef = CIT->second;

                    GuildMember Member;

                    //Adds this voice state to the guild member.
                    auto MIT = Ret->GuildRef->Members->find(JS.GetValue<std::string>("user_id"));
                    if (MIT != Ret->GuildRef->Members->end())
                        Member = MIT->second;
                    else
                    {
                        //Creates a new member.
                        try
                        {
                            Member = CObjectFactory::Deserialize<CGuildMember>(m_Client, JS.GetValue<std::string>("member"));
                        }
                        catch (const CJSONException &e)
                        {
                            llog << lerror << "Failed to parse JS for VoiceState member Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                            return Ret;
                        }
                    }

                    //Removes the voice state if the user isn't in a voice channel.
                    if (!Ret->ChannelRef && Member)
                    {
                        Member->State = nullptr;
                        return Ret;
                    }
                    else if(Member)
                        Member->State = Ret;
                }

                Ret->SessionID = JS.GetValue<std::string>("session_id");
                Ret->Deaf = JS.GetValue<bool>("deaf");
                Ret->Mute = JS.GetValue<bool>("mute");
                Ret->SelfDeaf = JS.GetValue<bool>("self_deaf");
                Ret->SelfMute = JS.GetValue<bool>("self_mute");
                Ret->SelfStream = JS.GetValue<bool>("self_stream");
                Ret->Supress = JS.GetValue<bool>("suppress");

                return Ret;
            }

            ~CVoiceStateFactory() = default;
    };
} // namespace DiscordBot

#endif //VOICESTATEFACTORY_HPP