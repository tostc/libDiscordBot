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

#ifndef MODIFYCHANNEL_HPP
#define MODIFYCHANNEL_HPP

#include <map>
#include <models/Channel.hpp>

namespace DiscordBot
{
    class CModifyChannel
    {
        public:
            CModifyChannel() : m_OverwritesSet(false) {}
            CModifyChannel(Channel ChannelRef) : CModifyChannel()
            {
                m_ChannelRef = ChannelRef;
            }

            inline void SetChannelRef(Channel ChannelRef)
            {
                m_ChannelRef = ChannelRef;
            }

            inline Channel GetChannelRef() const
            {
                return m_ChannelRef;
            }

            inline void SetName(std::string Name)
            {
                m_Values["name"] = Name;
            }

            inline void SetType(ChannelTypes Type)
            {
                m_Values["type"] = std::to_string((int)Type);
            }

            inline void SetPosition(int Position)
            {
                m_Values["position"] = std::to_string(Position);
            }

            inline void SetTopic(std::string Topic)
            {
                m_Values["topic"] = Topic;
            }

            inline void SetNSFW(bool NSFW)
            {
                m_Values["nsfw"] = NSFW ? "true" : "false";
            }

            inline void SetRateLimitUser(int RateLimitUser)
            {
                m_Values["rate_limit_per_user"] = std::to_string(RateLimitUser);
            }

            inline void SetBitrate(int Bitrate)
            {
                m_Values["bitrate"] = std::to_string(Bitrate);
            }

            inline void SetUserLimit(int UserLimit)
            {
                m_Values["user_limit"] = std::to_string(UserLimit);
            }

            inline void SetPermOverwrites(std::vector<PermissionOverwrites> PermOverwrites)
            {
                m_Overwrites = PermOverwrites;
                m_OverwritesSet = true;
            }

            inline std::vector<PermissionOverwrites> GetOverwrites() const
            {
                return m_Overwrites;
            }

            inline bool HasOverwrites() const
            {
                return m_OverwritesSet;
            }

            inline void SetCategorie(Channel Categorie)
            {
                m_Values["parent_id"] = Categorie->ID;
            }

            inline std::map<std::string, std::string> GetValues() const
            {
                return m_Values;
            }

            ~CModifyChannel() {}
        private:
            Channel m_ChannelRef;
            std::map<std::string, std::string> m_Values;
            std::vector<PermissionOverwrites> m_Overwrites;
            bool m_OverwritesSet;
    };
} // namespace DiscordBot


#endif //MODIFYCHANNEL_HPP