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

#include <models/Message.hpp>

namespace DiscordBot
{
    std::vector<SMention> CMessage::ParseParam(const std::string &Param)
    {
        std::vector<SMention> Ret;
        std::string ID;

        const char *beg = Param.c_str();
        const char *end = beg + Param.size();

        bool BeginTagFound = false;
        MentionTypes Type;

        while (beg != end)
        {
            switch (*beg)
            {
                case '<':
                {
                    BeginTagFound = true;
                }break;

                case '>':
                {
                    if(BeginTagFound)
                        Ret.push_back({Type, ID});

                    BeginTagFound = false;
                }break;

                default:
                {
                    if(BeginTagFound)
                        ID += beg;
                }break;
            }

            beg++;
        }

        return Ret;
    }
} // namespace DiscordBot