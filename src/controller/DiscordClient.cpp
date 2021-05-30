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
#include <models/DiscordException.hpp>
#include <models/Role.hpp>
#include "../helpers/Helper.hpp"
#include "../helpers/MultipartFormData.hpp"

#define CLOG_IMPLEMENTATION
#include <Log.hpp>

#ifdef DISCORDBOT_UNIX
#include <signal.h>
#endif

namespace DiscordBot
{
    DiscordClient IDiscordClient::Create(const std::string &Token, Intent Intents)
    {
        //Needed for windows.
        ix::initNetSystem();

        //Initialize libsodium.
        if (sodium_init() < 0) 
            llog << lerror << "Error to init libsodium" << lendl;

        return DiscordClient(new CDiscordClient(Token, Intents));
    }

    CDiscordClient::CDiscordClient(const std::string &Token, Intent Intents) : m_Intents(Intents), m_Token(Token), m_Terminate(false), m_HeartACKReceived(false), m_Quit(false), m_LastSeqNum(-1), m_IsAFK(false), m_State(OnlineState::ONLINE)
    {
#ifdef DISCORDBOT_UNIX
        //Ignores the SIGPIPE signal.
        signal(SIGPIPE, SIG_IGN);
#endif
        USER_AGENT = std::string("libDiscordBot (https://github.com/tostc/libDiscordBot, ") + VERSION + ")";

        m_EVManger.SubscribeMessage(QUEUE_NEXT_SONG, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));  
        m_EVManger.SubscribeMessage(RESUME, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));  
        m_EVManger.SubscribeMessage(RECONNECT, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));   
        m_EVManger.SubscribeMessage(QUIT, std::bind(&CDiscordClient::OnMessageReceive, this, std::placeholders::_1));   

        //Disable client side checking.
        ix::SocketTLSOptions DisabledTrust;
        DisabledTrust.caFile = "NONE";

        m_HTTPClient.setTLSOptions(DisabledTrust);
        m_Socket.setTLSOptions(DisabledTrust);
    }

    void CDiscordClient::SetState(OnlineState state)
    {
        m_State = state;
        UpdateUserInfo();
    }

    void CDiscordClient::SetAFK(bool AFK)
    {
        m_IsAFK = AFK;
        UpdateUserInfo();
    }

    void CDiscordClient::SetActivity(const std::string &Text, const std::string &URL)
    {
        m_Text = Text;
        m_URL = URL;
        UpdateUserInfo();
    }

    void CDiscordClient::UpdateUserInfo()
    {        
        // TODO: USE FACTORY
        SendOP(OPCodes::PRESENCE_UPDATE, CreateUserInfoJSON());
    }

#pragma region TO_CHANNEL

    // TODO: Create Factory
    std::string CDiscordClient::CreateUserInfoJSON()
    {
        CJSON json;

        std::string State = OnlineStateToStr(m_State);

        json.AddPair("since", static_cast<uint32_t>(time(nullptr)));
        json.AddPair("status", State);
        json.AddPair("afk", m_IsAFK);

        CJSON activity;
        activity.AddPair("name", m_Text);

        if(!m_URL.empty())
        {
            activity.AddPair("url", m_URL);
            activity.AddPair("type", 1); //Streaming
        }
        else
            activity.AddPair("type", 0); //Game

        json.AddJSON("game", activity.Serialize());

        return json.Serialize();
    }

    // TODO: Move to CChannel
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

    // TODO: Move to CChannel
    void CDiscordClient::Join(Channel channel)
    {
        if (!channel || channel->GuildID->empty() || channel->ID->empty())
            return;

        ChangeVoiceState(channel->GuildID, channel->ID);
    }

    // TODO: Move to CChannel
    void CDiscordClient::Leave(Guild guild)
    {
        if (!guild)
            return;

        ChangeVoiceState(guild->ID);
    }
#pragma endregion

#pragma region WHERE_TO_MOVE

    // TODO: ???
    AudioSource CDiscordClient::GetAudioSource(Guild guild)
    {
        if(!guild)
            return AudioSource();

        VoiceSockets::iterator IT = m_VoiceSockets->find(guild->ID);
        if (IT != m_VoiceSockets->end())
            return IT->second->GetAudioSource();

        return AudioSource();
    }   

    MusicQueue CDiscordClient::GetMusicQueue(Guild guild)
    {
        if(!guild)
            return MusicQueue();

        auto IT = m_MusicQueues->find(guild->ID);
        if (IT != m_MusicQueues->end())
            return IT->second;

        return MusicQueue();
    }

    bool CDiscordClient::IsPlaying(Guild guild)
    {
        return GetAudioSource(guild) != nullptr;
    }

    void CDiscordClient::AddToQueue(Guild guild, SongInfo Info)
    {
        if(!guild)
            return;

        auto IT = m_MusicQueues->find(guild->ID);
        if(IT != m_MusicQueues->end())
            IT->second->AddSong(Info);
        else if(m_QueueFactory)
        {
            auto Tmp = m_QueueFactory->Create();
            Tmp->SetGuildID(guild->ID);
            Tmp->SetOnWaitFinishCallback(std::bind(&CDiscordClient::OnQueueWaitFinish, this, std::placeholders::_1, std::placeholders::_2));
            Tmp->AddSong(Info);
            m_MusicQueues->insert({guild->ID, Tmp});
        }
    }

    bool CDiscordClient::StartSpeaking(Channel channel)
    {
        if (!channel || channel->GuildID->empty())
            return false;

        AudioSource Source;

        auto IT = m_MusicQueues->find(channel->GuildID);
        if(IT != m_MusicQueues->end())
        {
            if(IT->second->HasNext())
                Source = IT->second->Next();
            else
                IT->second->ClearQueue();
        }

        return StartSpeaking(channel, Source);
    }

    bool CDiscordClient::StartSpeaking(Channel channel, AudioSource source)
    {
        if (!channel || channel->GuildID->empty())
            return false;

        VoiceSockets::iterator IT = m_VoiceSockets->find(channel->GuildID);
        if (IT != m_VoiceSockets->end() && source)
            IT->second->StartSpeaking(source);
        else if(source)
        {
            Join(channel);

            m_AudioSources->insert({channel->GuildID, source});
        }

        return true;
    }

    void CDiscordClient::PauseSpeaking(Guild guild)
    {
        if(!guild)
            return;

        VoiceSockets::iterator IT = m_VoiceSockets->find(guild->ID);
        if (IT != m_VoiceSockets->end())
            IT->second->PauseSpeaking();
    }

    void CDiscordClient::ResumeSpeaking(Guild guild) 
    {
        if(!guild)
            return;

        VoiceSockets::iterator IT = m_VoiceSockets->find(guild->ID);
        if (IT != m_VoiceSockets->end())
            IT->second->ResumeSpeaking();
    }

    void CDiscordClient::StopSpeaking(Guild guild)
    {
        if(!guild)
            return;

        VoiceSockets::iterator IT = m_VoiceSockets->find(guild->ID);
        if (IT != m_VoiceSockets->end())
            IT->second->StopSpeaking();
    }

    void CDiscordClient::RemoveSong(Channel channel, size_t Index)
    {
        if (!channel || channel->GuildID->empty())
            return;

        auto IT = m_MusicQueues->find(channel->GuildID);
        if(IT != m_MusicQueues->end())
            IT->second->RemoveSong(Index);
    }

    void CDiscordClient::RemoveSong(Channel channel, const std::string &Name)
    {
        if (!channel || channel->GuildID->empty())
            return;

        auto IT = m_MusicQueues->find(channel->GuildID);
        if(IT != m_MusicQueues->end())
            IT->second->RemoveSong(Name);
    }

    void CDiscordClient::OnSpeakFinish(const std::string &Guild)
    {
        if(m_Controller)
        {
            m_EVManger.PostMessage(QUEUE_NEXT_SONG, Guild);

            auto IT = m_Guilds->find(Guild);
            if(IT != m_Guilds->end())
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

        VoiceSockets::iterator IT = m_VoiceSockets->find(Guild);
        if(IT != m_VoiceSockets->end())
            IT->second->StartSpeaking(Source);
    }

#pragma endregion WHERE_TO_MOVE


    void CDiscordClient::Run()
    {
        //Requests the gateway endpoint for bots.
        auto res = Get("/gateway/bot");
        if (res->statusCode == 200)
        {
            try
            {
                CJSON json;
                m_Gateway = json.Deserialize<std::shared_ptr<SGateway>>(res->body);
            }
            catch (const CJSONException &e)
            {
                llog << lerror << "Failed to parse JSON Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
                return;
            }

            //Connects to discords websocket.
            m_Socket.setUrl(m_Gateway->URL + "/?v=8&encoding=json");
            m_Socket.setOnMessageCallback(std::bind(&CDiscordClient::OnWebsocketEvent, this, std::placeholders::_1));
            m_Socket.start();

            //Runs until the bot quits.
            while (!m_Quit)
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        else
            llog << lerror << "HTTP " << res->statusCode << " Error " << res->errorMsg << lendl;
    }

    void CDiscordClient::Quit()
    {
        auto IT = m_Guilds->begin();
        while (IT != m_Guilds->end())
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

        m_Guilds->clear();
        m_VoiceSockets->clear();
        m_AudioSources->clear();
        m_Users->clear();
        m_MusicQueues->clear();
        m_Quit = true;
    }

    void CDiscordClient::QuitAsync()
    {
        m_EVManger.PostMessage(QUIT, 0, 200);
    }

    void CDiscordClient::OnMessageReceive(MessageBase Msg)
    {
        switch (Msg->Event)
        {
            // TODO: ??? WHERE_TO_MOVE
            case QUEUE_NEXT_SONG:
            {
                auto Data = std::static_pointer_cast<TMessage<std::string>>(Msg);

                AudioSource Source;

                auto MQIT = m_MusicQueues->find(Data->Value);
                if(MQIT != m_MusicQueues->end())
                {
                    if(MQIT->second->HasNext())
                        Source = MQIT->second->Next();
                    else
                        MQIT->second->ClearQueue();
                }

                if(Source)
                {
                    auto IT = m_VoiceSockets->find(Data->Value);
                    if(IT != m_VoiceSockets->end())
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

            case QUIT:
            {
                Quit();
            }break;
        }
    }

    // TODO: Split up in smaller methods.
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

                                m_BotUser = CObjectFactory::Deserialize<CUser>(this, json.GetValue<std::string>("user"));
                                m_Users->insert({m_BotUser->ID.load(), m_BotUser});

                                auto Unavailables = json.GetValue<std::vector<std::string>>("guilds");
                                for (auto &&e : Unavailables)
                                {
                                    CJSON tmp;
                                    tmp.ParseObject(e);

                                    m_Unavailables.push_back(tmp.GetValue<std::string>("id"));
                                }

                                // m_BotUser = CreateUser(json);

                                llog << linfo << "Connected with Discord! " << m_Socket.getUrl() << lendl;

                                if (m_Controller)
                                    m_Controller->OnReady();
                            }
                            break;

                            /*------------------------GUILDS Intent------------------------*/

                            case Adler32("GUILD_CREATE"):
                            {
                                json.ParseObject(Pay.D);

                                Guild guild = Guild(new CGuild(this));
                                guild->ID = json.GetValue<std::string>("id");
                                guild->Name = json.GetValue<std::string>("name");
                                guild->Icon = json.GetValue<std::string>("icon");

                                //Get all Roles;
                                std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("roles");
                                for (auto &&e : Array)
                                {
                                    Role Tmp = CObjectFactory::Deserialize<CRole>(this, e);
                                    guild->Roles->insert({Tmp->ID, Tmp});
                                }

                                //Get all Channels;
                                Array = json.GetValue<std::vector<std::string>>("channels");
                                for (auto &&e : Array)
                                {
                                    Channel Tmp = CObjectFactory::Deserialize<CChannel>(this, e);
                                    
                                    Tmp->GuildID = guild->ID;
                                    guild->Channels->insert({Tmp->ID, Tmp});
                                }

                                //Get all members.
                                Array = json.GetValue<std::vector<std::string>>("members");
                                for (auto &&e : Array)
                                    GuildMember Tmp = CObjectFactory::Deserialize<CGuildMember>(this, e);

                                //Get all voice states.
                                Array = json.GetValue<std::vector<std::string>>("voice_states");
                                for (auto &&e : Array)
                                    CObjectFactory::Deserialize<CVoiceState>(this, e);

                                //Gets the owner object.
                                std::string OwnerID = json.GetValue<std::string>("owner_id");
                                guild->Owner = GetMember(guild, OwnerID);
                                m_Guilds->insert({guild->ID, guild});

                                auto IT = std::find(m_Unavailables.begin(), m_Unavailables.end(), guild->ID);
                                if(IT != m_Unavailables.end())
                                {
                                    m_Unavailables.erase(IT);

                                    if(m_Controller)
                                        m_Controller->OnGuildAvailable(guild);
                                }
                                else if(m_Controller)
                                    m_Controller->OnGuildJoin(guild);
                            }break;

                            case Adler32("GUILD_DELETE"):
                            {
                                json.ParseObject(Pay.D);

                                auto IT = m_Guilds->find(json.GetValue<std::string>("id"));
                                if(IT != m_Guilds->end())
                                {
                                    bool Unavailable = json.GetValue<bool>("unavailable");
                                    auto InnerIT = std::find(m_Unavailables.begin(), m_Unavailables.end(), IT->second->ID);

                                    if(Unavailable && m_Controller && InnerIT != m_Unavailables.end())
                                    {
                                        m_Unavailables.erase(InnerIT);
                                        m_Controller->OnGuildUnavailable(IT->second);
                                    }
                                    else if(!Unavailable && m_Controller)
                                        m_Controller->OnGuildLeave(IT->second);
                                    else
                                        m_Unavailables.push_back(IT->second->ID);

                                    m_VoiceSockets->erase(IT->second->ID);
                                    m_MusicQueues->erase(IT->second->ID);
                                    m_Guilds->erase(IT);
                                }

                                llog << linfo << "GUILD_DELETE" << lendl;
                            }break;

                            /*------------------------GUILDS Intent------------------------*/

                            /*------------------------CHANNEL Intent------------------------*/

                            case Adler32("CHANNEL_CREATE"):
                            {
                                Channel Tmp = CObjectFactory::Deserialize<CChannel>(this, Pay.D);
                                
                                auto IT = m_Guilds->find(Tmp->GuildID);
                                if(IT != m_Guilds->end())
                                    IT->second->Channels->insert({Tmp->ID, Tmp});
                            }break;

                            case Adler32("CHANNEL_UPDATE"):
                            {
                                Channel Tmp = CObjectFactory::Deserialize<CChannel>(this, Pay.D);

                                auto IT = m_Guilds->find(Tmp->GuildID);
                                if(IT != m_Guilds->end())
                                {
                                    IT->second->Channels->erase(Tmp->ID);
                                    IT->second->Channels->insert({Tmp->ID, Tmp});
                                }
                            }break;

                            case Adler32("CHANNEL_DELETE"):
                            {
                                Channel Tmp = CObjectFactory::Deserialize<CChannel>(this, Pay.D);

                                auto IT = m_Guilds->find(Tmp->GuildID);
                                if(IT != m_Guilds->end())
                                    IT->second->Channels->erase(Tmp->ID);
                            }break;

                            /*------------------------CHANNEL Intent------------------------*/

                            /*------------------------GUILD_MEMBERS Intent------------------------*/
                            //ATTENTION: NEEDS "Server Members Intent" ACTIVATED TO WORK, OTHERWISE THE BOT FAIL TO CONNECT AND A ERROR IS WRITTEN TO THE CONSOLE!!!

                            case Adler32("GUILD_MEMBER_ADD"):
                            {
                                GuildMember Tmp = CObjectFactory::Deserialize<CGuildMember>(this, Pay.D);

                                if(m_Controller && Tmp)
                                {
                                    auto IT = m_Guilds->find(Tmp->GuildID);
                                    if(IT != m_Guilds->end())
                                    {
                                        Guild guild = IT->second;
                                        m_Controller->OnMemberAdd(guild, Tmp);
                                    }
                                    else
                                        llog << ldebug << "Invalid Guild ( " << Tmp->GuildID << " ) " << lendl;
                                }
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

                                auto GIT = m_Guilds->find(GuildID);
                                if(GIT != m_Guilds->end())
                                {
                                    Guild guild = GIT->second;//m_Guilds[GuildID];
                                    auto IT = guild->Members->find(UserID);
                                    if(IT != guild->Members->end())
                                    {
                                        IT->second->Roles->clear();
                                        for (auto &&e : Array)
                                            IT->second->Roles->push_back(guild->Roles->at(e));                               

                                        IT->second->Nick = Nick;
                                        IT->second->PremiumSince = Premium;

                                        if(m_Controller)
                                            m_Controller->OnMemberUpdate(guild, IT->second);
                                    } 
                                }
                                else
                                    llog << ldebug << "Invalid Guild ( " << GuildID << " ) " << lendl;
                            }break;

                            case Adler32("GUILD_BAN_ADD"):
                            case Adler32("GUILD_MEMBER_REMOVE"):
                            {
                                json.ParseObject(Pay.D);
                                std::string GuildID = json.GetValue<std::string>("guild_id");

                                json.ParseObject(json.GetValue<std::string>("user"));
                                std::string UserID = json.GetValue<std::string>("id");

                                auto GIT = m_Guilds->find(GuildID);
                                if(GIT != m_Guilds->end())
                                {
                                    Guild guild = GIT->second;//m_Guilds[GuildID];

                                    auto IT = guild->Members->find(UserID);
                                    if(IT != guild->Members->end())
                                    {
                                        GuildMember member = IT->second;
                                        guild->Members->erase(IT);

                                        if(m_Controller)
                                            m_Controller->OnMemberRemove(guild, member);
                                    }                                

                                    if(m_Users->find(UserID) != m_Users->end())
                                    {
                                        if(m_Users->at(UserID).use_count() == 1)
                                            m_Users->erase(UserID);
                                    }
                                }
                                else
                                    llog << ldebug << "Invalid Guild ( " << GuildID << " ) " << lendl;
                            }break;

                            /*------------------------GUILD_MEMBERS Intent------------------------*/

                            /*------------------------GUILD_PRESENCES Intent------------------------*/
                            //ATTENTION: NEEDS "Presence Intent" ACTIVATED TO WORK, OTHERWISE THE BOT FAIL TO CONNECT AND A ERROR IS WRITTEN TO THE CONSOLE!!!

                            case Adler32("PRESENCE_UPDATE"):
                            { 
                                json.ParseObject(Pay.D);
                                User user = GetUserOrAdd(json.GetValue<std::string>("user"));

                                if(!json.GetValue<std::string>("game").empty())
                                    user->Game = CObjectFactory::Deserialize<CActivity>(this, json.GetValue<std::string>("game"));

                                user->State = StrToOnlineState(json.GetValue<std::string>("status"));
                                std::vector<std::string> Acts = json.GetValue<std::vector<std::string>>("activities");
                                for (auto &&e : Acts)
                                    user->Activities->push_back(CObjectFactory::Deserialize<CActivity>(this, e));

                                CJSON JClientState;
                                JClientState.ParseObject(json.GetValue<std::string>("client_status")); 

                                user->Desktop = StrToOnlineState(JClientState.GetValue<std::string>("desktop"));      
                                user->Mobile = StrToOnlineState(JClientState.GetValue<std::string>("mobile"));   
                                user->Web = StrToOnlineState(JClientState.GetValue<std::string>("web"));                      

                                auto GIT = m_Guilds->find(json.GetValue<std::string>("guild_id"));
                                if(GIT != m_Guilds->end())
                                {
                                    GuildMember member;
                                    auto MIT = GIT->second->Members->find(user->ID);
                                    if(MIT == GIT->second->Members->end())
                                        member = GetMember(GIT->second, user->ID);
                                    else
                                        member = MIT->second;

                                    if(m_Controller)
                                        m_Controller->OnPresenceUpdate(GIT->second, member);
                                }
                            }break;

                            /*------------------------GUILD_PRESENCES Intent------------------------*/

                            /*------------------------GUILD_VOICE_STATES Intent------------------------*/

                            case Adler32("VOICE_STATE_UPDATE"):
                            {
                                json.ParseObject(Pay.D);

                                auto G = m_Guilds->find(json.GetValue<std::string>("guild_id"));
                                auto M = G->second->Members->find(json.GetValue<std::string>("user_id"));
                                Channel c;
                                if(M->second->State)
                                    c = M->second->State->ChannelRef;   //Saves the old channel.

                                VoiceState Tmp = CObjectFactory::Deserialize<CVoiceState>(this, Pay.D);

                                if (m_Controller && Tmp->GuildRef)
                                {
                                    if(Tmp->UserRef)
                                    {
                                        if(Tmp->UserRef->ID == m_BotUser->ID && !Tmp->ChannelRef)
                                        {
                                            m_VoiceSockets->erase(Tmp->GuildRef->ID);
                                            m_MusicQueues->erase(Tmp->GuildRef->ID);
                                        }

                                        auto IT = Tmp->GuildRef->Members->find(Tmp->UserRef->ID);
                                        if(IT != Tmp->GuildRef->Members->end())
                                        {
                                            m_Controller->OnVoiceStateUpdate(Tmp->GuildRef, IT->second);

                                            auto AIT = m_Admins->find(Tmp->GuildRef->ID);
                                            if(AIT != m_Admins->end())
                                            {
                                                auto Admin = std::dynamic_pointer_cast<CGuildAdmin>(AIT->second);

                                                if(!c)
                                                    c = Tmp->ChannelRef;
                                                    
                                                if(c)
                                                    Admin->OnUserVoiceStateChanged(c, IT->second);
                                            }
                                        }
                                    }
                                }   
                            }break;

                            /*------------------------GUILD_VOICE_STATES Intent------------------------*/

                            //Called if your bot joins a voice channel.
                            case Adler32("VOICE_SERVER_UPDATE"):
                            {
                                json.ParseObject(Pay.D);
                                Guilds::iterator GIT = m_Guilds->find(json.GetValue<std::string>("guild_id"));
                                if (GIT != m_Guilds->end())
                                {
                                    auto UIT = GIT->second->Members->find(m_BotUser->ID);
                                    if (UIT != GIT->second->Members->end())
                                    {
                                        VoiceSocket Socket = VoiceSocket(new CVoiceSocket(json, UIT->second->State->SessionID, m_BotUser->ID));
                                        Socket->SetOnSpeakFinish(std::bind(&CDiscordClient::OnSpeakFinish, this, std::placeholders::_1));
                                        m_VoiceSockets->insert({GIT->second->ID, Socket});

                                        //Creates a music queue for the server.
                                        if(m_QueueFactory)
                                        {
                                            if(m_MusicQueues->find(GIT->second->ID) == m_MusicQueues->end())
                                            {
                                                MusicQueue MQ = m_QueueFactory->Create();
                                                MQ->SetGuildID(GIT->second->ID);
                                                MQ->SetOnWaitFinishCallback(std::bind(&CDiscordClient::OnQueueWaitFinish, this, std::placeholders::_1, std::placeholders::_2));
                                                m_MusicQueues->insert({GIT->second->ID, MQ});
                                            }
                                        }

                                        //Plays the queued audiosource.
                                        AudioSources::iterator IT = m_AudioSources->find(GIT->second->ID);
                                        if (IT != m_AudioSources->end())
                                        {
                                            Socket->StartSpeaking(IT->second);
                                            m_AudioSources->erase(IT);
                                        }
                                    }
                                }
                            }break;

                            /*------------------------GUILD_MESSAGES Intent------------------------*/

                            case Adler32("MESSAGE_CREATE"):
                            case Adler32("MESSAGE_UPDATE"):
                            case Adler32("MESSAGE_DELETE"):
                            {
                                Message msg = CObjectFactory::Deserialize<CMessage>(this, Pay.D);
                                
                                std::shared_ptr<CGuildAdmin> Admin;
                                auto AIT = m_Admins->find(msg->GuildRef->ID);
                                if(AIT != m_Admins->end())
                                    Admin = std::dynamic_pointer_cast<CGuildAdmin>(AIT->second);

                                switch (Adler32(Pay.T.c_str()))
                                {
                                    case Adler32("MESSAGE_CREATE"):
                                    {
                                        if (m_Controller)
                                            m_Controller->OnMessage(msg);

                                        if(Admin)
                                            Admin->OnMessageEvent(ActionType::MESSAGE_CREATED, msg->ChannelRef, msg);
                                    }break;

                                    case Adler32("MESSAGE_UPDATE"):
                                    {
                                        if (m_Controller)
                                            m_Controller->OnMessageEdited(msg);

                                        if(Admin)
                                            Admin->OnMessageEvent(ActionType::MESSAGE_EDITED, msg->ChannelRef, msg);
                                    }break;

                                    case Adler32("MESSAGE_DELETE"):
                                    {
                                        if (m_Controller)
                                            m_Controller->OnMessageDeleted(msg);

                                        if(Admin)
                                            Admin->OnMessageEvent(ActionType::MESSAGE_DELETED, msg->ChannelRef, msg);
                                    }break;
                                }

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

    void CDiscordClient::Heartbeat()
    {
        while (!m_Terminate)
        {
            //Start a reconnect.
            if (!m_HeartACKReceived)
            {
                m_Socket.stop();
                m_VoiceSockets->clear();

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
        }
    }

    void CDiscordClient::SendOP(CDiscordClient::OPCodes OP, const std::string &D)
    {
        SPayload Pay;
        Pay.OP = (uint32_t)OP;
        Pay.D = D;

        try
        {
            CJSON json;
            std::string TT = json.Serialize(Pay);
            m_Socket.send(json.Serialize(Pay));
        }
        catch (const CJSONException &e)
        {
            llog << lerror << "Failed to serialize the Payload object. Enumtype: " << GetEnumName(e.GetErrType()) << " what(): " << e.what() << lendl;
        }
    }

    void CDiscordClient::SendIdentity()
    {
        SIdentify id;
        id.Token = m_Token;
        id.Properties["$os"] = "linux";
        id.Properties["$browser"] = "libDiscordBot";
        id.Properties["$device"] = "libDiscordBot";
        id.Properties["presence"] = CreateUserInfoJSON();
        id.Intents = m_Intents;

        CJSON json;
        SendOP(OPCodes::IDENTIFY, json.Serialize(id));
    }

    void CDiscordClient::SendResume()
    {
        SResume resume;
        resume.Token = m_Token;
        resume.SessionID = m_SessionID;
        resume.Seq = m_LastSeqNum;

        CJSON json;
        SendOP(OPCodes::RESUME, json.Serialize(resume));
    }

    bool CDiscordClient::CheckPermissions(Guild guild, Permission Needed, std::vector<PermissionOverwrites> Overwrites)
    {
        GuildMember Member = GetMember(guild, m_BotUser->ID.load());

        bool HasPermission = false;
        for (auto it = Member->Roles->begin(); it < Member->Roles->end(); it++)
        {
            Permission Perm = it->get()->Permissions;
            for (auto &&p : Overwrites)
            {
                Perm &= ~p->Deny;
                Perm |= p->Allow;
            }            

            if((Perm & Needed) == Needed)
            {
                HasPermission = true;
                break;
            }
        }

        return HasPermission;
    }

    ix::HttpResponsePtr CDiscordClient::Get(const std::string &URL)
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Adds the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["User-Agent"] = USER_AGENT;

        return m_HTTPClient.get(std::string(BASE_URL) + URL, args);
    }

    ix::HttpResponsePtr CDiscordClient::Post(const std::string &URL, const std::string &Body, const std::string &ContentType)
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Adds the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["Content-Type"] = ContentType;
        args->extraHeaders["User-Agent"] = USER_AGENT;

        return m_HTTPClient.post(std::string(BASE_URL) + URL, Body, args);
    }

    ix::HttpResponsePtr CDiscordClient::Put(const std::string &URL, const std::string &Body)
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Adds the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["Content-Type"] = "application/json";
        args->extraHeaders["User-Agent"] = USER_AGENT;

        return m_HTTPClient.put(std::string(BASE_URL) + URL, Body, args);
    }

    ix::HttpResponsePtr CDiscordClient::Patch(const std::string &URL, const std::string &Body, const std::string &ContentType)
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Adds the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["Content-Type"] = ContentType;
        args->extraHeaders["User-Agent"] = USER_AGENT;

        return m_HTTPClient.patch(std::string(BASE_URL) + URL, Body, args);
    }

    ix::HttpResponsePtr CDiscordClient::Delete(const std::string &URL, const std::string &Body)
    {
        ix::HttpRequestArgsPtr args = ix::HttpRequestArgsPtr(new ix::HttpRequestArgs());

        //Adds the bot token.
        args->extraHeaders["Authorization"] = "Bot " + m_Token;
        args->extraHeaders["User-Agent"] = USER_AGENT;

        if(Body != "")
        {
            args->extraHeaders["Content-Type"] = "application/json";
            return m_HTTPClient.request(std::string(BASE_URL) + URL, "DELETE", Body, args);
        }
        else
            return m_HTTPClient.request(std::string(BASE_URL) + URL, "DELETE", "", args);
    }

    std::string CDiscordClient::OnlineStateToStr(OnlineState state)
    {
        switch(state)
        {
            case OnlineState::ONLINE:
            {
                return "online";
            }break;

            case OnlineState::DND:
            {
                return "dnd";
            }break;

            case OnlineState::IDLE:
            {
                return "idle";
            }break;

            case OnlineState::INVISIBLE:
            {
                return "invisible";
            }break;
            
            case OnlineState::OFFLINE:
            {
                return "offline";
            }break;

            default:
            {
                return "offline";
            }break;
        }
    }

    OnlineState CDiscordClient::StrToOnlineState(const std::string &state)
    {
        switch(Adler32(state.c_str()))
        {
            case Adler32("online"):
            {
                return OnlineState::ONLINE;
            }break;

            case Adler32("dnd"):
            {
                return OnlineState::DND;
            }break;

            case Adler32("idle"):
            {
                return OnlineState::IDLE;
            }break;

            case Adler32("invisible"):
            {
                return OnlineState::INVISIBLE;
            }break;

            case Adler32("offline"):
            {
                return OnlineState::OFFLINE;
            }break;

            default:
            {
                return OnlineState::OFFLINE;
            }break;
        }
    }

    GuildMember CDiscordClient::GetMember(Guild guild, const std::string &UserID)
    {
        auto UserIT = guild->Members->find(UserID);
        GuildMember Ret;

        if(UserIT != guild->Members->end())
            Ret = UserIT->second;
        else
        {
            auto res = Get("/guilds/" + guild->ID + "/members/" + UserID);
            if (res->statusCode != 200)
                llog << lerror << "Failed to receive owner info HTTP: " << res->statusCode << " MSG: " << res->errorMsg << lendl;
            else
            {
                try
                {    
                    Ret = CObjectFactory::Deserialize<CGuildMember>(this, res->body);
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
} // namespace DiscordBot
