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
#include <IDiscordClient.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>
#include <thread>
#include <map>
#include <models/User.hpp>
#include <models/Guild.hpp>
#include "Payload.hpp"
#include "VoiceSocket.hpp"

namespace DiscordBot
{
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
                INVALIDE_SESSION        = 9,        //Receive	        The session has been invalidated. You should reconnect and identify/resume accordingly.
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

            CDiscordClient(const std::string &Token) : m_Token(Token), m_Terminate(false), m_HeartACKReceived(false), m_Quit(false), m_LastSeqNum(-1) {}

            /**
             * @brief Connects to the given channel and uses the source to speak.
             * 
             * @param channel: The voice channel to connect to.
             * @param source: The audio source for speaking.
             * 
             * @return Returns true if the connection succeeded.
             */
            bool StartSpeaking(Channel channel, AudioSource source) override;

            /**
             * @brief Pauses the audio source. Call @see ResumeSpeaking to continue streaming.
             * 
             * @param channel: The voice channel to pause.
             */
            void PauseSpeaking(Channel channel) override {}

            /**
             * @brief Resumes the audio source.
             * 
             * @param channel: The voice channel to resume.
             */
            void ResumeSpeaking(Channel channel) override {}

            /**
             * @brief Stops the audio source.
             * 
             * @param channel: The voice channel to stop.
             */
            void StopSpeaking(Channel channel) override {}

            /**
             * @brief Joins a audio channel.
             * 
             * @param channel: The voice channel to join.
             */
            void Join(Channel channel) override;

            /**
             * @brief Leaves the audio channel.
             * 
             * @param channel: The voice channel to leave.
             */
            void Leave(Channel channel) override;

            /**
             * @brief Sends a message to a given channel.
             * 
             * @param channel: Text channel which will receive the message.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            void SendMessage(Channel channel, const std::string Text, bool TTS = false);

            /**
             * @return Returns the audio source for the given channel. Null if there is no audio source available.
             */
            AudioSource GetAudioSource(Channel channel) override {}

            /**
             * @brief Runs the bot. The call returns if you calls @see Quit().
             */
            void Run() override;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            void Quit() override;

            ~CDiscordClient() {}
        private:
            const char *BASE_URL = "https://discordapp.com/api";
            using Users = std::map<std::string, User>;
            using Guilds = std::map<std::string, Guild>;
            using VoiceSockets = std::map<std::string, VoiceSocket>;
            using AudioSources = std::map<std::string, AudioSource>;

            std::string m_Token;
            std::shared_ptr<SGateway> m_Gateway;
            ix::WebSocket m_Socket;
            std::thread m_Heartbeat;
            volatile bool m_Terminate;
            volatile bool m_HeartACKReceived;
            volatile bool m_Quit;
            uint32_t m_HeartbeatInterval;
            volatile uint32_t m_LastSeqNum;
            std::string m_SessionID;
            User m_BotUser;

            //Map of all users in different servers.
            Users m_Users;

            //All Guilds where the bot is in.
            Guilds m_Guilds;

            //All open voice connections.
            VoiceSockets m_VoiceSockets;

            AudioSources m_AudioSources;

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

            /**
             * @brief Called from voice socket if a audio source finished.
             */
            void OnSpeakFinish(const std::string &Guild);

            User CreateUser(CJSON &json);
            GuildMember CreateMember(CJSON &json);
            VoiceState CreateVoiceState(CJSON &json, Guild guild);
            Channel CreateChannel(CJSON &json);
            Message CreateMessage(CJSON &json);
    };
} // namespace DiscordBot


#endif //DISCORDCLIENT_HPP