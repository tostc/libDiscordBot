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
#include <models/Embed.hpp>
#include <controller/IMusicQueue.hpp>
#include <controller/Factory.hpp>
#include <config.h>

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

        DIRECT_MESSAGE_TYPING = (1 << 14),
        //- TYPING_START

        DEFAULTS = GUILDS | GUILD_VOICE_STATES | GUILD_MESSAGES | DIRECT_MESSAGES | GUILD_MEMBERS
    };

    inline Intent operator |(Intent lhs, Intent rhs)  
    {
        return static_cast<Intent>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }  

    class DISCORDBOT_EXPORT IDiscordClient
    {
        public:
            IDiscordClient(/* args */) {}

            /**
             * @brief Registers a controller for this client. This controller receives all events of the client. See @see IController for more informations.
             * 
             * @tparam ...Args: Arguments which are passed to the constructer of the controller. 
             */
            template<class T, class ...Args, typename std::enable_if<std::is_base_of<IController, T>::value>::type* = nullptr>
            inline void RegisterController(Args&& ...args)
            {
                m_Controller = Controller(new T(this, std::forward<Args...>(args)...));
            }

            /**
             * @brief Registers a music queue template for this client. This queue type will created for each connected voice server and handles the audio.
             * 
             * @tparam ...Args: Arguments which are passed to the constructer of the music queue. 
             */
            template<class T, class ...Args, typename std::enable_if<std::is_base_of<IMusicQueue, T>::value>::type* = nullptr>
            inline void RegisterMusicQueue(Args&& ...args)
            {
                auto Tuple = std::make_tuple(args...);
                m_QueueFactory = Factory(new CFactory<IMusicQueue, T, decltype(Tuple)>(Tuple));
            }

            /**
             * @return Gets the controller which is associated with the client.
             */
            inline Controller GetCurrentController()
            {
                return m_Controller;
            }

            /**
             * @brief Adds a song to the music queue.
             * 
             * @param guild: The guild which is associated with the queue.
             * @param Info: Song informations.
             */
            virtual void AddToQueue(Guild guild, SongInfo Info) = 0;

            /**
             * @brief Connects to the given channel and uses the queue to speak.
             * 
             * @param channel: The voice channel to connect to.
             * 
             * @return Returns true if the connection succeeded.
             */
            virtual bool StartSpeaking(Channel channel) = 0;

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
             * @param guild: The guild to pause.
             */
            virtual void PauseSpeaking(Guild guild) = 0;

            /**
             * @brief Resumes the audio source.
             * 
             * @param guild: The guild to resume.
             */
            virtual void ResumeSpeaking(Guild guild) = 0;

            /**
             * @brief Stops the audio source.
             * 
             * @param guild: The guild to stop.
             */
            virtual void StopSpeaking(Guild guild) = 0;

            /**
             * @brief Removes a song from the queue by its index.
             */
            virtual void RemoveSong(Channel channel, size_t Index) = 0;

            /**
             * @brief Removes a song from the queue by its title or part of the title.
             */
            virtual void RemoveSong(Channel channel, const std::string &Name) = 0;

            /**
             * @brief Joins a audio channel.
             * 
             * @param channel: The voice channel to join.
             */
            virtual void Join(Channel channel) = 0;

            /**
             * @brief Leaves the audio channel.
             * 
             * @param guild: The guild to leave the voice channel.
             */
            virtual void Leave(Guild guild) = 0;

            /**
             * @brief Sends a message to a given channel.
             * 
             * @param channel: Text channel which will receive the message.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            virtual void SendMessage(Channel channel, const std::string Text, Embed embed = nullptr, bool TTS = false) = 0;

            /**
             * @return Returns the audio source for the given guild. Null if there is no audio source available.
             */
            virtual AudioSource GetAudioSource(Guild guild) = 0;

            /**
             * @return Returns the music queue for the given guild. Null if there is no music queue available.
             */
            virtual MusicQueue GetMusicQueue(Guild guild) = 0;

            /**
             * @return Returns true if a audio source is playing in the given guild.
             */
            virtual bool IsPlaying(Guild guild) = 0;

            /**
             * @brief Runs the bot. The call returns if you calls @see Quit().
             */
            virtual void Run() = 0;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            virtual void Quit() = 0;

            /**
             * @return Gets the bot user.
             */
            virtual User GetBotUser() = 0;

            /**
             * @param Token: Your Discord bot token. Which you have created <a href="https://discordapp.com/developers/applications">here</a>.
             * 
             * @return Returns a new DiscordClient object.
             */
            static DISCORDBOT_EXPORT DiscordClient Create(const std::string &Token, Intent Intents = Intent::DEFAULTS);

            /**
             * @return Returns the Version of the library.
             */
            static inline std::string GetVersion()
            {
                return std::string(VERSION);
            }

            virtual ~IDiscordClient() {}

        protected:
            Controller m_Controller;

            using Factory = std::shared_ptr<IFactory<IMusicQueue>>;
            Factory m_QueueFactory;
    };
} // namespace DiscordBot


#endif //IDISCORDCLIENT_HPP