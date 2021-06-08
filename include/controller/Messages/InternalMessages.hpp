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

#ifndef INTERNALMESSAGES_HPP
#define INTERNALMESSAGES_HPP

#include <models/Snowflake.hpp>
#include <controller/Messages/MessageResult.hpp>
#include <memory>
#include <models/PermissionOverwrites.hpp>
#include <models/DiscordEnums.hpp>
#include <vector>

namespace DiscordBot
{
    // Internal API don't use.
    namespace Internal
    {
        enum Requests
        {
            JOIN,

            PERMISSIONS,

            // HTTP operations
            PUT,
            POST,
            PATCH,
            GET,
            DELETE,

            RESUME,
            RECONNECT,
            QUIT
        };

        class IInternalMessage
        {
            public:
                CSnowflake ID;
                MessageResult Res;
        };

        class CHttpMessage : public IInternalMessage
        {
            public:
                std::string URL;
                std::string Body;
                std::string ContentType;
        };

        class CPermissionMessage : public IInternalMessage
        {
            public:
                Permission Perm;
                std::vector<PermissionOverwrites> Overwrites;
                CSnowflake GuildID;
        };

        class CChangeVoiceStateMessage : public IInternalMessage
        {
            public:
                CSnowflake GuildID;
        };

        using InternalMessage = std::shared_ptr<IInternalMessage>;
        using HttpMessage = std::shared_ptr<CHttpMessage>;
        using PermissionMessage = std::shared_ptr<CPermissionMessage>;
        using ChangeVoiceStateMessage = std::shared_ptr<CChangeVoiceStateMessage>;
    }
} // namespace DiscordBot


#endif //INTERNALMESSAGES_HPP