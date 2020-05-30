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

#ifndef ICONTROLLER_HPP
#define ICONTROLLER_HPP

#include <string>
#include <models/Message.hpp>
#include <controller/ICommand.hpp>
#include <tuple>
#include <controller/Factory.hpp>
#include <controller/ICommandsConfig.hpp>
#include <config.h>

namespace DiscordBot
{
    class IDiscordClient;

    /**
     * @brief Describes the command.
     */
    struct SCommandDescription
    {
        std::string Cmd;            //!< Command name.
        std::string Description;    //!< Command description. Shows in the help dialog.
        int ParamCount;             //!< Expected parameter count.
        std::string ParamDelimiter; //!< Delimiter for multiple parameters.
    };

    /**
     * @brief Controller interface which receives events from the client.
     * 
     * @note All callbacks can called from different threads.
     */
    class DISCORDBOT_EXPORT IController
    {
        public:
            IController(IDiscordClient *client);

            /**
             * @brief Called if the handshake with discord is finished.
             */
            virtual void OnReady() {}

            /** 
             * @brief Called if the voice state of a guild member updates. Eg. move, connect, disconnect.
             * 
             * @param guild: Guild wich contains the member
             * @param Member: Member which voice states has been updated.
             */
            virtual void OnVoiceStateUpdate(Guild guild, GuildMember Member) {}

            /**
             * @brief Called if a new message was sended. Process the message and call associated commands.
             * 
             * @note Integrated help command "Prefix h" or "Prefix help".
             * 
             * @param msg: Message object. See @see ::CMessage for more informations.
             */
            void OnMessage(Message msg);

            /**
             * @brief Called if a audio source finished playing.
             * 
             * @param guild: Guild where the audio source finished.
             */
            virtual void OnEndSpeaking(Guild guild) {}

            /**
             * @brief Called if the client resumes the session with discord.
             */
            virtual void OnResume() {}

            /**
             * @brief Called if the bot lose the connection to discord or when the bot quits.
             */
            virtual void OnDisconnect() {}

            /**
             * @brief Called if @see IDiscordClient::Quit() is called.
             * 
             * @note In some situations the Quit() method is called internally from the client. This occurs if there is a huge error.
             */
            virtual void OnQuit() {}

            /**
             * @return Gets the command prefix.
             */
            inline std::string GetPrefix()
            {
                return Prefix;
            }

            /**
             * @return Gets all commands for a member. Only commands which the member can execute will be return.
             */
            std::vector<SCommandDescription> GetCommands(Guild guild, GuildMember member);

            virtual ~IController() = default;

        protected:
            template<class T, class ...Args, typename std::enable_if<std::is_base_of<ICommand, T>::value>::type* = nullptr>
            inline void RegisterCommand(SCommandDescription Desc, Args ...args)
            {
                m_CommandDescs[Desc.Cmd] = Desc;
                auto Tuple = std::make_tuple(args...);

                Factory tmp = Factory(new CFactory<ICommand, T, decltype(Tuple)>(Tuple));
                // CommandExecuter tmp = CommandExecuter(new SCommandExecuter<T, decltype(Tuple)>(Tuple));
                m_Commands[Desc.Cmd] = tmp;
            }

            /**
             * @brief Called if a new message was sended.
             * 
             * @param msg: Message object. See @see ::CMessage for more informations.
             * @param Handled: Set to true if no command which is associated with the message should called.
             */
            virtual void OnMessage(Message msg, bool &Handled) {}
    
            IDiscordClient *Client;
            std::string Prefix;     //!< Command prefix.
            CommandsConfig CmdsConfig;  //!< Member to save and load the config for the different commands.

        private:
            using Factory = std::shared_ptr<IFactory<ICommand>>;
            std::map<std::string, Factory> m_Commands;
            std::map<std::string, SCommandDescription> m_CommandDescs;
    };

    using Controller = std::shared_ptr<IController>;
} // namespace DiscordBot


#endif //ICONTROLLER_HPP