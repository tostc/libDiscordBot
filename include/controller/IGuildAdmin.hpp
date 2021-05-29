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

#ifndef IGUILDADMIN_HPP
#define IGUILDADMIN_HPP

#include <memory>
#include <models/User.hpp>
#include <models/Channel.hpp>
#include <models/Guild.hpp>
#include <models/GuildMember.hpp>
#include <models/ModifyMember.hpp>
#include <models/ModifyChannel.hpp>
#include <models/Action.hpp>

namespace DiscordBot
{
    /**
     * @brief This interface covers all function where you need special rights on a server.
     */
    class IGuildAdmin
    {
        public:
            IGuildAdmin() = default;

            /**
             * @brief Modifies a member.
             * 
             * @param mod: Modification object.
             * 
             * @attention The bot needs some permissions which you can find inside the ::CModifyMember class.
             * 
             * @throw CDiscordClientException on error.
             */
            [[deprecated("Will be remove in the next release. Please use CGuildMember::Modify instead!")]]
            virtual void ModifyMember(const CModifyMember &mod) = 0;

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
            [[deprecated("Will be remove in the next release. Please use CGuild::Ban instead!")]]
            virtual void BanMember(User member, const std::string &Reason = "", int DeleteMsgDays = -1) = 0;

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
            [[deprecated("Will be remove in the next release. Please use CGuild::Unban instead!")]]
            virtual void UnbanMember(User user) = 0;
            
            /**
             * @attention The bot needs following permission `BAN_MEMBERS`
             * 
             * @return Returns a list of banned users of a guild. (ret.first = reason, ret.second = user)
             */
            [[deprecated("Will be remove in the next release. Please use CGuild::GetGuildBans instead!")]]
            virtual std::vector<std::pair<std::string, User>> GetGuildBans() = 0;

            /**
             * @brief Kicks a member from the guild.
             * 
             * @param member: Member to kick.
             * 
             * @attention The bot needs following permission `KICK_MEMBERS`
             */
            [[deprecated("Will be remove in the next release. Please use CGuild::Kick instead!")]]
            virtual void KickMember(User member) = 0;

            /**
             * @brief Create a new channel.
             * 
             * @param channel: Channel informations.
             * 
             * @attention The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CDiscordClientException on error.
             */
            [[deprecated("Will be remove in the next release. Please use CGuild::CreateChannel instead!")]]
            virtual void CreateChannel(const CModifyChannel &channel) = 0;

            /**
             * @brief Modifies a channel.
             * 
             * @param channel: Channel informations.
             * 
             * @attention The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CDiscordClientException on error.
             */
            [[deprecated("Will be remove in the next release. Please use CChannel::Modify instead!")]]
            virtual void ModifyChannel(const CModifyChannel &channel) = 0;

            /**
             * @brief Deletes a channel.
             * 
             * @param channel: Channel to delete.
             * 
             * @note This can't be undone.
             * @attention The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CDiscordClientException on error.
             */
            [[deprecated("Will be remove in the next release. Please use CChannel::Delete instead!")]]
            virtual void DeleteChannel(Channel channel, const std::string &reason) = 0;

            /**
             * @brief Add an action to a channel which is triggered, if a given event occured.
             * 
             * @param channel: Channel to add the action to or null for all channels.
             * @param action: Action which is triggered.
             * 
             * @throw CDiscordClientException on error.
             */
            virtual void AddChannelAction(Channel channel, Action action) = 0;

            /**
             * @brief Removes an action from a channel.
             * 
             * @param channel: Channel to remove the action from or null.
             * @param types: Action types to remove
             */
            virtual void RemoveChannelAction(Channel channel, ActionType types) = 0;

            virtual ~IGuildAdmin() = default;
    };

    using GuildAdmin = std::shared_ptr<IGuildAdmin>;
}

#endif //IGUILDADMIN_HPP