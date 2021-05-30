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

#ifndef GUILDMEMBERFACTORY_HPP
#define GUILDMEMBERFACTORY_HPP

#include "../../controller/DiscordClient.hpp"
#include "ISerializeFactory.hpp"
#include <models/GuildMember.hpp>
#include <string>
#include <vector>

namespace DiscordBot
{
    class CGuildMemberFactory : public TSerializeFactory<CGuildMember>
    {
        public:
            CGuildMemberFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            GuildMember Deserialize(CJSON &JS) override
            {
                GuildMember Ret = GuildMember(new CGuildMember(m_Client));
                std::string UserInfo = JS.GetValue<std::string>("user");

                User member;
                Guild guild = m_Client->GetGuild(JS.GetValue<std::string>("guild_id"));

                //Gets the user which is associated with the member.
                if (!UserInfo.empty())
                    member = m_Client->GetUserOrAdd(UserInfo);

                Ret->GuildID = guild->ID;
                Ret->UserRef = member;
                Ret->Nick = JS.GetValue<std::string>("nick");
                Ret->JoinedAt = JS.GetValue<std::string>("joined_at");
                Ret->PremiumSince = JS.GetValue<std::string>("premium_since");
                Ret->Deaf = JS.GetValue<bool>("deaf");
                Ret->Mute = JS.GetValue<bool>("mute");

                //Adds the roles
                auto Array = JS.GetValue<std::vector<std::string>>("roles");
                for (auto &&e : Array)
                {
                    auto RIT = guild->Roles->find(e);
                    if(RIT != guild->Roles->end())
                        Ret->Roles->push_back(RIT->second);
                }

                if (Ret->UserRef)
                    guild->Members->insert({Ret->UserRef->ID, Ret});

                return Ret;
            }

            ~CGuildMemberFactory() = default;
    };
} // namespace DiscordBot

#endif //GUILDMEMBERFACTORY_HPP