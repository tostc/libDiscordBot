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
#include <ixwebsocket/IXHttpClient.h>
#include <thread>
#include <map>
#include <models/User.hpp>
#include <models/Guild.hpp>
#include <models/Role.hpp>
#include <atomic>
#include "MessageManager.hpp"
#include "../model/Payload.hpp"
#include "VoiceSocket.hpp"

#undef SendMessage

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

            CDiscordClient(const std::string &Token, Intent Intents);

            /**
             * @brief Adds a song to the music queue.
             * 
             * @param guild: The guild which is associated with the queue.
             * @param Info: Song informations.
             */
            void AddToQueue(Guild guild, SongInfo Info) override;

            /**
             * @brief Connects to the given channel and uses the queue to speak.
             * 
             * @param channel: The voice channel to connect to.
             * 
             * @return Returns true if the connection succeeded.
             */
            bool StartSpeaking(Channel channel) override;

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
             * @brief Pauses the audio source. @see ResumeSpeaking to continue streaming.
             * 
             * @param guild: The guild to pause.
             */
            void PauseSpeaking(Guild guild) override;

            /**
             * @brief Resumes the audio source.
             * 
             * @param guild: The guild to resume.
             */
            void ResumeSpeaking(Guild guild) override;

            /**
             * @brief Stops the audio source.
             * 
             * @param guild: The guild to stop.
             */
            void StopSpeaking(Guild guild) override;

            /**
             * @brief Removes a song from the queue by its index.
             */
            void RemoveSong(Channel channel, size_t Index) override;

            /**
             * @brief Removes a song from the queue by its title or part of the title.
             */
            void RemoveSong(Channel channel, const std::string &Name) override;

            /**
             * @brief Joins a audio channel.
             * 
             * @param channel: The voice channel to join.
             */
            void Join(Channel channel) override;

            /**
             * @brief Leaves the audio channel.
             * 
             * @param guild: The guild to leave the voice channel.
             */
            void Leave(Guild guild) override;

            /**
             * @brief Sends a message to a given channel.
             * 
             * @param channel: Text channel which will receive the message.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            void SendMessage(Channel channel, const std::string Text, Embed embed = nullptr, bool TTS = false) override;

            /**
             * @return Returns the audio source for the given guild. Null if there is no audio source available.
             */
            AudioSource GetAudioSource(Guild guild) override;

            /**
             * @return Returns the music queue for the given guild. Null if there is no music queue available.
             */
            MusicQueue GetMusicQueue(Guild guild) override;

            /**
             * @return Returns true if a audio source is playing in the given guild.
             */
            bool IsPlaying(Guild guild) override;

            /**
             * @brief Runs the bot. The call returns if you calls Quit(). @see Quit()
             */
            void Run() override;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            void Quit() override;

            /**
             * @return Gets the bot user.
             */
            User GetBotUser() override
            {
                return m_BotUser;
            }

            ~CDiscordClient() {}
        private:
            enum
            {
                QUEUE_NEXT_SONG,
                RESUME,
                RECONNECT
            };

            const char *BASE_URL = "https://discordapp.com/api";
            using Users = std::map<std::string, User>;
            using Guilds = std::map<std::string, Guild>;
            using VoiceSockets = std::map<std::string, VoiceSocket>;
            using AudioSources = std::map<std::string, AudioSource>;
            using MusicQueues = std::map<std::string, MusicQueue>;

            CMessageManager m_EVManger;
            Intent m_Intents;

            std::mutex m_MusicQueueLock;
            std::mutex m_AudioSourcesLock;
            std::mutex m_VoiceSocketsLock;

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

            //Map of all users in different servers.
            Users m_Users;

            //All Guilds where the bot is in.
            Guilds m_Guilds;

            //All open voice connections.
            VoiceSockets m_VoiceSockets;

            AudioSources m_AudioSources;

            MusicQueues m_MusicQueues;

            /**
             * @brief Joins or leaves a voice channel.
             */
            void ChangeVoiceState(const std::string &Guild, const std::string &Channel = "");

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

            /**
             * @brief Called from voice socket if a audio source finished.
             */
            void OnSpeakFinish(const std::string &Guild);

            void OnQueueWaitFinish(const std::string &Guild, AudioSource Source);

            GuildMember GetMember(Guild guild, const std::string &UserID);

            User CreateUser(CJSON &json);
            GuildMember CreateMember(CJSON &json, Guild guild);
            VoiceState CreateVoiceState(CJSON &json, Guild guild);
            Channel CreateChannel(CJSON &json);
            Message CreateMessage(CJSON &json);
            Role CreateRole(CJSON &json);
    };
} // namespace DiscordBot


#endif //DISCORDCLIENT_HPP