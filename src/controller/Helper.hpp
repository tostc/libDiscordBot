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

#ifndef HELPER_HPP
#define HELPER_HPP

#include <stdint.h>
#include <chrono>
#include <algorithm>

namespace DiscordBot
{
    /**
     * @brief Const adler32 implementation to use strings in switch cases.
     */
    const static int BASE = 65521;
    inline constexpr size_t Adler32(const char *Data)
    {
        size_t S1 = 1 & 0xFFFF;
        size_t S2 = (1 >> 16) & 0xFFFF;

        const char *Beg = Data;
        while (*Beg)
        {
            S1 = (S1 + *Beg) % BASE;
            S2 = (S2 + S1) % BASE;
            Beg++;
        }

        return (S2 << 16) + S1;
    }

    inline std::string ToLower(std::string Str)
    {
        std::transform(Str.begin(), Str.end(), Str.begin(), tolower);

        return Str; 
    }

    inline int64_t GetTimeMillis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    inline bool IsLittleEndian()
    {
        short t = 1;
        return ((char*)&t)[0];
    }

    inline short ChangeEndianess(short Val)
    {
        return ((Val << 8) & 0xFF00) | ((Val >> 8) & 0xFF);
    }

    inline int ChangeEndianess(int Val)
    {
        return ((Val << 24) & 0xFF000000) | ((Val >> 24) & 0xFF) | ((Val << 8) & 0x00FF0000) | ((Val >> 8) & 0xFF00);
    }

} // namespace DiscordBot


#endif //HELPER_HPP