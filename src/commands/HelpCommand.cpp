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

#include "HelpCommand.hpp"
#include <DiscordBot/IDiscordClient.hpp>

namespace DiscordBot
{
    CHelpCommand::CHelpCommand(IController *controller, IDiscordClient *client) : m_Controller(controller), m_Client(client)
    {
        RegisterCommandHandler("h", std::bind(&CHelpCommand::Help, this, std::placeholders::_1));
        RegisterCommandHandler("help", std::bind(&CHelpCommand::Help, this, std::placeholders::_1));
    }

    void CHelpCommand::Help(CommandContext ctx)
    {
        std::string Dialog;
        const int BufferSize = 200;
        char Buf[BufferSize];

        auto Cmds = m_Controller->GetCommands(ctx->Msg->GuildRef, ctx->Msg->Member);
        for (auto &&e : Cmds)
        {
            int Size = snprintf(Buf, BufferSize, "%s%-20s%2s-%2s%s", m_Controller->GetPrefix(ctx->Msg->GuildRef).c_str(), e.Cmd.c_str(), "", "", e.Description.c_str());
            Dialog += std::string(Buf, Buf + Size) + '\n';
        }

        ctx->Msg->ChannelRef->SendMessage(Dialog);
    }
} // namespace DiscordBot