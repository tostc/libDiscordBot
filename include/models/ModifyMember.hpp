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
    class CModifyMember
    {
        public:
            CModifyMember() : m_HasRoles(false) {}
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
            
            /**
             * @brief Sets the new nickname of a user.
             * 
             * @attention The bot needs following permission `MANAGE_NICKNAMES`
             */
            inline void SetNick(std::string Nick)
            {
                m_Values["nick"] = Nick;
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
                m_HasRoles = true;
            }
            
            /**
             * @brief Deafs a member in a voice channel.
             * 
             * @attention The bot needs following permission `DEAFEN_MEMBERS`
             */
            inline void SetDeaf(bool Deaf)
            {
                m_Values["deaf"] = Deaf ? "true" : "false";
            }
            
            /**
             * @brief Mutes a member in a voice channel.
             * 
             * @attention The bot needs following permission `MUTE_MEMBERS`
             */
            inline void SetMute(bool Mute)
            {
                m_Values["mute"] = Mute ? "true" : "false";
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
                m_Values["channel_id"] = c ? c->ID.load() : "null";
            }

            inline std::map<std::string, std::string> GetValues() const
            {
                return m_Values;
            }

            inline bool HasRoles() const
            {
                return m_HasRoles;
            }

            ~CModifyMember() {}
        private:
            User m_UserRef;
            std::map<std::string, std::string> m_Values;

            std::vector<Role> m_Roles;
            bool m_HasRoles;

    };
} // namespace DiscordBot


#endif //MODIFYMEMBER_HPP