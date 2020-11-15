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

#ifndef MODIFYMEMBER_HPP
#define MODIFYMEMBER_HPP

#include <models/User.hpp>
#include <models/Role.hpp>
#include <models/Channel.hpp>

namespace DiscordBot
{
    enum class Modifications
    {
        NONE = 0,
        NICK = 1,
        ROLES = 2,
        DEAF = 4,
        MUTE = 8,
        MOVE = 16
    };

    inline Modifications operator |(Modifications lhs, Modifications rhs)  
    {
        return static_cast<Modifications>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }

    inline Modifications operator &(Modifications lhs, Modifications rhs)  
    {
        return static_cast<Modifications>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
    }

    class CModifyMember
    {
        public:
            CModifyMember() : m_Mods(Modifications::NONE) {}
            CModifyMember(User userRef) : CModifyMember() 
            {
                m_UserRef = userRef;
            }

            inline User GetUserRef() const
            {
                return m_UserRef;
            }
            
            /**
             * @brief Sets the user to modify.
             */
            inline void SetUserRef(User UserRef)
            {
                m_UserRef = UserRef;
            }

            inline std::string GetNick() const
            {
                return m_Nick;
            }
            
            /**
             * @brief Sets the new nickname of a user.
             * 
             * @attention The bot needs following permission `MANAGE_NICKNAMES`
             */
            inline void SetNick(std::string Nick)
            {
                m_Nick = Nick;
                m_Mods = m_Mods | Modifications::NICK;
            }

            inline std::vector<Role> GetRoles() const
            {
                return m_Roles;
            }
            
            /**
             * @brief Sets the roles of a user.
             * 
             * @attention The bot needs following permission `MANAGE_ROLES`
             */
            inline void SetRoles(std::vector<Role> Roles)
            {
                m_Roles = Roles;
                m_Mods = m_Mods | Modifications::ROLES;
            }

            inline bool IsDeafed() const
            {
                return m_Deaf;
            }
            
            /**
             * @brief Deafs a member in a voice channel.
             * 
             * @attention The bot needs following permission `DEAFEN_MEMBERS`
             */
            inline void SetDeaf(bool Deaf)
            {
                m_Deaf = Deaf;
                m_Mods = m_Mods | Modifications::DEAF;
            }

            inline bool IsMuted() const
            {
                return m_Mute;
            }
            
            /**
             * @brief Mutes a member in a voice channel.
             * 
             * @attention The bot needs following permission `MUTE_MEMBERS`
             */
            inline void SetMute(bool Mute)
            {
                m_Mute = Mute;
                m_Mods = m_Mods | Modifications::MUTE;
            }

            inline Channel GetChannel() const
            {
                return m_Channel;
            }
            
            /**
             * @brief Moves a member to a channel.
             * 
             * @note If c is null the user will be kicked from the voice channel.
             * 
             * @attention The bot needs following permission `MOVE_MEMBERS`
             */
            inline void SetChannel(Channel c)
            {
                m_Channel = c;
                m_Mods = m_Mods | Modifications::MOVE;
            }

            inline Modifications GetMods() const
            {
                return m_Mods;
            }

            ~CModifyMember() {}
        private:
            Modifications m_Mods;

            User m_UserRef;
            std::string m_Nick;
            std::vector<Role> m_Roles;
            bool m_Deaf;
            bool m_Mute;
            Channel m_Channel;

    };
} // namespace DiscordBot


#endif //MODIFYMEMBER_HPP