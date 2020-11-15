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
            virtual void UnbanMember(User user) = 0;
            
            /**
             * @attention The bot needs following permission `BAN_MEMBERS`
             * 
             * @return Returns a list of banned users of a guild. (ret.first = reason, ret.second = user)
             */
            virtual std::vector<std::pair<std::string, User>> GetGuildBans() = 0;

            /**
             * @brief Kicks a member from the guild.
             * 
             * @param member: Member to kick.
             * 
             * @attention The bot needs following permission `KICK_MEMBERS`
             */
            virtual void KickMember(User member) = 0;

            virtual ~IGuildAdmin() = default;
    };

    using GuildAdmin = std::shared_ptr<IGuildAdmin>;
}

#endif //IGUILDADMIN_HPP