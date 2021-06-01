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

#ifndef GUILD_HPP
#define GUILD_HPP

#include <memory>
#include <map>
#include <string>
#include <models/User.hpp>
#include <models/channels/IChannel.hpp>
#include <models/GuildMember.hpp>
#include <models/Role.hpp>
#include <models/atomic.hpp>
#include <models/ChannelProperties.hpp>
#include <models/DiscordObject.hpp>

namespace DiscordBot
{
    class IDiscordClient;

    class CGuild : public CDiscordObject
    {
        public:
            using BanList = std::vector<std::pair<std::string, User>>;

            CGuild(IDiscordClient *Client) : CDiscordObject(Client) {}

            atomic<std::string> Name;
            atomic<std::string> Icon;

            GuildMember Owner;

            atomic<std::map<CSnowflake, GuildMember>> Members;
            atomic<std::map<CSnowflake, Channel>> Channels; 
            atomic<std::map<CSnowflake, Role>> Roles;

            /**
             * @brief Creates a new channel on the server.
             * 
             * @param ChannelInfo: Channel informations.
             * 
             * @note The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            Channel CreateChannel(const CChannelProperties &ChannelInfo);

            /**
             * @brief Bans a member from the guild.
             * 
             * @param Member: Member to ban.
             * @param Reason: Ban reason.
             * @param DeleteMsgDays: Deletes all messages of the banned user. (0 - 7 are valid values. -1 ignores the value.)
             * 
             * @note The bot needs following permission `BAN_MEMBERS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Ban(GuildMember Member, const std::string &Reason = "", int DeleteMsgDays = -1);

            /**
             * @brief Unbans a user.
             * 
             * @param user: User which is banned.
             * 
             * @note The bot needs following permission `BAN_MEMBERS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Unban(User user);

            /**
             * @brief Kicks a member from the guild.
             * 
             * @param Member: Member to kick.
             * 
             * @note The bot needs following permission `KICK_MEMBERS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Kick(GuildMember Member);

            /**
             * @note The bot needs following permission `BAN_MEMBERS`
             *
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             *  
             * @return Returns a list of banned users of a guild. (ret.first = reason, ret.second = user)
             */
            BanList GetBanList(); 

            ~CGuild() {}
        private:
            /**
             * @brief Checks if the bot can ban users.
             */
            void BanMembersCheck();
    };

    using Guild = std::shared_ptr<CGuild>;
}

#endif //GUILD_HPP