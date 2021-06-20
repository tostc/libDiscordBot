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

#ifndef DISCORDOBJECT_HPP
#define DISCORDOBJECT_HPP

#include <DiscordBot/Models/DiscordEnums.hpp>
#include <DiscordBot/Utils/Snowflake.hpp>
#include <DiscordBot/Models/PermissionOverwrites.hpp>
#include <DiscordBot/Messages/MessageManager.hpp>
#include <vector>

namespace DiscordBot
{
    class IDiscordClient;

    class CDiscordObject
    {
        public:
            CDiscordObject(IDiscordClient *Client, Internal::CMessageManager *MsgMgr) : m_Client(Client), m_MsgMgr(MsgMgr) {}

            CSnowflake ID;
            CSnowflake GuildID;

            ~CDiscordObject() = default;
        protected:
            // INTERNAL APIS DO NOT USE!!!

            /**
             * @brief Checks for permissions.
             * 
             * @throw CPermissionException On missing permissions.
             */
            void CheckPermissions(Permission p, std::vector<PermissionOverwrites> Overwrites = std::vector<PermissionOverwrites>());
            std::string PermissionToString(Permission val);

            Internal::HttpMessage CreateHttpMessage(const std::string &url, const std::string &body, const std::string &contentType = "application/json");

            IDiscordClient *m_Client;
            Internal::CMessageManager *m_MsgMgr;
    };
} // namespace DiscordBot

#endif //DISCORDOBJECT_HPP