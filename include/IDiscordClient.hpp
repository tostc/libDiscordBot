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
#include <models/OnlineState.hpp>

namespace DiscordBot
{
    class IDiscordClient;
    using DiscordClient = std::shared_ptr<IDiscordClient>;
    using Users = std::map<std::string, User>;
    using Guilds = std::map<std::string, Guild>;

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

        GUILD_MEMBERS = (1 << 1),           //!< Please visit <a href="https://discord.com/developers/docs/topics/gateway#privileged-intents">this</a> website to use this intent.
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

        GUILD_PRESENCES = (1 << 8),         //!< Please visit <a href="https://discord.com/developers/docs/topics/gateway#privileged-intents">this</a> website to use this intent.
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
             * @brief Pauses the audio source. @see ResumeSpeaking to continue streaming.
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
             * @brief Sends a message to a given user.
             * 
             * @param user: Userwhich will receive the message.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            virtual void SendMessage(User user, const std::string Text, Embed embed = nullptr, bool TTS = false) = 0;

            /**
             * @brief Renames a guildmember.
             * 
             * @param member: Member to rename.
             * @param Name: The new name.
             * 
             * @attention The bot needs following permission `MANAGE_NICKNAMES`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void RenameMember(GuildMember member, const std::string &Name) = 0;

            /**
             * @brief Mutes a member in a voice channel.
             * 
             * @param member: Member to mute.
             * @param mute: True if the member should be mute.
             * 
             * @attention The bot needs following permission `MUTE_MEMBERS`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void MuteMember(GuildMember member, bool mute) = 0;

            /**
             * @brief Deafs a member in a voice channel.
             * 
             * @param member: Member to deaf.
             * @param mute: True if the member should be deaf.
             * 
             * @attention The bot needs following permission `DEAFEN_MEMBERS`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void DeafMember(GuildMember member, bool deaf) = 0;

            /**
             * @brief Modifies the roles of a member.
             * 
             * @param member: Member to modify.
             * @param Roles: Roles to assign.
             * 
             * @attention The bot needs following permission `MANAGE_ROLES`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void ModifyRoles(GuildMember member, std::vector<Role> Roles) = 0;

            /**
             * @brief Moves a member to a channel.
             * 
             * @param member: Member to move.
             * @param c: Channel to move to.
             * 
             * @note If c is null the user will be kicked from the voice channel.
             * 
             * @attention The bot needs following permission `MOVE_MEMBERS`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void MoveMember(GuildMember member, Channel c) = 0;

            /**
             * @brief Bans a member from the guild.
             * 
             * @param member: Member to ban.
             * @param Reason: Ban reason.
             * @param DeleteMsgDays: Deletes all messages of the banned user. (0 - 7 are valid values. -1 ignores the value.)
             * 
             * @attention The bot needs following permission `BAN_MEMBERS`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void BanMember(GuildMember member, const std::string &Reason = "", int DeleteMsgDays = -1) = 0;

            /**
             * @brief Unbans a user.
             * 
             * @param guild: The guild were the user is banned.
             * @param user: User which is banned.
             * 
             * @attention The bot needs following permission `BAN_MEMBERS`
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void UnbanMember(Guild guild, User user) = 0;
            
            /**
             * @attention The bot needs following permission `BAN_MEMBERS`
             * 
             * @return Returns a list of banned users of a guild. (ret.first = reason, ret.second = user)
             */
            virtual std::vector<std::pair<std::string, User>> GetGuildBans(Guild guild) = 0;

            /**
             * @brief Kicks a member from the guild.
             * 
             * @param member: Member to kick.
             * 
             * @attention The bot needs following permission `KICK_MEMBERS`
             */
            virtual void KickMember(GuildMember member) = 0;

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
            virtual Guild GetGuild(const std::string &GID) = 0;

            /**
             * @return Gets a list of all users.
             */
            virtual Users GetUsers() = 0;

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