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
     * @brief Access modes for commands.
     */
    enum class AccessMode
    {
        OWNER,          //!< This command can only be accessed by the owner.
        ROLE,           //!< This command can only be accessed by a given role.
        EVERYBODY       //!< Free for all.
    };

    DISCORDBOT_EXPORT std::string AccessModeToString(AccessMode Mode);

    /**
     * @brief Describes the command.
     */
    struct SCommandDescription
    {
        std::string Cmd;            //!< Command name.
        std::string Description;    //!< Command description. Shows in the help dialog.
        int ParamCount;             //!< Expected parameter count.
        std::string ParamDelimiter; //!< Delimiter for multiple parameters.
        AccessMode Mode;            //!< Access mode for this command. This is the default mode for a new server. The owner can access all commands. @see AccessMode
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
             * @param guild: Guild which contains the member
             * @param Member: Member which voice states has been updated.
             * 
             * @note The GUILD_VOICE_STATES intent needs to be set to receive this event. This intent is set by default. @see Intent
             */
            virtual void OnVoiceStateUpdate(Guild guild, GuildMember Member) {}

            /** 
             * @brief Called if a guild member joins.
             * 
             * @param guild: Guild which contains the member
             * @param Member: Member which has been join.
             * 
             * @note The GUILD_MEMBERS intent needs to be set to receive this event. Also you must activate "Server Members Intent" under the bot section of your Discord Application (Website).This intent is set by default. @see Intent
             */
            virtual void OnMemberAdd(Guild guild, GuildMember Member) {}

            /** 
             * @brief Called if a guild member updates. E.g. Nick, premium support, roles
             * 
             * @param guild: Guild which contains the member
             * @param Member: Member which has been updated.
             * 
             * @note The GUILD_MEMBERS intent needs to be set to receive this event. Also you must activate "Server Members Intent" under the bot section of your Discord Application (Website).This intent is set by default. @see Intent
             */
            virtual void OnMemberUpdate(Guild guild, GuildMember Member) {}

            /** 
             * @brief Called if a guild member leaves a guild. E.g. Ban, kick, leave
             * 
             * @param guild: Guild which had contains the member
             * @param Member: Member which leaves
             * 
             * @note The GUILD_MEMBERS  Also you must activate "Server Members Intent" under the bot section of your Discord Application (Website).This intent is set by default. @see Intent
             */
            virtual void OnMemberRemove(Guild guild, GuildMember Member) {}

            /**
             * @brief Called if a user changes his activity state or online state.
             * 
             * @param guild: Guild which had contains the member
             * @param Member: Member which updates
             * 
             * @note The GUILD_PRESENCES intent needs to be set to receive this event. Please visit <a href="https://discord.com/developers/docs/topics/gateway#privileged-intents">this</a> website to use this intent.
             */
            virtual void OnPresenceUpdate(Guild guild, GuildMember Member) {}

            /**
             * @brief Called if a new message was sended. Process the message and call associated commands.
             * 
             * @note Integrated help command "Prefix h" or "Prefix help".
             * 
             * @param msg: Message object. @see CMessage for more informations.
             * 
             * @note The GUILD_MESSAGES intent needs to be set to receive this event. This intent is set by default. @see Intent
             */
            void OnMessage(Message msg);

            /**
             * @brief Called if a message is updated.
             * 
             * @param msg: Message object which contains the update data.
             * 
             * @note The GUILD_MESSAGES intent needs to be set to receive this event. This intent is set by default. @see Intent
             */
            virtual void OnMessageEdited(Message msg) {}

            /**
             * @brief Called if a message is deleted.
             * 
             * @param msg: Partial message object which contains the message id, guild and channel.
             * 
             * @note The GUILD_MESSAGES intent needs to be set to receive this event. This intent is set by default. @see Intent
             */
            virtual void OnMessageDeleted(Message msg) {}

            /**
             * @brief Called if a guild becomes available, either after OnReady or if a guild becomes available again.
             * 
             * @param guild: Guild which comes available.
             */
            virtual void OnGuildAvailable(Guild guild) {}

            /**
             * @brief Called if the bot joins a new guild.
             * 
             * @param guild: The joined guild.
             */
            virtual void OnGuildJoin(Guild guild) {}

            /**
             * @brief Called if a guild becomes unavailable.
             * 
             * @param guild: Guild which comes unavailable.
             */
            virtual void OnGuildUnavailable(Guild guild) {}

            /**
             * @brief Called if the bot leaves a guild.
             * 
             * @param guild: The leaved guild.
             */
            virtual void OnGuildLeave(Guild guild) {}

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
             * @return Gets the prefix for a given guild or the default prefix, if no ones is configured.
             */
            std::string GetPrefix(Guild g);

            /**
             * @return Gets the commands config.
             */
            inline CommandsConfig GetCmdConfig()
            {
                return CmdsConfig;
            }

            inline bool CommandExists(const std::string &Cmd)
            {
                auto IT = m_CommandDescs.find(Cmd);
                return IT != m_CommandDescs.end();
            }

            inline AccessMode GetAccessMode(const std::string &Cmd)
            {
                return m_CommandDescs[Cmd].Mode;
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
                m_Commands[Desc.Cmd] = tmp;
            }

            /**
             * @brief Called if a new message was sended.
             * 
             * @param msg: Message object. @see CMessage for more informations.
             * @param Handled: Set to true if no command which is associated with the message should called.
             */
            virtual void OnMessage(Message msg, bool &Handled) {}
    
            IDiscordClient *Client;
            std::string Prefix;         //!< Default command prefix.
            CommandsConfig CmdsConfig;  //!< Member to save and load the config for the different commands. @see ICommandsConfig for more informations

        private:
            /**
             * @return Returns true if the user can access the command.
             */
            bool CanAccess(Guild guild, GuildMember member, const std::string &Cmd);

            using Factory = std::shared_ptr<IFactory<ICommand>>;
            std::map<std::string, Factory> m_Commands;
            std::map<std::string, SCommandDescription> m_CommandDescs;
    };

    using Controller = std::shared_ptr<IController>;
} // namespace DiscordBot


#endif //ICONTROLLER_HPP