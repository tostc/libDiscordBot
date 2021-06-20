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

#include <atomic>
#include <DiscordBot/Channels/IChannel.hpp>
#include <DiscordBot/Properties/GuildMemberProperties.hpp>
#include <DiscordBot/Models/Role.hpp>
#include <DiscordBot/Models/User.hpp>
#include <DiscordBot/Voice/VoiceState.hpp>
#include <DiscordBot/Utils/Atomic.hpp>
#include <DiscordBot/Models/DiscordObject.hpp>
#include <string>
#include <vector>

namespace DiscordBot
{
    class IDiscordClient;

    class CGuildMember : public CDiscordObject
    {
        public:
            CGuildMember(IDiscordClient *Client, Internal::CMessageManager *MsgMgr) : CDiscordObject(Client, MsgMgr), Deaf(false), Mute(false) {}

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
             * @note The bot needs some permissions which you can find inside the ::CGuildMemberProperties class.
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Modify(const CGuildMemberProperties &Modifications);

            /**
             * @brief Moves the member to a voice channel.
             * 
             * @param channel: The channel where the user is moved to. A value of null means, kick the user from the voice channel.
             * 
             * @note The bot needs following permission `MOVE_MEMBERS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Move(Channel channel);

            ~CGuildMember() {}
        private:
    };

    using GuildMember = std::shared_ptr<CGuildMember>;
} // namespace DiscordBot


#endif //GUILDMEMBER_HPP