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

#ifndef DISCORDCLIENT_HPP
#define DISCORDCLIENT_HPP

#include <string>
#include <JSON.hpp>
#include <stdint.h>
#include <DiscordBot/IDiscordClient.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXHttpClient.h>
#include <thread>
#include <map>
#include <DiscordBot/Models/User.hpp>
#include <DiscordBot/Models/Guild.hpp>
#include <DiscordBot/Models/Role.hpp>
#include <DiscordBot/Models/Activity.hpp>
#include <atomic>
#include <DiscordBot/Messages/MessageManager.hpp>
#include "../Models/Payload.hpp"
#include "VoiceSocket.hpp"
#include <DiscordBot/Utils/Atomic.hpp>
#include <DiscordBot/Channels/VoiceChannel.hpp>
#include "GuildAdmin.hpp"
#include "../helpers/Factory/ObjectFactory.hpp"
#include <DiscordBot/Voice/VoiceClient.hpp>
#include <DiscordBot/Utils/Snowflake.hpp>

#undef SendMessage

namespace DiscordBot
{
    using namespace Internal;

    class CDiscordClient : public IDiscordClient
    {
        public:
            //All informations from https://discordapp.com/developers/docs/topics/opcodes-and-status-codes
            enum class OPCodes
            {
                //Name                  Code        Client Action       Description
                DISPATCH                = 0,        //Receive           An event was dispatched.
                HEARTBEAT               = 1,        //Send/Receive      Fired periodically by the client to keep the connection alive.
                IDENTIFY                = 2,        //Send              Starts a new session during the initial handshake.
                PRESENCE_UPDATE         = 3,        //Send	            Update the client's presence.
                VOICE_STATE_UPDATE      = 4,        //Send              Used to join/leave or move between voice channels.
                RESUME                  = 6,        //Send	            Resume a previous session that was disconnected.
                RECONNECT               = 7,        //Receive	        You should attempt to reconnect and resume immediately.
                REQUEST_GUILD_MEMBERS   = 8,        //Send	            Request information about offline guild members in a large guild.
                INVALID_SESSION         = 9,        //Receive	        The session has been invalidated. You should reconnect and identify/resume accordingly.
                HELLO                   = 10,       //Receive	        Sent immediately after connecting, contains the heartbeat_interval to use.
                HEARTBEAT_ACK           = 11        //Receive           Sent in response to receiving a heartbeat to acknowledge that it has been received.
            };

            /**
             * @brief Identifies the bot.
             */
            struct SIdentify
            {
                std::string Token;
                std::map<std::string, std::string> Properties;
                Intent Intents;

                void Serialize(CJSON &json) const
                {
                    json.AddPair("token", Token);
                    json.AddPair("properties", Properties);
                    json.AddPair("intents", (uint32_t)Intents);
                }
            };


            /**
             * @brief Resumes the bot.
             */
            struct SResume
            {
                std::string Token;
                std::string SessionID;
                uint32_t Seq;

                void Serialize(CJSON &json) const
                {
                    json.AddPair("token", Token);
                    json.AddPair("session_id", SessionID);
                    json.AddPair("seq", Seq);
                }
            };

            /**
             * @brief Sessions limits object which is returned after the bot is connected to the discord servers.
             */
            struct SSessionStartLimit
            {
                uint32_t Total;
                uint32_t Remaining;
                uint32_t ResetAfter;

                void Deserialize(CJSON &json)
                {
                    Total = json.GetValue<uint32_t>("total");
                    Remaining = json.GetValue<uint32_t>("remaining");
                    ResetAfter = json.GetValue<uint32_t>("reset_after");
                }
            };

            /**
             * @brief Gateway object which is returned after the bot is connected to the discord servers.
             */
            struct SGateway
            {
                std::string URL;
                uint32_t Shards;
                SSessionStartLimit Limit;

                void Deserialize(CJSON &json)
                {
                    URL = json.GetValue<std::string>("url");
                    Shards = json.GetValue<uint32_t>("shards");
                    Limit = json.GetValue<SSessionStartLimit>("session_start_limit");
                }
            };

            CDiscordClient(const std::string &Token, Intent Intents);

            /**
             * @brief Sets the online status of the bot.
             * 
             * @param state: Online state of the bot. @see State.
             */
            void SetState(OnlineState state) override;

            /**
             * @brief Sets the bot AFK.
             * 
             * @param AFK: True if the bot is afk.
             */
            void SetAFK(bool AFK) override;

            /**
             * @brief Sets the "Playing" status text or sets the bot in streaming mode.
             * 
             * @param Text: The text after "Playing"
             * @param URL: [Optional] A url to twitch or youtube, if this url is set the bot "Streams" on these platforms.
             */
            void SetActivity(const std::string &Text, const std::string &URL = "") override;

            /**
             * @brief Runs the bot. The call returns if you calls Quit(). @see Quit()
             */
            void Run() override;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            void Quit() override;

            /**
             * @brief Same as Quit() but as asynchronous call. Internally Quit() is called. @see Quit()
             */
            void QuitAsync() override;

            /**
             * @return Gets the bot user.
             */
            User GetBotUser() override
            {
                return m_BotUser;
            }

            /**
             * @return Gets the bot guild member of a given guild.
             */
            GuildMember GetBotMember(Guild guild) override
            {
                GuildMember ret;
                if(guild)
                {
                    auto IT = guild->Members->find(m_BotUser->ID);
                    if(IT != guild->Members->end())
                        ret = IT->second;
                }

                return ret;
            }

            /**
             * @return Gets the list of all connected servers.
             */
            Guilds GetGuilds() override
            {
                return m_Guilds;
            }

            /**
             * @return Gets a guild object by its id or null.
             */
            Guild GetGuild(const CSnowflake &GID) override
            {
                auto IT = m_Guilds->find(GID);
                if(IT != m_Guilds->end())
                    return IT->second;

                return nullptr;
            }

            /**
             * @return Gets a list of all users.
             */
            Users GetUsers() override
            {
                return m_Users;
            }

            ~CDiscordClient() {}

            // Internal APIs

            /**
             * @brief Joins or leaves a voice channel.
             */
            void ChangeVoiceState(CSnowflake GuildID, CSnowflake ChannelID);

            /**
             * @brief Checks the bots permissions.
             */
            bool CheckPermissions(Guild guild, Permission Needed, std::vector<PermissionOverwrites> Overwrites = std::vector<PermissionOverwrites>());

            ix::HttpResponsePtr Get(const std::string &URL);
            ix::HttpResponsePtr Post(const std::string &URL, const std::string &Body, const std::string &ContentType = "application/json");
            ix::HttpResponsePtr Put(const std::string &URL, const std::string &Body);
            ix::HttpResponsePtr Patch(const std::string &URL, const std::string &Body, const std::string &ContentType = "application/json");
            ix::HttpResponsePtr Delete(const std::string &URL, const std::string &Body = "");

            GuildMember GetMember(Guild guild, const CSnowflake &UserID);
            User GetUser(const std::string &ID)
            {
                auto IT = m_Users->find(ID);
                if(IT != m_Users->end())
                    return IT->second;

                return nullptr;
            }

            User GetUserOrAdd(const std::string &js)
            {
                CJSON json;
                json.ParseObject(js);

                auto Ret = GetUser(json.GetValue<std::string>("id"));
                if(Ret)
                    return Ret;

                Ret = CObjectFactory::Deserialize<CUser>(this, js);
                m_Users->insert({Ret->ID, Ret});

                return Ret;
            }

            CMessageManager *GetMessageManager()
            {
                return &m_EVManger;
            }
        private:
            using VoiceClients = std::map<CSnowflake, VoiceClient>;
            using VoiceConnectionResults = std::map<CSnowflake, MessageResult>;

            const char *BASE_URL = "https://discord.com/api";
            std::string USER_AGENT;

            CMessageManager m_EVManger;
            Intent m_Intents;

            std::string m_Token;
            std::shared_ptr<SGateway> m_Gateway;
            ix::WebSocket m_Socket;
            ix::HttpClient m_HTTPClient;

            std::thread m_Heartbeat;
            std::atomic<bool> m_Terminate;
            std::atomic<bool> m_HeartACKReceived;
            std::atomic<bool> m_Quit;
            uint32_t m_HeartbeatInterval;
            std::atomic<uint32_t> m_LastSeqNum;
            std::string m_SessionID;
            User m_BotUser;

            atomic<VoiceConnectionResults> m_VoiceConnectionResults;

            // Active connections
            atomic<VoiceClients> m_VoiceClients;

            // Unavailable guild IDs.
            std::vector<CSnowflake> m_Unavailables;

            //Map of all users in different servers.
            atomic<Users> m_Users;

            //All Guilds where the bot is in.
            atomic<Guilds> m_Guilds;

            bool m_IsAFK;
            OnlineState m_State;
            std::string m_Text; //Playing xy
            std::string m_URL;  //Streams on xy

            /**
             * @return Creates a user info object and return it as json string.
             */
            std::string CreateUserInfoJSON();

            /**
             * @brief Updates the userinfo things like online state, afk, now playing etc.
             */
            void UpdateUserInfo();

            /**
             * @brief Handles async. Messages.
             */
            void OnMessageReceive(MessageBase Msg);

            /**
             * @brief Receives all websocket events from discord. This is the heart of the bot.
             */
            void OnWebsocketEvent(const ix::WebSocketMessagePtr& msg);

            /**
             * @brief Sends a heartbeat.
             */
            void Heartbeat();

            /**
             * @brief Builds and sends a payload object.
             */
            void SendOP(OPCodes OP, const std::string &D);

            /**
             * @brief Sends the identity.
             */
            void SendIdentity();

            /**
             * @brief Sends a resume request.
             */
            void SendResume();

            std::string OnlineStateToStr(OnlineState state);
            OnlineState StrToOnlineState(const std::string &state);
    };
} // namespace DiscordBot


#endif //DISCORDCLIENT_HPP