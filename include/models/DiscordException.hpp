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

#ifndef DISCORDEXCEPTION_HPP
#define DISCORDEXCEPTION_HPP

#include <string.h>
#include <stdexcept>

namespace DiscordBot 
{
    enum class DiscordClientErrorType
    {
        MISSING_PERMISSION,     //!< Throws if your bot doesn't have the permission to execute an action inside a guild.
        HTTP_ERROR,             //!< There was an error with the request.
        MEMBER_NOT_IN_VC,       //!< The user isn't in a voice channel.
        PARAMETER_IS_NULL,      //!< Throws if a required parameter is a nullptr.
        MISSING_USER_REF,       //!< Throws if a user reference is null.
    };

    class CDiscordClientException : public std::exception
    {
        public:
            CDiscordClientException() {}
            CDiscordClientException(DiscordClientErrorType Type) : m_ErrType(Type) {}
            CDiscordClientException(const std::string &Msg, DiscordClientErrorType Type) : m_Msg(Msg), m_ErrType(Type) {}

            const char *what() const noexcept override
            {
                return m_Msg.c_str();
            }

            DiscordClientErrorType ErrType() const noexcept
            {
                return m_ErrType;
            }

        private:
            std::string m_Msg;
            DiscordClientErrorType m_ErrType;
    };
}

#endif //DISCORDEXCEPTION_HPP