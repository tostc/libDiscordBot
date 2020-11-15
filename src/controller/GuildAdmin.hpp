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

#ifndef GUILDADMIN_HPP
#define GUILDADMIN_HPP

#include <controller/IGuildAdmin.hpp>

namespace DiscordBot
{
    class CDiscordClient;

    class CGuildAdmin : public IGuildAdmin
    {
        public:
            CGuildAdmin(CDiscordClient *client, Guild guild) : m_Client(client), m_Guild(guild) {}

            void ModifyMember(const CModifyMember &mod) override;
            void BanMember(User member, const std::string &Reason = "", int DeleteMsgDays = -1) override;
            void UnbanMember(User user) override;
            std::vector<std::pair<std::string, User>> GetGuildBans() override;
            void KickMember(User member) override;

            ~CGuildAdmin() {}

        private:
            GuildMember CheckBotPermissions(Permission p, const std::string &errMsg);

            /**
             * @brief Checks if a member has a given right.
             */
            bool HasPermission(GuildMember member, Permission perm);
            void RenameSelf(const std::string &js);

            CDiscordClient *m_Client;
            Guild m_Guild;
    };
} // namespace DiscordBot


#endif //GUILDADMIN_HPP