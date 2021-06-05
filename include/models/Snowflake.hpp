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

#ifndef SNOWFLAKE_HPP
#define SNOWFLAKE_HPP

#include <iostream>
#include <config.h>
#include <memory>
#include <stddef.h>
#include <string>

namespace DiscordBot
{
    class CSnowflake
    {
        public:
            CSnowflake() : m_ID(0) {}

            CSnowflake(const char *ID) : CSnowflake(std::string(ID)) {}

            CSnowflake(const std::string &ID) : CSnowflake()
            {
                if(ID.empty())
                    return;
#ifdef BOT64
                m_ID = std::stoul(ID);
#else
                m_ID = std::stoull(ID);
#endif
            }

            CSnowflake(uint64_t ID) : m_ID(ID) {}

            inline bool IsValidID()
            {
                return m_ID != 0;
            }

            inline bool operator=(const CSnowflake &rhs)
            {
                return m_ID = rhs.m_ID;
            }

            inline bool operator!=(const CSnowflake &rhs)
            {
                return m_ID != rhs.m_ID;
            }

            inline bool operator==(const CSnowflake &rhs)
            {
                return m_ID == rhs.m_ID;
            }

            inline bool operator>(const CSnowflake &rhs)
            {
                return m_ID > rhs.m_ID;
            }

            inline bool operator<(const CSnowflake &rhs)
            {
                return m_ID < rhs.m_ID;
            }

            inline bool operator>=(const CSnowflake &rhs)
            {
                return m_ID >= rhs.m_ID;
            }

            inline bool operator<=(const CSnowflake &rhs)
            {
                return m_ID <= rhs.m_ID;
            }

            inline operator uint64_t() const
            {
                return m_ID;
            }

            explicit inline operator std::string() const
            {
                return std::to_string(m_ID);
            }

            ~CSnowflake() = default;
        private:
            uint64_t m_ID;
    };

    inline std::ostream &operator<<(std::ostream &of, const CSnowflake &rhs)
    {
        of << (uint64_t)rhs;
        return of;
    }
} // namespace DiscordBot

#endif //SNOWFLAKE_HPP