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

#ifndef ICHANNEL_HPP
#define ICHANNEL_HPP

#include <memory>
#include <models/DiscordEnums.hpp>
#include <models/DiscordObject.hpp>
#include <models/Embed.hpp>
#include <models/File.hpp>
#include <models/PermissionOverwrites.hpp>
#include <models/Role.hpp>
#include <models/User.hpp>
#include <models/atomic.hpp>
#include <string>
#include <vector>

namespace DiscordBot
{  
    class IDiscordClient;
    class CMessage;

    class CChannelProperties;

    using Message = std::shared_ptr<CMessage>;

    class IChannel : public CDiscordObject
    {
        public:
            IChannel(IDiscordClient *client) : CDiscordObject(client), Position(0) {}

            ChannelTypes Type;
            std::atomic<int> Position;
            atomic<std::vector<PermissionOverwrites>> Overwrites;
            atomic<std::string> Name;
            atomic<std::string> Icon;
            atomic<std::string> ParentID;

            //DM
            atomic<std::vector<User>> Recipients;
            atomic<std::string> OwnerID;
            atomic<std::string> AppID;

            /**
             * @brief Modifies this channel.
             * 
             * @note The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Modify(const CChannelProperties &Modifications);

            /**
             * @brief Deletes this channel. This can't be undone.
             * 
             * @param Reason: Reason why the bot delete this channel.
             * 
             * @note The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Delete(const std::string &Reason);

            virtual ~IChannel() = default;
        private:
            /**
             * @brief Checks if the bot can manage channels.
             */
            void ManageChannelsCheck();
    };

    using Channel = std::shared_ptr<IChannel>;
}

#endif //ICHANNEL_HPP