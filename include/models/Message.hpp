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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <models/GuildMember.hpp>
#include <models/Channel.hpp>
#include <models/Guild.hpp>

namespace DiscordBot
{
    class CMessage
    {
        public:
            CMessage(/* args */) {}

            std::string ID;
            Channel ChannelRef;     //!< Could contain a dummy channel if this is a dm. Only the id field is filled.
            Guild GuildRef;
            User Author;
            GuildMember Member;
            std::string Content;
            std::string Timestamp;
            std::string EditedTimestamp;
            bool Mention;
            std::vector<GuildMember> Mentions;  //!< Could contains  dummy Guild Members if this is a dm. Only the UserRef field is filled.

            ~CMessage() {}
        private:
        /* data */
    };

    using Message = std::shared_ptr<CMessage>;
} // namespace DiscordBot


#endif //MESSAGE_HPP