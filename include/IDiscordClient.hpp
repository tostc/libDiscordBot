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
#include <models/DiscordEnums.hpp>
#include <controller/IGuildAdmin.hpp>

namespace DiscordBot
{
    class IDiscordClient;
    using DiscordClient = std::shared_ptr<IDiscordClient>;
    using Users = std::map<CSnowflake, User>;
    using Guilds = std::map<CSnowflake, Guild>;

    class DISCORDBOT_EXPORT IDiscordClient
    {
        public:
            IDiscordClient(/* args */) {}

            /**
             * @brief Registers a controller for this client. This controller receives all events of the client. @see IController for more informations.
             * 
             * @tparam T: Derived class of IController
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
             * @tparam T: Derived class of IMusicQueue
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
             * @brief Sets the online status of the bot.
             * 
             * @param state: Online state of the bot. @see State.
             */
            virtual void SetState(OnlineState state) = 0;

            /**
             * @brief Sets the bot AFK.
             * 
             * @param AFK: True if the bot is afk.
             */
            virtual void SetAFK(bool AFK) = 0;

            /**
             * @brief Sets the "Playing" status text or sets the bot in streaming mode.
             * 
             * @param Text: The text after "Playing"
             * @param URL: [Optional] A url to twitch or youtube, if this url is set the bot "Streams" on these platforms.
             */
            virtual void SetActivity(const std::string &Text, const std::string &URL = "") = 0;

            /**
             * @brief Runs the bot. The call returns if you calls Quit(). @see Quit()
             */
            virtual void Run() = 0;

            /**
             * @brief Quits the bot. And disconnects all voice states.
             */
            virtual void Quit() = 0;

            /**
             * @brief Same as Quit() but as asynchronous call. Internally Quit() is called. @see Quit()
             */
            virtual void QuitAsync() = 0;

            /**
             * @return Gets the bot user.
             */
            virtual User GetBotUser() = 0;

            /**
             * @return Gets the bot guild member of a given guild.
             */
            virtual GuildMember GetBotMember(Guild guild) = 0;

            /**
             * @return Gets the list of all connected servers.
             */
            virtual Guilds GetGuilds() = 0;

            /**
             * @return Gets a guild object by its id or null.
             */
            virtual Guild GetGuild(const CSnowflake &GID) = 0;

            /**
             * @return Gets a list of all users.
             */
            virtual Users GetUsers() = 0;

            /**
             * @param Token: Your Discord bot token. Which you have created <a href="https://discordapp.com/developers/applications">here</a>.
             * 
             * @return Returns a new DiscordClient object.
             */
            static DiscordClient Create(const std::string &Token, Intent Intents = Intent::DEFAULTS);

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