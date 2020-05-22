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

#ifndef VOICESOCKET_HPP
#define VOICESOCKET_HPP

#include <JSON.hpp>
#include <controller/IAudioSource.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUdpSocket.h>
#include <atomic>
#include "MessageManager.hpp"

namespace DiscordBot
{    
    using OnStopSpeaking = std::function<void(const std::string&)>;

    /**
     * @brief Manages all voice events. Also encode and encrypts audio. 
     */
    class CVoiceSocket
    {
        public:
            //All informations from https://discordapp.com/developers/docs/topics/opcodes-and-status-codes#voice
            enum class OPCodes
            {
                //Name                  Code        Sent By             Description
                IDENTIFY                = 0,        //client            Begin a voice websocket connection.
                SELECT_PROTOCOL         = 1,        //client            Select the voice protocol.
                READY                   = 2,        //server            Complete the websocket handshake.
                HEARTBEAT               = 3,        //client	        Keep the websocket connection alive.
                SESSION_DESCIPTION      = 4,        //server            Describe the session.
                SPEAKING                = 5,        //client and server Indicate which users are speaking.
                HEARTBEAT_ACK           = 6,        //server	        Sent to acknowledge a received client heartbeat.
                RESUME                  = 7,        //client	        Resume a connection.
                HELLO                   = 8,        //server	        Time to wait between sending heartbeats in milliseconds.
                RESUMED                 = 9,        //server	        Acknowledge a successful session resume.
                CLIENT_DISCONNECT       = 13        //server            A client has disconnected from the voice channel
            };

            /**
             * @param json: JSON from VOICE_SERVER_UPDATE event,
             * @param SessionID: Session ID of the bot voice state.
             * @param ClientID: Bot client ID.
             */
            CVoiceSocket(CJSON &json, const std::string &SessionID, const std::string &ClientID);

            /**
             * @brief Sets the callback which is called if the audio source finished.
             */
            void SetOnSpeakFinish(OnStopSpeaking call)
            {
                m_Callback = call;
            }

            /**
             * @brief Starts a new audio stream. Stops the old one.
             * 
             * @param Source: Audiosource which is send to discord.
             */
            void StartSpeaking(AudioSource Source);

            /**
             * @brief Pause the sending of audio.
             */
            void PauseSpeaking();

            /**
             * @brief Resumes the sending of audio.
             */
            void ResumeSpeaking();

            /**
             * @brief Stops the sending of audio. Raise a OnSpeakFinish event.
             */
            void StopSpeaking();

            /**
             * @return Gets the current playing audio source or null.
             */
            AudioSource GetAudioSource()
            {
                return m_Source;
            }

            ~CVoiceSocket();
        private:
            static const int FREQUENCY = 48000;     //!< Supported sample rate of Discord.
            static const int CHANNEL = 2;           //!< Supported channel count of Discord.
            static const int MILLISECONDS = 20;     //!< Time of samples wich will be send.
            static const int RTPHEADERSIZE = 12;    //!< Size of the rtp header.
            static const int NONCESIZE = RTPHEADERSIZE * 2; //!< Size of the key salt.
            static const int PACKET_CACHE = 1000 / MILLISECONDS;    //!< Cache Packets for 1 second.

            enum
            {
                RESUME
            };

            CMessageManager m_EVManager;
            OnStopSpeaking m_Callback;

            std::string m_Token;
            std::string m_ClientID;
            std::string m_GuildID;
            ix::WebSocket m_Socket;
            ix::UdpSocket m_UDPSocket;
            std::thread m_Heartbeat;
            std::atomic<bool> m_Terminate;
            std::atomic<bool> m_HeartACKReceived;
            uint32_t m_HeartbeatInterval;
            std::atomic<uint32_t> m_LastSeqNum;
            std::string m_SessionID;

            AudioSource m_Source;
            std::atomic<bool> m_Stop;
            std::atomic<bool> m_Pause;
            std::atomic<bool> m_Reconnect;
            std::thread m_Playback;

            std::vector<uint8_t> m_SecKey;

            uint32_t m_SSRC;

            /**
             * @brief Handles async. Messages.
             */
            void OnMessageReceive(MessageBase Msg);

            /**
             * @brief Builds and sends a payload object.
             */
            void SendOP(OPCodes OP, const std::string &D);

            /**
             * @brief Receives all websocket events from discord. This is the heart of the voice.
             */
            void OnWebsocketEvent(const ix::WebSocketMessagePtr& msg);

            /**
             * @brief Sends a heartbeat.
             */
            void Heartbeat();

            /**
             * @brief Encode, encrypt and send audio data.
             */
            void Playback();

            /**
             * @brief Informates Discord that the bot begins to speak or is finish with speaking.
             */
            void SetSpeaking(bool Speak);
    };

    using VoiceSocket = std::shared_ptr<CVoiceSocket>;
} // namespace DiscordBot


#endif //VOICESOCKET_HPP