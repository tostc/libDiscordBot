/*
 * MIT License
 *
 * Copyright = (c) 2020 Christian Tost
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files = (the "Software"), to deal
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

#ifndef IDISCORDCLIENT_HPP
#define IDISCORDCLIENT_HPP

#include <memory>
#include <controller/IController.hpp>
#include <controller/IAudioSource.hpp>

namespace DiscordBot
{
    class IDiscordClient;
    using DiscordClient = std::shared_ptr<IDiscordClient>;

    //Discord Gateway intents https://discordapp.com/developers/docs/topics/gateway#gateway-intents
    enum class Intent
    {
        GUILDS = (1 << 0),
        //- GUILD_CREATE
        //- GUILD_UPDATE
        //- GUILD_DELETE
        //- GUILD_ROLE_CREATE
        //- GUILD_ROLE_UPDATE
        //- GUILD_ROLE_DELETE
        //- CHANNEL_CREATE
        //- CHANNEL_UPDATE
        //- CHANNEL_DELETE
        //- CHANNEL_PINS_UPDATE

        GUILD_MEMBERS = (1 << 1),
        //- GUILD_MEMBER_ADD
        //- GUILD_MEMBER_UPDATE
        //- GUILD_MEMBER_REMOVE

        GUILD_BANS = (1 << 2),
        //- GUILD_BAN_ADD
        //- GUILD_BAN_REMOVE

        GUILD_EMOJIS = (1 << 3),
        //- GUILD_EMOJIS_UPDATE

        GUILD_INTEGRATIONS = (1 << 4),
        //- GUILD_INTEGRATIONS_UPDATE

        GUILD_WEBHOOKS = (1 << 5),
        //- WEBHOOKS_UPDATE

        GUILD_INVITES = (1 << 6),
        //- INVITE_CREATE
        //- INVITE_DELETE

        GUILD_VOICE_STATES = (1 << 7),
        //- VOICE_STATE_UPDATE

        GUILD_PRESENCES = (1 << 8),
        //- PRESENCE_UPDATE

        GUILD_MESSAGES = (1 << 9),
        //- MESSAGE_CREATE
        //- MESSAGE_UPDATE
        //- MESSAGE_DELETE
        //- MESSAGE_DELETE_BULK

        GUILD_MESSAGE_REACTIONS = (1 << 10),
        //- MESSAGE_REACTION_ADD
        //- MESSAGE_REACTION_REMOVE
        //- MESSAGE_REACTION_REMOVE_ALL
        //- MESSAGE_REACTION_REMOVE_EMOJI

        GUILD_MESSAGE_TYPING = (1 << 11),
        //- TYPING_START

        DIRECT_MESSAGES = (1 << 12),
        //- CHANNEL_CREATE
        //- MESSAGE_CREATE
        //- MESSAGE_UPDATE
        //- MESSAGE_DELETE
        //- CHANNEL_PINS_UPDATE

        DIRECT_MESSAGE_REACTIONS = (1 << 13),
        //- MESSAGE_REACTION_ADD
        //- MESSAGE_REACTION_REMOVE
        //- MESSAGE_REACTION_REMOVE_ALL
        //- MESSAGE_REACTION_REMOVE_EMOJI

        DIRECT_MESSAGE_TYPING = (1 << 14)
        //- TYPING_START
    };

    inline Intent operator |(Intent lhs, Intent rhs)  
    {
        return static_cast<Intent>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }  

    class IDiscordClient
    {
        public:
            IDiscordClient(/* args */) {}

            /**
             * @brief Registers a controller for this client. This controller receives all events of the client. See @see ::IController for more informations.
             * 
             * @tparam ...Args: Arguments which are passed to the constructer of the controller. 
             */
            template<class T, class ...Args, typename std::enable_if<std::is_base_of<IController, T>::value>::type* = nullptr>
            void RegisterController(Args&& ...args)
            {
                m_Controller = std::unique_ptr<IController>(new T(std::forward<Args...>(args)...));
                m_Controller->Client = this;
            }

            /**
             * @brief Connects to the given channel and uses the source to speak.
             * 
             * @param channel: The voice channel to connect to.
             * @param source: The audio source for speaking.
             * 
             * @return Returns true if the connection succeeded.
             */
            virtual bool StartSpeaking(Channel channel, AudioSource source) = 0;

            /**
             * @brief Pauses the audio source. Call @see ResumeSpeaking to continue streaming.
             * 
             * @param channel: The voice channel to pause.
             */
            virtual void PauseSpeaking(Channel channel) = 0;

            /**
             * @brief Resumes the audio source.
             * 
             * @param channel: The voice channel to resume.
             */
            virtual void ResumeSpeaking(Channel channel) = 0;

            /**
             * @brief Stops the audio source.
             * 
             * @param channel: The voice channel to stop.
             */
            virtual void StopSpeaking(Channel channel) = 0;

            /**
             * @brief Joins a audio channel.
             * 
             * @param channel: The voice channel to join.
             */
            virtual void Join(Channel channel) = 0;

            /**
             * @brief Leaves the audio channel.
             * 
             * @param channel: The voice channel to leave.
             */
            virtual void Leave(Channel channel) = 0;

            /**
             * @brief Sends a message to a given channel.
             * 
             * @param channel: Text channel which will receive the message.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            virtual void SendMessage(Channel channel, const std::string Text, bool TTS = false) = 0;

            /**
             * @return Returns the audio source for the given channel. Null if there is no audio source available.
             */
            virtual AudioSource GetAudioSource(Channel channel) = 0;

            /**
             * @brief Runs the bot. The call returns if you calls @see Quit().
             */
            virtual void Run() = 0;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            virtual void Quit() = 0;

            /**
             * @param Token: Your Discord bot token. Which you have created <a href="https://discordapp.com/developers/applications">here</a>.
             * 
             * @return Returns a new DiscordClient object.
             */
            static DiscordClient Create(const std::string &Token);

            virtual ~IDiscordClient() {}

        protected:
            std::unique_ptr<IController> m_Controller;
    };
} // namespace DiscordBot


#endif //IDISCORDCLIENT_HPP