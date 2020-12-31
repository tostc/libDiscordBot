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

#ifndef ACTIVITY_HPP
#define ACTIVITY_HPP

#include <memory>
#include <string>
#include <atomic>
#include <models/atomic.hpp>

namespace DiscordBot
{
    enum class ActivityType
    {
        GAME,
        STREAMING,
        LISTENING,
        CUSTOM
    };

    enum class ActivityFlags
    {
        INSTANCE = 1 << 0,
        JOIN = 1 << 1,
        SPECTATE = 1 << 2,
        JOIN_REQUEST = 1 << 3,
        SYNC = 1 << 4,
        PLAY = 1 << 5
    };

    inline ActivityFlags operator |(ActivityFlags lhs, ActivityFlags rhs)  
    {
        return static_cast<ActivityFlags>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }

    class CParty
    {
        public:
            CParty() {}

            atomic<std::string> ID;
            atomic<std::vector<int>> Size;    //(current_size, max_size)	used to show the party's current and maximum size

            ~CParty() {}
    };

    using Party = std::shared_ptr<CParty>;

    class CSecrets
    {
        public:
            CSecrets() {}

            atomic<std::string> Join;
            atomic<std::string> Spectate;
            atomic<std::string> Match;

            ~CSecrets() {}
    };

    using Secrets = std::shared_ptr<CSecrets>;

    class CActivity
    {
        public:
            CActivity(/* args */) {}

            atomic<std::string> Name;
            ActivityType Type;
            atomic<std::string> URL;
            std::atomic<int> CreatedAt;
            std::atomic<int> StartTime;
            std::atomic<int> EndTime;
            atomic<std::string> AppID;
            atomic<std::string> Details;
            atomic<std::string> State;
            //Emoji
            Party PartyObject;
            //Asset
            Secrets Secret;
            std::atomic<bool> Instance;
            ActivityFlags Flags;            

            ~CActivity() {}
    };

    using Activity = std::shared_ptr<CActivity>;
} // namespace DiscordBot


#endif //ACTIVITY_HPP