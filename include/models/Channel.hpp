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
#include <models/File.hpp>
#include <string>
#include <models/Role.hpp>
#include <models/atomic.hpp>
#include <models/Embed.hpp>

namespace DiscordBot
{  
    class IDiscordClient;
    class CMessage;

    using Message = std::shared_ptr<CMessage>;

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
            CChannel(IDiscordClient *client) : Position(0), NSFW(false), Bitrate(0), UserLimit(0), RateLimit(0), m_Client(client) {}

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

            /**
             * @brief Modifies this channel.
             * 
             * @note The bot needs following permission `MANAGE_CHANNELS`
             * 
             * @throw CPermissionException On missing permissions.
             * @throw CDiscordClientException On error.
             */
            void Modify(const CModifyChannel &Modifications);

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

            ~CChannel() {}
        private:
            /**
             * @brief Checks if the current channel is a text channel. Also checks if the bot has permissions to send messages.
             */
            void BasicSendMessageCheck(bool NeedTTS);

            /**
             * @brief Checks if the bot can manage channels.
             */
            void ManageChannelsCheck();

            IDiscordClient *m_Client;
    };

    using Channel = std::shared_ptr<CChannel>;
}

#endif //CHANNEL_HPP