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

#ifndef GUILDMEMBER_HPP
#define GUILDMEMBER_HPP

#include <models/User.hpp>
#include <models/VoiceState.hpp>
#include <vector>
#include <string>
#include <models/Role.hpp>
#include <atomic>
#include <models/atomic.hpp>
#include <models/ModifyMember.hpp>

namespace DiscordBot
{
    class IDiscordClient;

    class CGuildMember
    {
        public:
            CGuildMember(IDiscordClient *Client) : m_Client(Client), Deaf(false), Mute(false) {}

            atomic<std::string> GuildID;
            User UserRef;
            atomic<std::string> Nick;
            atomic<std::vector<Role>> Roles;
            atomic<std::string> JoinedAt;
            atomic<std::string> PremiumSince;
            std::atomic<bool> Deaf;
            std::atomic<bool> Mute;

            VoiceState State;

            /**
             * @brief Modifies a this member.
             * 
             * @param Modifications: Modification object.
             * 
             * @note The bot needs some permissions which you can find inside the ::CModifyMember class.
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Modify(const CModifyMember &Modifications);

            ~CGuildMember() {}
        private:
            IDiscordClient *m_Client;
    };

    using GuildMember = std::shared_ptr<CGuildMember>;
} // namespace DiscordBot


#endif //GUILDMEMBER_HPP