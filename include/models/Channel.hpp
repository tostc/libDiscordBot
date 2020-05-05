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

            std::string ID;
            std::string Type;
            int Allow;
            int Deny;

            ~CPermissionOverwrites() {}
    };

    using PermissionOverwrites = std::shared_ptr<CPermissionOverwrites>;

    class CChannel
    {
        public:
            CChannel() : Position(0), NSFW(false), Bitrate(0), UserLimit(0), RateLimit(0) {}

            std::string ID;
            ChannelTypes Type;
            std::string GuildID;
            int Position;
            std::vector<PermissionOverwrites> Overwrites;
            std::string Name;
            std::string Topic;
            bool NSFW;
            std::string LastMessageID;
            int Bitrate;
            int UserLimit;
            int RateLimit;
            std::vector<User> Recipients;
            std::string Icon;
            std::string OwnerID;
            std::string AppID;
            std::string ParentID;
            std::string LastPinTimestamp;

            ~CChannel() {}

        private:
        /* data */
    };

    using Channel = std::shared_ptr<CChannel>;
}

#endif //CHANNEL_HPP