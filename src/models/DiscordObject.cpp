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

#include "../controller/DiscordClient.hpp"
#include <models/exceptions/PermissionException.hpp>
#include <models/DiscordObject.hpp>
#include <models/Guild.hpp>
#include <tinyformat.h>

namespace DiscordBot
{
    void CDiscordObject::CheckPermissions(Permission p, std::vector<PermissionOverwrites> Overwrites)
    {
        CDiscordClient *Client = dynamic_cast<CDiscordClient*>(m_Client);
        Guild guild = Client->GetGuild(GuildID);
        if(!Client->CheckPermissions(guild, p, Overwrites))
            throw CPermissionException(tfm::format("Missing permission: '%s'", PermissionToString(p)));
    }

    std::string CDiscordObject::PermissionToString(Permission val)
    {
        switch(val)
        {
            case Permission::CREATE_INSTANT_INVITE:
            {
                return "Permission::CREATE_INSTANT_INVITE";
            }break;

            case Permission::KICK_MEMBERS:
            {
                return "Permission::KICK_MEMBERS";
            }break;

            case Permission::BAN_MEMBERS:
            {
                return "Permission::BAN_MEMBERS";
            }break;

            case Permission::ADMINISTRATOR:
            {
                return "Permission::ADMINISTRATOR";
            }break;

            case Permission::MANAGE_CHANNELS:
            {
                return "Permission::MANAGE_CHANNELS";
            }break;

            case Permission::MANAGE_GUILD:
            {
                return "Permission::MANAGE_GUILD";
            }break;

            case Permission::ADD_REACTIONS:
            {
                return "Permission::ADD_REACTIONS";
            }break;

            case Permission::VIEW_AUDIT_LOG:
            {
                return "Permission::VIEW_AUDIT_LOG";
            }break;

            case Permission::PRIORITY_SPEAKER:
            {
                return "Permission::PRIORITY_SPEAKER";
            }break;

            case Permission::STREAM:
            {
                return "Permission::STREAM";
            }break;

            case Permission::VIEW_CHANNEL:
            {
                return "Permission::VIEW_CHANNEL";
            }break;

            case Permission::SEND_MESSAGES:
            {
                return "Permission::SEND_MESSAGES";
            }break;

            case Permission::SEND_TTS_MESSAGES:
            {
                return "Permission::SEND_TTS_MESSAGES";
            }break;

            case Permission::MANAGE_MESSAGES:
            {
                return "Permission::MANAGE_MESSAGES";
            }break;

            case Permission::EMBED_LINKS:
            {
                return "Permission::EMBED_LINKS";
            }break;

            case Permission::ATTACH_FILES:
            {
                return "Permission::ATTACH_FILES";
            }break;

            case Permission::READ_MESSAGE_HISTORY:
            {
                return "Permission::READ_MESSAGE_HISTORY";
            }break;

            case Permission::MENTION_EVERYONE:
            {
                return "Permission::MENTION_EVERYONE";
            }break;

            case Permission::USE_EXTERNAL_EMOJIS:
            {
                return "Permission::USE_EXTERNAL_EMOJIS";
            }break;

            case Permission::VIEW_GUILD_INSIGHTS:
            {
                return "Permission::VIEW_GUILD_INSIGHTS";
            }break;

            case Permission::CONNECT:
            {
                return "Permission::CONNECT";
            }break;

            case Permission::SPEAK:
            {
                return "Permission::SPEAK";
            }break;

            case Permission::MUTE_MEMBERS:
            {
                return "Permission::MUTE_MEMBERS";
            }break;

            case Permission::DEAFEN_MEMBERS:
            {
                return "Permission::DEAFEN_MEMBERS";
            }break;

            case Permission::MOVE_MEMBERS:
            {
                return "Permission::MOVE_MEMBERS";
            }break;

            case Permission::USE_VAD:
            {
                return "Permission::USE_VAD";
            }break;

            case Permission::CHANGE_NICKNAME:
            {
                return "Permission::CHANGE_NICKNAME";
            }break;

            case Permission::MANAGE_NICKNAMES:
            {
                return "Permission::MANAGE_NICKNAMES";
            }break;

            case Permission::MANAGE_ROLES:
            {
                return "Permission::MANAGE_ROLES";
            }break;

            case Permission::MANAGE_WEBHOOKS:
            {
                return "Permission::MANAGE_WEBHOOKS";
            }break;

            case Permission::MANAGE_EMOJIS:
            {
                return "Permission::MANAGE_EMOJIS";
            }break;

        }
        return "Unknown";
    }
} // namespace DiscordBot
