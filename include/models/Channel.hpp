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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <memory>
#include <vector>
#include <models/User.hpp>
#include <string>
#include <models/Role.hpp>
#include <models/atomic.hpp>

namespace DiscordBot
{
    enum class ChannelTypes
    {
        GUILD_TEXT,
        DM,
        GUILD_VOICE,
        GROUP_DM,
        GUILD_CATEGORY,
        GUILD_NEWS,
        GUILD_STORE
    };

    class CPermissionOverwrites
    {
        public:
            CPermissionOverwrites() {}

            atomic<std::string> ID;     //!< User or role id
            atomic<std::string> Type;   //!< role or user
            Permission Allow;
            Permission Deny;

            ~CPermissionOverwrites() {}
    };

    using PermissionOverwrites = std::shared_ptr<CPermissionOverwrites>;

    class CChannel
    {
        public:
            CChannel() : Position(0), NSFW(false), Bitrate(0), UserLimit(0), RateLimit(0) {}

            atomic<std::string> ID;
            ChannelTypes Type;
            atomic<std::string> GuildID;
            std::atomic<int> Position;
            atomic<std::vector<PermissionOverwrites>> Overwrites;
            atomic<std::string> Name;
            atomic<std::string> Topic;
            std::atomic<bool> NSFW;
            atomic<std::string> LastMessageID;
            std::atomic<int> Bitrate;
            std::atomic<int> UserLimit;
            std::atomic<int> RateLimit;
            atomic<std::vector<User>> Recipients;
            atomic<std::string> Icon;
            atomic<std::string> OwnerID;
            atomic<std::string> AppID;
            atomic<std::string> ParentID;
            atomic<std::string> LastPinTimestamp;

            ~CChannel() {}

        private:
        /* data */
    };

    using Channel = std::shared_ptr<CChannel>;
}

#endif //CHANNEL_HPP