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
#include <models/Message.hpp>
#include <mutex>
#include <map>

namespace DiscordBot
{
    class CDiscordClient;

    class CGuildAdmin : public IGuildAdmin
    {
        public:
            CGuildAdmin(CDiscordClient *client, Guild guild) : m_Client(client), m_Guild(guild) {}

            void AddChannelAction(Channel channel, Action action) override;
            void RemoveChannelAction(Channel channel, ActionType types) override;

            // Internal events for the actions.
            void OnUserVoiceStateChanged(Channel c, GuildMember m);
            void OnMessageEvent(ActionType Type, Channel c, Message m);

            ~CGuildAdmin() {}

        private:
            template<class T>
            void FireAction(ActionType Type, Action a, Channel c, T val)
            {
                auto action = std::dynamic_pointer_cast<CAction<T>>(a);

                if(action->Filter(Type, c, val))
                    action->FireAction(Type, c, val);
            }

            std::mutex m_Lock;

            CDiscordClient *m_Client;
            Guild m_Guild;
            std::map<std::string, std::map<ActionType, Action>> m_Actions;
    };
} // namespace DiscordBot


#endif //GUILDADMIN_HPP