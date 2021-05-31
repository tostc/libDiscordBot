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
#include <models/channels/IChannel.hpp>
#include <models/Guild.hpp>
#include <models/GuildMember.hpp>
#include <models/GuildMemberProperties.hpp>
#include <models/ChannelProperties.hpp>
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