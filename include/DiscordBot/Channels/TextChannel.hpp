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

#ifndef TEXTCHANNEL_HPP
#define TEXTCHANNEL_HPP

#include <DiscordBot/Channels/IChannel.hpp>

namespace DiscordBot
{
    class CTextChannel : public IChannel
    {
        public:
            CTextChannel(IDiscordClient *Client, Internal::CMessageManager *MsgMgr) : IChannel(Client, MsgMgr), NSFW(false), RateLimit(0) {}

            atomic<std::string> Topic;
            std::atomic<bool> NSFW;
            CSnowflake LastMessageID;
            std::atomic<int> RateLimit;
            atomic<std::string> LastPinTimestamp;

            /**
             * @brief Sends a message with attachment to a given channel.
             * 
             * @param File: File to attach.
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             * 
             * @note Needs the 'SEND_MESSAGE' permission to send messages. For TTS you need also 'SEND_TTS_MESSAGES'
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException If the current channel is not a text channel.
             * 
             * @return Returns a message object.
             */
            Message SendMessage(CFile &File, const std::string &Text, Embed embed = nullptr, bool TTS = false);

            /**
             * @brief Sends a message to a given channel.
             * 
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             * 
             * @note Needs the 'SEND_MESSAGE' permission to send messages. For TTS you need also 'SEND_TTS_MESSAGES'
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException If the current channel is not a text channel.
             * 
             * @return Returns a message object.
             */
            Message SendMessage(const std::string &Text, Embed embed = nullptr, bool TTS = false);

            ~CTextChannel() = default;
        private:
            /**
             * @brief Checks if the current channel is a text channel. Also checks if the bot has permissions to send messages.
             */
            void BasicSendMessageCheck(bool NeedTTS);
    };

    using TextChannel = std::shared_ptr<CTextChannel>;
} // namespace DiscordBot


#endif //TEXTCHANNEL_HPP