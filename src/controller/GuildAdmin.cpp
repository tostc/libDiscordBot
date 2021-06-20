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

#include "GuildAdmin.hpp"
#include "DiscordClient.hpp"
#include <DiscordBot/Exceptions/DiscordException.hpp>
#include <DiscordBot/Exceptions/PermissionException.hpp>
#include <vector>
#include "../helpers/Helper.hpp"

namespace DiscordBot
{
    void CGuildAdmin::AddChannelAction(Channel channel, Action action)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::string ID;
        if(channel)
            ID = channel->ID;

        ActionType Types = action->GetTypes();
        for (uint32_t i = 1; i < (uint32_t)ActionType::TOTAL_ACTIONS; i <<= 1)
        {
            ActionType Type = Types & (ActionType)i;
            if(Type != ActionType::NONE)
            {
                auto IT = m_Actions.find(ID);
                if(IT == m_Actions.end())
                    m_Actions.insert({ID, {{Type, action}}});
                else
                {
                    auto InnerIT = IT->second.find(Type);
                    if(InnerIT != IT->second.end())
                        throw CDiscordClientException("Action is already registered!", DiscordClientErrorType::ACTION_ALREADY_REG);

                    IT->second.insert({Type, action});
                }
            }
        }
    }

    void CGuildAdmin::RemoveChannelAction(Channel channel, ActionType types) 
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        std::string ID;
        if(channel)
            ID = channel->ID;

        for (uint32_t i = 1; i < (uint32_t)ActionType::TOTAL_ACTIONS; i <<= 1)
        {
            ActionType Type = types & (ActionType)i;
            auto IT = m_Actions.find(ID);
            if(IT != m_Actions.end())
                IT->second.erase(Type);
        }
    }

    void CGuildAdmin::OnUserVoiceStateChanged(Channel c, GuildMember m)
    {
        // std::lock_guard<std::mutex> lock(m_Lock);
        // std::vector<std::string> IDs = {
        //     "",
        //     c->ID
        // };

        // for (auto ID : IDs)
        // {
        //     auto IT = m_Actions.find(ID);
        //     if(IT != m_Actions.end())
        //     {
        //         ActionType Type = ActionType::NONE;
        //         if(m->State)
        //             Type = ActionType::USER_JOIN;
        //         else
        //             Type = ActionType::USER_LEAVE;

        //         auto InnerIT = IT->second.find(Type);
        //         if(InnerIT != IT->second.end())
        //             FireAction(Type, InnerIT->second, c, m);
        //     }
        // }
    }

    void CGuildAdmin::OnMessageEvent(ActionType Type, Channel c, Message m)
    {
        // std::lock_guard<std::mutex> lock(m_Lock);
        // std::vector<std::string> IDs = {
        //     "",
        //     c->ID
        // };

        // for (auto ID : IDs)
        // {
        //     auto IT = m_Actions.find(ID);
        //     if(IT != m_Actions.end())
        //     {
        //         auto InnerIT = IT->second.find(Type);
        //         if(InnerIT != IT->second.end())
        //             FireAction(Type, InnerIT->second, c, m);
        //     }
        // }
    }
} // namespace DiscordBot
