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

#include "DiscordClient.hpp"
#include <iostream>
#include <sodium.h>
#include "Helper.hpp"

#define CLOG_IMPLEMENTATION
#include <Log.hpp>

#ifdef DISCORDBOT_UNIX
#include <signal.h>
#endif

namespace DiscordBot
{
    /**
     * @param Token: Your Discord bot token. Which you have created <a href="https://discordapp.com/developers/applications">here</a>.
     * 
     * @return Returns a new DiscordClient object.
     */
    DiscordClient IDiscordClient::Create(const std::string &Token, Intent Intents)
    {
        //Needed for windows.
        ix::initNetSystem();

        //Initialize libsodium.
        if (sodium_init() < 0) 
            llog << lerror << "Error to init libsodium" << lendl;

        return DiscordClient(new CDiscordClient(Token, Intents));
    }

    CDiscordClient::CDiscordClient(const std::string &Token, Intent Intents) : m_Intents(Intents), m_Token(Token), m_Terminate(false), m_HeartACKReceived(false), m_Quit(false), m_LastSeqNum(-1) 
    {
#ifdef DISCORDBOT_UNIX
        //Ignores the SIGPIPE signal.
        signal(SIGPIPE, SIG_IGN);
#endif

        m_EVManger.SubscribeMessage(QUEUE_NEXT_SONG, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));  
        m_EVManger.SubscribeMessage(RESUME, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));  
        m_EVManger.SubscribeMessage(RECONNECT, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));   

        //Disable client side checking.
        ix::SocketTLSOptions DisabledTrust;
        DisabledTrust.caFile = "NONE";

        m_HTTPClient.setTLSOptions(DisabledTrust);
        m_Socket.setTLSOptions(DisabledTrust);
    }

    /**
     * @brief Joins or leaves a voice channel.
     */
    void CDiscordClient::ChangeVoiceState(const std::string &Guild, const std::string &Channel)
    {
        CJSON json;
        json.AddPair("guild_id", Guild);

        if(!Channel.empty())
            json.AddPair("channel_id", Channel);
        else
            json.AddPair("channel_id", nullptr);

        json.AddPair("self_mute", false);
        json.AddPair("self_deaf", false);

        SendOP(OPCodes::VOICE_STATE_UPDATE, json.Serialize());
    }

    /**
     * @brief Joins a audio channel.
     * 
     * @param channel: The voice channel to join.
     */
    void CDiscordClient::Join(Channel channel)
    {
        if (!channel || channel->GuildID.empty() || channel->ID.empty())
            return;

        ChangeVoiceState(channel->GuildID, channel->ID);
    }

    /**
     * @brief Leaves the audio channel.
     * 
     * @param guild: The guild to leave the voice channel.
     */
    void CDiscordClient::Leave(Guild guild)
    {
        if (!guild)
            return;

        ChangeVoiceState(guild->ID);
    }

    /**
     * @brief Sends a message to a given channel.
     * 
     * @param channel: Text channel which will receive the message.
     * @param Text: Text to send;
     * @param TTS: True to enable tts.
     */
    void CDiscordClient::SendMessage(Channel channel, const std::string Text, Embed embed, bool TTS)
    {
        if(channel->Type != ChannelTypes::GUILD_TEXT && channel->Type != ChannelTypes::DM)
            return;

        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Add the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["Content-Type"] = "application/json";

        CJSON json;
        json.AddPair("content", Text);
        json.AddPair("tts", TTS);

        if(embed)
        {
            CJSON jembed;
            jembed.AddPair("title", embed->Title);
            jembed.AddPair("description", embed->Description);

            if(!embed->URL.empty())
                jembed.AddPair("url", embed->URL);

            if(!embed->Type.empty())
                jembed.AddPair("type", embed->Type);

            json.AddJSON("embed", jembed.Serialize());
        }

        auto res = m_HTTPClient.post(std::string(BASE_URL) + "/channels/" + channel->ID + "/messages", json.Serialize(), args);
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << lendl;
    }

    /**
     * @brief Sends a message to a given user.
     * 
     * @param user: Userwhich will receive the message.
     * @param Text: Text to send;
     * @param TTS: True to enable tts.
     */
    void CDiscordClient::SendMessage(User user, const std::string Text, Embed embed, bool TTS)
    {
        CJSON json;
        json.AddPair("recipient_id", user->ID);

        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Add the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["Content-Type"] = "application/json";

        auto res = m_HTTPClient.post(std::string(BASE_URL) + "/users/@me/channels", json.Serialize(), args);
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << lendl;
        else
        {
            json.ParseObject(res->payload);
            Channel c = CreateChannel(json);

            SendMessage(c, Text, embed, TTS);
        }
    }

    /**
     * @return Returns the audio source for the given guild. Null if there is no audio source available.
     */
    AudioSource CDiscordClient::GetAudioSource(Guild guild)
    {
        if(!guild)
            return AudioSource();

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(guild->ID);
        if (IT != m_VoiceSockets.end())
            return IT->second->GetAudioSource();

        return AudioSource();
    }   

    /**
     * @return Returns the music queue for the given guild. Null if there is no music queue available.
     */
    MusicQueue CDiscordClient::GetMusicQueue(Guild guild)
    {
        if(!guild)
            return MusicQueue();

        std::lock_guard<std::mutex> lock(m_MusicQueueLock);
        auto IT = m_MusicQueues.find(guild->ID);
        if (IT != m_MusicQueues.end())
            return IT->second;

        return MusicQueue();
    }

    /**
     * @return Returns true if a audio source is playing in the given guild.
     */
    bool CDiscordClient::IsPlaying(Guild guild)
    {
        return GetAudioSource(guild) != nullptr;
    }

    /**
     * @brief Runs the bot. The call returns if you calls Quit(). @see Quit()
     */
    void CDiscordClient::Run()
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Add the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;

        //Requests the gateway endpoint for bots.
        auto res = m_HTTPClient.get(BASE_URL + std::string("/gateway/bot"), args);
        if (res->statusCode == 200)
        {
            try
            {
                CJSON json;
                m_Gateway = json.Deserialize<std::shared_ptr<SGateway>>(res->payload);
            }
            catch (const CJSONException &e)
            {
                llog << lerror << "Failed to parse JSON Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                return;
            }

            //Connects to discords websocket.
            m_Socket.setUrl(m_Gateway->URL + "/?v=6&encoding=json");
            m_Socket.setOnMessageCallback(std::bind(&CDiscordClient::OnWebsocketEvent, this, std::placeholders::_1));
            m_Socket.start();

            //Runs until the bot quits.
            while (!m_Quit)
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        else
            llog << lerror << "HTTP " << res->statusCode << " Error " << res->errorMsg << lendl;
    }

    /**
     * @brief Quits the bot. And disconnects all voice states.
     */
    void CDiscordClient::Quit()
    {
        auto IT = m_Guilds.begin();
        while (IT != m_Guilds.end())
        {
            Leave(IT->second);
            IT++;
        }

        m_Terminate = true;
        if (m_Heartbeat.joinable())
            m_Heartbeat.join();

        m_Socket.stop();
        
        if (m_Controller)
        {
            m_Controller->OnDisconnect();
            m_Controller->OnQuit();
            m_Controller = nullptr;
        }

        m_Guilds.clear();
        m_VoiceSockets.clear();
        m_AudioSources.clear();
        m_Users.clear();
        m_MusicQueues.clear();
        m_Quit = true;
    }

    /**
     * @brief Adds a song to the music queue.
     * 
     * @param guild: The guild which is associated with the queue.
     * @param Info: Song informations.
     */
    void CDiscordClient::AddToQueue(Guild guild, SongInfo Info)
    {
        if(!guild)
            return;

        std::lock_guard<std::mutex> lock(m_MusicQueueLock);

        auto IT = m_MusicQueues.find(guild->ID);
        if(IT != m_MusicQueues.end())
            IT->second->AddSong(Info);
        else if(m_QueueFactory)
        {
            auto Tmp = m_QueueFactory->Create();
            Tmp->SetGuildID(guild->ID);
            Tmp->SetOnWaitFinishCallback(std::bind(&CDiscordClient::OnQueueWaitFinish, this, std::placeholders::_1, std::placeholders::_2));
            Tmp->AddSong(Info);
            m_MusicQueues[guild->ID] = Tmp;
        }
    }

    /**
     * @brief Connects to the given channel and uses the queue to speak.
     * 
     * @param channel: The voice channel to connect to.
     * 
     * @return Returns true if the connection succeeded.
     */
    bool CDiscordClient::StartSpeaking(Channel channel)
    {
        if (!channel || channel->GuildID.empty())
            return false;

        AudioSource Source;

        std::lock_guard<std::mutex> lock(m_MusicQueueLock);
        auto IT = m_MusicQueues.find(channel->GuildID);
        if(IT != m_MusicQueues.end())
        {
            if(IT->second->HasNext())
                Source = IT->second->Next();
            else
                IT->second->ClearQueue();
        }

        return StartSpeaking(channel, Source);
    }

    /**
     * @brief Connects to the given channel and uses the source to speak.
     * 
     * @param channel: The voice channel to connect to.
     * @param source: The audio source for speaking.
     * 
     * @return Returns true if the connection succeeded.
     */
    bool CDiscordClient::StartSpeaking(Channel channel, AudioSource source)
    {
        if (!channel || channel->GuildID.empty())
            return false;

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(channel->GuildID);
        if (IT != m_VoiceSockets.end() && source)
            IT->second->StartSpeaking(source);
        else if(source)
        {
            Join(channel);

            std::lock_guard<std::mutex> lock(m_AudioSourcesLock);
            m_AudioSources[channel->GuildID] = source;
        }

        return true;
    }

    /**
     * @brief Pauses the audio source. @see ResumeSpeaking to continue streaming.
     * 
     * @param guild: The guild to pause.
     */
    void CDiscordClient::PauseSpeaking(Guild guild)
    {
        if(!guild)
            return;

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(guild->ID);
        if (IT != m_VoiceSockets.end())
            IT->second->PauseSpeaking();
    }

    /**
     * @brief Resumes the audio source.
     * 
     * @param guild: The guild to resume.
     */
    void CDiscordClient::ResumeSpeaking(Guild guild) 
    {
        if(!guild)
            return;

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(guild->ID);
        if (IT != m_VoiceSockets.end())
            IT->second->ResumeSpeaking();
    }

    /**
     * @brief Stops the audio source.
     * 
     * @param guild: The guild to stop.
     */
    void CDiscordClient::StopSpeaking(Guild guild)
    {
        if(!guild)
            return;

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(guild->ID);
        if (IT != m_VoiceSockets.end())
            IT->second->StopSpeaking();
    }

    /**
     * @brief Removes a song from the queue by its index.
     */
    void CDiscordClient::RemoveSong(Channel channel, size_t Index)
    {
        if (!channel || channel->GuildID.empty())
            return;

        std::lock_guard<std::mutex> lock(m_MusicQueueLock);
        auto IT = m_MusicQueues.find(channel->GuildID);
        if(IT != m_MusicQueues.end())
            IT->second->RemoveSong(Index);
    }

    /**
     * @brief Removes a song from the queue by its title or part of the title.
     */
    void CDiscordClient::RemoveSong(Channel channel, const std::string &Name)
    {
        if (!channel || channel->GuildID.empty())
            return;

        std::lock_guard<std::mutex> lock(m_MusicQueueLock);
        auto IT = m_MusicQueues.find(channel->GuildID);
        if(IT != m_MusicQueues.end())
            IT->second->RemoveSong(Name);
    }

    void CDiscordClient::OnMessageReceive(MessageBase Msg)
    {
        switch (Msg->Event)
        {
            case QUEUE_NEXT_SONG:
            {
                auto Data = std::static_pointer_cast<TMessage<std::string>>(Msg);

                AudioSource Source;

                std::lock_guard<std::mutex> lock(m_MusicQueueLock);
                auto MQIT = m_MusicQueues.find(Data->Value);
                if(MQIT != m_MusicQueues.end())
                {
                    if(MQIT->second->HasNext())
                        Source = MQIT->second->Next();
                    else
                        MQIT->second->ClearQueue();
                }

                if(Source)
                {
                    std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
                    auto IT = m_VoiceSockets.find(Data->Value);
                    if(IT != m_VoiceSockets.end())
                        IT->second->StartSpeaking(Source);
                }
            }break;

            case RESUME:
            {
                m_Socket.start();
            }break;

            case RECONNECT:
            {
                m_SessionID.clear();
                m_Socket.start();
            }break;
        }
    }

    /**
     * @brief Receives all websocket events from discord. This is the heart of the bot.
     */
    void CDiscordClient::OnWebsocketEvent(const ix::WebSocketMessagePtr &msg)
    {
        switch (msg->type)
        {
            case ix::WebSocketMessageType::Open:
            {
                llog << linfo << "Websocket opened URI: " << msg->openInfo.uri << " Protocol: " << msg->openInfo.protocol << lendl;
            }break;

            case ix::WebSocketMessageType::Error:
            {
                llog << lerror << "Websocket error " << msg->errorInfo.reason << lendl;
            }break;

            case ix::WebSocketMessageType::Close:
            {
                m_Terminate = true;
                m_HeartACKReceived = false;
                llog << linfo << "Websocket closed code " << msg->closeInfo.code << " Reason " << msg->closeInfo.reason << lendl;
            }break;

            case ix::WebSocketMessageType::Message:
            {
                CJSON json;
                SPayload Pay;

                try
                {
                    Pay = json.Deserialize<SPayload>(msg->str);
                }
                catch (const CJSONException &e)
                {
                    llog << lerror << "Failed to parse JSON Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                    return;
                }

                switch ((OPCodes)Pay.OP)
                {
                    case OPCodes::DISPATCH:
                    {
                        m_LastSeqNum = Pay.S;
                        std::hash<std::string> hash;

                        //Gateway Events https://discordapp.com/developers/docs/topics/gateway#commands-and-events-gateway-events
                        switch (Adler32(Pay.T.c_str()))
                        {
                            //Called after the handshake is completed.
                            case Adler32("READY"):
                            {
                                json.ParseObject(Pay.D);
                                m_SessionID = json.GetValue<std::string>("session_id");

                                json.ParseObject(json.GetValue<std::string>("user"));
                                m_BotUser = CreateUser(json);

                                llog << linfo << "Connected with Discord! " << m_Socket.getUrl() << lendl;

                                if (m_Controller)
                                    m_Controller->OnReady();
                            }
                            break;

                            /*------------------------GUILDS Intent------------------------*/

                            case Adler32("GUILD_CREATE"):
                            {
                                json.ParseObject(Pay.D);

                                Guild guild = Guild(new CGuild());
                                guild->ID = json.GetValue<std::string>("id");
                                guild->Name = json.GetValue<std::string>("name");

                                //Get all Roles;
                                std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("roles");
                                for (auto &&e : Array)
                                {
                                    CJSON jRole;
                                    jRole.ParseObject(e);

                                    Role Tmp = CreateRole(jRole);
                                    guild->Roles[Tmp->ID] = Tmp;
                                }

                                //Get all Channels;
                                Array = json.GetValue<std::vector<std::string>>("channels");
                                for (auto &&e : Array)
                                {
                                    CJSON jChannel;
                                    jChannel.ParseObject(e);

                                    Channel Tmp = CreateChannel(jChannel);
                                    Tmp->GuildID = guild->ID;
                                    guild->Channels[Tmp->ID] = Tmp;
                                }

                                //Get all members.
                                Array = json.GetValue<std::vector<std::string>>("members");
                                for (auto &&e : Array)
                                {
                                    CJSON Member;
                                    Member.ParseObject(e);

                                    GuildMember Tmp = CreateMember(Member, guild);

                                    // if (Tmp->UserRef)
                                    //     guild->Members[Tmp->UserRef->ID] = Tmp;
                                }

                                //Get all voice states.
                                Array = json.GetValue<std::vector<std::string>>("voice_states");
                                for (auto &&e : Array)
                                {
                                    CJSON State;
                                    State.ParseObject(e);

                                    CreateVoiceState(State, guild);
                                }

                                //Gets the owner object.
                                std::string OwnerID = json.GetValue<std::string>("owner_id");
                                guild->Owner = GetMember(guild, OwnerID);
                                m_Guilds[guild->ID] = guild;
                            }break;

                            case Adler32("GUILD_DELETE"):
                            {
                                json.ParseObject(Pay.D);

                                std::lock_guard<std::mutex> lock(m_MusicQueueLock);
                                std::lock_guard<std::mutex> lock1(m_VoiceSocketsLock);

                                m_VoiceSockets.erase(json.GetValue<std::string>("id"));
                                m_MusicQueues.erase(json.GetValue<std::string>("id"));
                                m_Guilds.erase(json.GetValue<std::string>("id"));

                                llog << linfo << "GUILD_DELETE" << lendl;
                            }break;

                            /*------------------------GUILDS Intent------------------------*/

                            /*------------------------GUILD_MEMBERS Intent------------------------*/
                            //ATTENTION: NEEDS "Server Members Intent" ACTIVATED TO WORK, OTHERWISE THE BOT FAIL TO CONNECT AND A ERROR IS WRITTEN TO THE CONSOLE!!!

                            case Adler32("GUILD_MEMBER_ADD"):
                            {
                                CJSON Member;
                                Member.ParseObject(Pay.D);

                                std::string GuildID = Member.GetValue<std::string>("guild_id");

                                auto IT = m_Guilds.find(GuildID);
                                if(IT != m_Guilds.end())
                                {
                                    Guild guild = IT->second;//m_Guilds[GuildID];
                                    GuildMember Tmp = CreateMember(Member, guild);

                                    if(m_Controller)
                                        m_Controller->OnMemberAdd(guild, Tmp);
                                }
                                else
                                    llog << ldebug << "Invalid Guild ( " << GuildID << " ) " << lendl;
                            }break;

                            case Adler32("GUILD_MEMBER_UPDATE"):
                            {
                                json.ParseObject(Pay.D);
                                std::string GuildID = json.GetValue<std::string>("guild_id");
                                std::string Premium = json.GetValue<std::string>("premium_since");
                                std::string Nick = json.GetValue<std::string>("nick");
                                std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("roles");

                                json.ParseObject(json.GetValue<std::string>("user"));
                                std::string UserID = json.GetValue<std::string>("id");

                                auto GIT = m_Guilds.find(GuildID);
                                if(GIT != m_Guilds.end())
                                {
                                    Guild guild = GIT->second;//m_Guilds[GuildID];
                                    auto IT = guild->Members.find(UserID);
                                    if(IT != guild->Members.end())
                                    {
                                        IT->second->Roles.clear();
                                        for (auto &&e : Array)
                                            IT->second->Roles.push_back(guild->Roles[e]);                               

                                        IT->second->Nick = Nick;
                                        IT->second->PremiumSince = Premium;

                                        if(m_Controller)
                                            m_Controller->OnMemberUpdate(guild, IT->second);
                                    } 
                                }
                                else
                                    llog << ldebug << "Invalid Guild ( " << GuildID << " ) " << lendl;
                            }break;

                            case Adler32("GUILD_MEMBER_REMOVE"):
                            {
                                json.ParseObject(Pay.D);
                                std::string GuildID = json.GetValue<std::string>("guild_id");

                                json.ParseObject(json.GetValue<std::string>("user"));
                                std::string UserID = json.GetValue<std::string>("id");

                                auto GIT = m_Guilds.find(GuildID);
                                if(GIT != m_Guilds.end())
                                {
                                    Guild guild = GIT->second;//m_Guilds[GuildID];

                                    auto IT = guild->Members.find(UserID);
                                    if(IT != guild->Members.end())
                                    {
                                        GuildMember member = IT->second;
                                        guild->Members.erase(IT);

                                        if(m_Controller)
                                            m_Controller->OnMemberRemove(guild, member);
                                    }                                

                                    if(m_Users.find(UserID) != m_Users.end())
                                    {
                                        if(m_Users[UserID].use_count() == 1)
                                            m_Users.erase(UserID);
                                    }
                                }
                                else
                                    llog << ldebug << "Invalid Guild ( " << GuildID << " ) " << lendl;
                            }break;

                            /*------------------------GUILD_MEMBERS Intent------------------------*/

                            /*------------------------GUILD_VOICE_STATES Intent------------------------*/

                            case Adler32("VOICE_STATE_UPDATE"):
                            {
                                json.ParseObject(Pay.D);
                                VoiceState Tmp = CreateVoiceState(json, nullptr);

                                if (m_Controller && Tmp->GuildRef)
                                {
                                    if(Tmp->UserRef)
                                    {
                                        if(Tmp->UserRef->ID == m_BotUser->ID && !Tmp->ChannelRef)
                                        {
                                            std::lock_guard<std::mutex> lock(m_MusicQueueLock);
                                            std::lock_guard<std::mutex> lock1(m_VoiceSocketsLock);

                                            m_VoiceSockets.erase(Tmp->GuildRef->ID);
                                            m_MusicQueues.erase(Tmp->GuildRef->ID);
                                        }

                                        auto IT = Tmp->GuildRef->Members.find(Tmp->UserRef->ID);
                                        if(IT != Tmp->GuildRef->Members.end())
                                            m_Controller->OnVoiceStateUpdate(Tmp->GuildRef, IT->second);
                                    }
                                }   
                            }break;

                            /*------------------------GUILD_VOICE_STATES Intent------------------------*/

                            //Called if your bot joins a voice channel.
                            case Adler32("VOICE_SERVER_UPDATE"):
                            {
                                json.ParseObject(Pay.D);
                                Guilds::iterator GIT = m_Guilds.find(json.GetValue<std::string>("guild_id"));
                                if (GIT != m_Guilds.end())
                                {
                                    auto UIT = GIT->second->Members.find(m_BotUser->ID);
                                    if (UIT != GIT->second->Members.end())
                                    {
                                        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
                                        VoiceSocket Socket = VoiceSocket(new CVoiceSocket(json, UIT->second->State->SessionID, m_BotUser->ID));
                                        Socket->SetOnSpeakFinish(std::bind(&CDiscordClient::OnSpeakFinish, this, std::placeholders::_1));
                                        m_VoiceSockets[GIT->second->ID] = Socket;

                                        //Creates a music queue for the server.
                                        if(m_QueueFactory)
                                        {
                                            std::lock_guard<std::mutex> lock(m_MusicQueueLock);
                                            if(m_MusicQueues.find(GIT->second->ID) == m_MusicQueues.end())
                                            {
                                                MusicQueue MQ = m_QueueFactory->Create();
                                                MQ->SetGuildID(GIT->second->ID);
                                                MQ->SetOnWaitFinishCallback(std::bind(&CDiscordClient::OnQueueWaitFinish, this, std::placeholders::_1, std::placeholders::_2));
                                                m_MusicQueues[GIT->second->ID] = MQ;
                                            }
                                        }

                                        //Plays the queued audiosource.
                                        std::lock_guard<std::mutex> lock1(m_AudioSourcesLock);
                                        AudioSources::iterator IT = m_AudioSources.find(GIT->second->ID);
                                        if (IT != m_AudioSources.end())
                                        {
                                            Socket->StartSpeaking(IT->second);
                                            m_AudioSources.erase(IT);
                                        }
                                    }
                                }
                            }break;

                            /*------------------------GUILD_MESSAGES Intent------------------------*/

                            case Adler32("MESSAGE_CREATE"):
                            {
                                json.ParseObject(Pay.D);
                                Message msg = CreateMessage(json);

                                if (m_Controller)
                                    m_Controller->OnMessage(msg);
                            }break;

                            /*------------------------GUILD_MESSAGES Intent------------------------*/

                            //Called if a session resumed.
                            case Adler32("RESUMED"):
                            {
                                llog << linfo << "Resumed" << lendl;

                                if (m_Controller)
                                    m_Controller->OnResume();
                            } break;
                        }
                }break;

                case OPCodes::HELLO:
                {
                    try
                    {
                        json.ParseObject(Pay.D);
                        m_HeartbeatInterval = json.GetValue<uint32_t>("heartbeat_interval");
                    }
                    catch (const CJSONException &e)
                    {
                        llog << lerror << "Failed to parse JSON Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                        return;
                    }

                    if (m_SessionID.empty())
                        SendIdentity();
                    else
                        SendResume();

                    m_HeartACKReceived = true;
                    m_Terminate = false;

                    if (m_Heartbeat.joinable())
                        m_Heartbeat.join();

                    m_Heartbeat = std::thread(&CDiscordClient::Heartbeat, this);
                }break;

                case OPCodes::HEARTBEAT_ACK:
                {
                    m_HeartACKReceived = true;
                }break;

                //Something is wrong.
                case OPCodes::INVALID_SESSION:
                {
                    if (Pay.D == "true")
                        SendResume();
                    else
                    {
                        //TODO: Maybe deadlock. Let's find out.
                        llog << linfo << "INVALID_SESSION CLOSE SOCKET" << lendl;
                        m_Socket.close();
                        llog << linfo << "INVALID_SESSION SOCKET CLOSED" << lendl;
                        m_EVManger.PostMessage(RECONNECT, 0, 5000);
                    }
                        //Quit();

                    llog << linfo << "INVALID_SESSION" << lendl;
                }break;
                }
            }break;
        }
    }

    /**
     * @brief Sends a heartbeat.
     */
    void CDiscordClient::Heartbeat()
    {
        while (!m_Terminate)
        {
            //Start a reconnect.
            if (!m_HeartACKReceived)
            {
                m_Socket.stop();

                m_Users.clear();
                m_Guilds.clear();

                std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
                m_VoiceSockets.clear();

                if (m_Controller)
                    m_Controller->OnDisconnect();

                m_Terminate = true;
                m_EVManger.PostMessage(RESUME, 0, 100);

                break;
            }

            SendOP(OPCodes::HEARTBEAT, m_LastSeqNum != -1 ? std::to_string(m_LastSeqNum) : "");
            m_HeartACKReceived = false;

            // Terminateable timeout.
            int64_t Beg = GetTimeMillis();
            while (((GetTimeMillis() - Beg) < m_HeartbeatInterval) && !m_Terminate)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            // std::this_thread::sleep_for(std::chrono::milliseconds(m_HeartbeatInterval));
        }
    }

    /**
     * @brief Builds and sends a payload object.
     */
    void CDiscordClient::SendOP(CDiscordClient::OPCodes OP, const std::string &D)
    {
        SPayload Pay;
        Pay.OP = (uint32_t)OP;
        Pay.D = D;

        try
        {
            CJSON json;
            m_Socket.send(json.Serialize(Pay));
        }
        catch (const CJSONException &e)
        {
            llog << lerror << "Failed to serialize the Payload object. Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
        }
    }

    /**
     * @brief Sends the identity.
     */
    void CDiscordClient::SendIdentity()
    {
        SIdentify id;
        id.Token = m_Token;
        id.Properties["$os"] = "linux";
        id.Properties["$browser"] = "libDiscordBot";
        id.Properties["$device"] = "libDiscordBot";
        id.Intents = m_Intents;

        CJSON json;
        SendOP(OPCodes::IDENTIFY, json.Serialize(id));
    }

    /**
     * @brief Sends a resume request.
     */
    void CDiscordClient::SendResume()
    {
        SResume resume;
        resume.Token = m_Token;
        resume.SessionID = m_SessionID;
        resume.Seq = m_LastSeqNum;

        CJSON json;
        SendOP(OPCodes::RESUME, json.Serialize(resume));
    }

    /**
     * @brief Called from voice socket if a audio source finished.
     */
    void CDiscordClient::OnSpeakFinish(const std::string &Guild)
    {
        if(m_Controller)
        {
            m_EVManger.PostMessage(QUEUE_NEXT_SONG, Guild);

            auto IT = m_Guilds.find(Guild);
            if(IT != m_Guilds.end())
                m_Controller->OnEndSpeaking(IT->second);
        }
    }

    void CDiscordClient::OnQueueWaitFinish(const std::string &Guild, AudioSource Source)
    {
        if(!Source)
        {
            m_EVManger.PostMessage(QUEUE_NEXT_SONG, Guild);
            return;
        }

        std::lock_guard<std::mutex> lock(m_VoiceSocketsLock);
        VoiceSockets::iterator IT = m_VoiceSockets.find(Guild);
        if(IT != m_VoiceSockets.end())
            IT->second->StartSpeaking(Source);
    }

    GuildMember CDiscordClient::GetMember(Guild guild, const std::string &UserID)
    {
        auto UserIT = guild->Members.find(UserID);
        GuildMember Ret;

        if(UserIT != guild->Members.end())
            Ret = UserIT->second;
        else
        {
            ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

            //Add the bot token.
            args->extraHeaders["Authorization"] = "Bot " + m_Token;

            auto res = m_HTTPClient.get(std::string(BASE_URL) + "/guilds/" + guild->ID + "/members/" + UserID, args);
            if (res->statusCode != 200)
                llog << lerror << "Failed to receive owner info HTTP: " << res->statusCode << " MSG: " << res->errorMsg << lendl;
            else
            {
                try
                {    
                    CJSON JOwner;
                    JOwner.ParseObject(res->payload);

                    Ret = CreateMember(JOwner, guild);
                }
                catch (const CJSONException &e)
                {
                    llog << lerror << "Failed to parse owner JSON Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                    return nullptr;
                }
            }
        }

        return Ret;
    }

    User CDiscordClient::CreateUser(CJSON &json)
    {
        User Ret = User(new CUser());

        Ret->ID = json.GetValue<std::string>("id");
        Ret->Username = json.GetValue<std::string>("username");
        Ret->Discriminator = json.GetValue<std::string>("discriminator");
        Ret->Avatar = json.GetValue<std::string>("avatar");
        Ret->Bot = json.GetValue<bool>("bot");
        Ret->System = json.GetValue<bool>("system");
        Ret->MFAEnabled = json.GetValue<bool>("mfa_enabled");
        Ret->Locale = json.GetValue<std::string>("locale");
        Ret->Verified = json.GetValue<bool>("verified");
        Ret->Email = json.GetValue<std::string>("email");
        Ret->Flags = (UserFlags)json.GetValue<int>("flags");
        Ret->PremiumType = (PremiumTypes)json.GetValue<int>("premium_type");
        Ret->PublicFlags = (UserFlags)json.GetValue<int>("public_flags");

        m_Users[Ret->ID] = Ret;

        return Ret;
    }

    GuildMember CDiscordClient::CreateMember(CJSON &json, Guild guild)
    {
        GuildMember Ret = GuildMember(new CGuildMember());
        std::string UserInfo = json.GetValue<std::string>("user");
        User member;

        //Gets the user which is associated with the member.
        if (!UserInfo.empty())
        {
            CJSON user;
            user.ParseObject(UserInfo);

            Users::iterator IT = m_Users.find(user.GetValue<std::string>("id"));
            if (IT != m_Users.end())
                member = IT->second;
            else
                member = CreateUser(user);
        }

        Ret->UserRef = member;
        Ret->Nick = json.GetValue<std::string>("nick");
        Ret->JoinedAt = json.GetValue<std::string>("joined_at");
        Ret->PremiumSince = json.GetValue<std::string>("premium_since");
        Ret->Deaf = json.GetValue<bool>("deaf");
        Ret->Mute = json.GetValue<bool>("mute");

        //Adds the roles
        auto Array = json.GetValue<std::vector<std::string>>("roles");
        for (auto &&e : Array)
        {
            auto RIT = guild->Roles.find(e);
            if(RIT != guild->Roles.end())
                Ret->Roles.push_back(RIT->second);
        }

        if (Ret->UserRef)
            guild->Members[Ret->UserRef->ID] = Ret;

        return Ret;
    }

    VoiceState CDiscordClient::CreateVoiceState(CJSON &json, Guild guild)
    {
        VoiceState Ret = VoiceState(new CVoiceState());

        if (!guild)
        {
            Guilds::iterator IT = m_Guilds.find(json.GetValue<std::string>("guild_id"));
            if (IT != m_Guilds.end())
                Ret->GuildRef = IT->second;
        }
        else
            Ret->GuildRef = guild;

        auto IT = m_Users.find(json.GetValue<std::string>("user_id"));
        if (IT != m_Users.end())
            Ret->UserRef = IT->second;

        if (Ret->GuildRef)
        {
            auto CIT = Ret->GuildRef->Channels.find(json.GetValue<std::string>("channel_id"));
            if (CIT != Ret->GuildRef->Channels.end())
                Ret->ChannelRef = CIT->second;

            GuildMember Member;

            //Adds this voice state to the guild member.
            auto MIT = Ret->GuildRef->Members.find(json.GetValue<std::string>("user_id"));
            if (MIT != Ret->GuildRef->Members.end())
                Member = MIT->second;
            else
            {
                //Creates a new member.
                try
                {
                    CJSON JMember;
                    JMember.ParseObject(json.GetValue<std::string>("member"));

                    Member = CreateMember(JMember, Ret->GuildRef);
                }
                catch (const CJSONException &e)
                {
                    llog << lerror << "Failed to parse JSON for VoiceState member Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
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

        Ret->SessionID = json.GetValue<std::string>("session_id");
        Ret->Deaf = json.GetValue<bool>("deaf");
        Ret->Mute = json.GetValue<bool>("mute");
        Ret->SelfDeaf = json.GetValue<bool>("self_deaf");
        Ret->SelfMute = json.GetValue<bool>("self_mute");
        Ret->SelfStream = json.GetValue<bool>("self_stream");
        Ret->Supress = json.GetValue<bool>("suppress");

        return Ret;
    }

    Channel CDiscordClient::CreateChannel(CJSON &json)
    {
        Channel Ret = Channel(new CChannel());

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

            Ret->Overwrites.push_back(ov);
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
            CJSON juser;
            juser.ParseObject(e);

            Users::iterator IT = m_Users.find(juser.GetValue<std::string>("id"));
            User user;

            if (IT != m_Users.end())
                user = IT->second;
            else
                user = CreateUser(juser);

            Ret->Recipients.push_back(user);
        }

        Ret->Icon = json.GetValue<std::string>("icon");
        Ret->OwnerID = json.GetValue<std::string>("owner_id");
        Ret->AppID = json.GetValue<std::string>("application_id");
        Ret->ParentID = json.GetValue<std::string>("parent_id");
        Ret->LastPinTimestamp = json.GetValue<std::string>("last_pin_timestamp");

        return Ret;
    }

    Message CDiscordClient::CreateMessage(CJSON &json)
    {
        Message Ret = Message(new CMessage());
        Channel channel;

        Guilds::iterator IT = m_Guilds.find(json.GetValue<std::string>("guild_id"));
        if (IT != m_Guilds.end())
        {
            Ret->GuildRef = IT->second;
            std::map<std::string, Channel>::iterator CIT = Ret->GuildRef->Channels.find(json.GetValue<std::string>("channel_id"));
            if (CIT != Ret->GuildRef->Channels.end())
                channel = CIT->second;
        }

        //Creates a dummy object for DMs.
        if (!channel)
        {
            channel = Channel(new CChannel());
            channel->ID = json.GetValue<std::string>("channel_id");
            channel->Type = ChannelTypes::DM;
        }

        Ret->ID = json.GetValue<std::string>("id");
        Ret->ChannelRef = channel;

        std::string UserJson = json.GetValue<std::string>("author");
        if (!UserJson.empty())
        {
            CJSON juser;
            juser.ParseObject(UserJson);

            User user;
            Users::iterator UIT = m_Users.find(juser.GetValue<std::string>("id"));
            if (UIT != m_Users.end())
                user = UIT->second;
            else
                user = CreateUser(juser);

            Ret->Author = user;

            //Gets the guild member, if this message is not a dm.
            if (Ret->GuildRef)
            {
                auto MIT = Ret->GuildRef->Members.find(Ret->Author->ID);
                if (MIT != Ret->GuildRef->Members.end())
                    Ret->Member = MIT->second;
                else
                    Ret->Member = GetMember(Ret->GuildRef, Ret->Author->ID);
            }
        }

        Ret->Content = json.GetValue<std::string>("content");
        Ret->Timestamp = json.GetValue<std::string>("timestamp");
        Ret->EditedTimestamp = json.GetValue<std::string>("edited_timestamp");
        Ret->Mention = json.GetValue<bool>("mention_everyone");

        std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("mentions");
        for (auto &&e : Array)
        {
            CJSON jmention;
            jmention.ParseObject(e);

            User user;
            Users::iterator UIT = m_Users.find(jmention.GetValue<std::string>("id"));
            if (UIT != m_Users.end())
                user = UIT->second;
            else
                user = CreateUser(jmention);

            bool Found = false;

            if (Ret->GuildRef)
            {
                auto MIT = Ret->GuildRef->Members.find(Ret->Author->ID);
                if (MIT != Ret->GuildRef->Members.end())
                {
                    Found = true;
                    Ret->Mentions.push_back(MIT->second);
                }
            }

            //Create a fake Guildmember for DMs.
            if (!Found)
            {
                Ret->Mentions.push_back(GuildMember(new CGuildMember()));
                Ret->Mentions.back()->UserRef = user;
            }
        }

        return Ret;
    }

    Role CDiscordClient::CreateRole(CJSON &json)
    {
        Role ret = Role(new CRole());

        ret->ID = json.GetValue<std::string>("id");
        ret->Name = json.GetValue<std::string>("name");
        ret->Color = json.GetValue<uint32_t>("color");
        ret->Hoist = json.GetValue<bool>("hoist");
        ret->Position = json.GetValue<int>("position");
        ret->Permissions = (Permission)json.GetValue<uint32_t>("permissions");
        ret->Managed = json.GetValue<bool>("managed");
        ret->Mentionable = json.GetValue<bool>("mentionable");

        return ret;
    }
} // namespace DiscordBot
