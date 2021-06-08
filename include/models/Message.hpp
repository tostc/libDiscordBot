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
#include <models/channels/TextChannel.hpp>
#include <models/Guild.hpp>
#include <string>
#include <models/DiscordEnums.hpp>
#include <models/DiscordObject.hpp>

namespace DiscordBot
{
    class IDiscordClient;

    struct SMention
    {
        MentionTypes Type;
        std::string ID;
    };

    class CMessage : public CDiscordObject
    {
        public:
            CMessage(IDiscordClient *client, Internal::CMessageManager *MsgMgr) : CDiscordObject(client, MsgMgr) {}

            TextChannel ChannelRef;     //!< Could contain a dummy channel if this is a dm. Only the id field is filled.
            Guild GuildRef;
            User Author;
            GuildMember Member;
            std::string Content;
            std::string Timestamp;
            std::string EditedTimestamp;
            bool Mention;
            std::vector<GuildMember> Mentions;  //!< Could contains  dummy Guild Members if this is a dm. Only the UserRef field is filled.
            std::vector<Role> RoleMentions;
            std::vector<TextChannel> ChannelMentions;   //!< Only textual channels that are visible to everyone.

            /**
             * @brief Parses a parameter for the Discords messaging format. https://discord.com/developers/docs/reference#message-formatting
             * 
             * @return Returns a list of mentions.
             */
            std::vector<SMention> ParseParam(const std::string &Param);

            /**
             * @brief Adds a reaction to this message.
             * 
             * @param Emoji: UTF8 Emoji to add.
             * 
             * @attention The bot needs following permissions `READ_MESSAGE_HISTORY` and `ADD_REACTIONS`
             */
            void CreateReaction(const std::string &Emoji);

            /**
             * @brief Removes all reactions from this message.
             * 
             * @attention The bot needs following permission `MANAGE_MESSAGES`
             */
            void DeleteAllReactions();

            /**
             * @param Emoji: UTF8 Emoji to get.
             * 
             * @return Gets a list of all Users which have reacted with the given emoji.
             */
            std::vector<User> GetReactions(const std::string &Emoji);

            /**
             * @brief Edits the message to a given channel.
             * 
             * @param Text: Text to send;
             * @param TTS: True to enable tts.
             */
            void Edit(const std::string &Text, Embed embed = nullptr, bool TTS = false);

            /**
             * @brief Just deletes the message.
             */
            void Delete();

            ~CMessage() = default;
        private:
            std::string UriEscape(const std::string &Uri);
    };

    using Message = std::shared_ptr<CMessage>;
} // namespace DiscordBot


#endif //MESSAGE_HPP