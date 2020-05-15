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

#ifndef VOICESTATE_HPP
#define VOICESTATE_HPP

#include <memory>
#include <models/Channel.hpp>
#include <models/User.hpp>
#include <string>

namespace DiscordBot
{
    class CGuild;
    using Guild = std::shared_ptr<CGuild>;

    class CVoiceState
    {
        public:
            CVoiceState() : Deaf(false), Mute(false), SelfDeaf(false), SelfMute(false), SelfStream(false), Supress(false) {}

            Guild GuildRef;
            Channel ChannelRef;
            User UserRef;
            std::string SessionID;
            bool Deaf;
            bool Mute;
            bool SelfDeaf;
            bool SelfMute;
            bool SelfStream;
            bool Supress;

            ~CVoiceState() {}
        private:
            /* data */
    };

    using VoiceState = std::shared_ptr<CVoiceState>;
} // namespace DiscordBot


#endif //VOICESTATE_HPP