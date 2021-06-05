/*
 * MIT License
 *
 * Copyright (c) 2021 Christian Tost
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

#ifndef VOICECHANNEL_HPP
#define VOICECHANNEL_HPP

#include <controller/VoiceClient.hpp>
#include <models/channels/IChannel.hpp>
#include <memory>

namespace DiscordBot
{
    class CVoiceChannel : public IChannel
    {
        public:
            CVoiceChannel(IDiscordClient *Client, Internal::CMessageManager *MsgMgr) : IChannel(Client, MsgMgr), Bitrate(0), UserLimit(0) {}

            std::atomic<int> Bitrate;
            std::atomic<int> UserLimit;

            /**
             * @brief Joins a audio channel.
             */
            VoiceClient Join();

            /**
             * @brief Leaves the audio channel.
             */
            // void Leave();

            ~CVoiceChannel() = default;
    };

    using VoiceChannel = std::shared_ptr<CVoiceChannel>;
} // namespace DiscordBot

#endif //VOICECHANNEL_HPP