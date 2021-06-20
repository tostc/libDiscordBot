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

#ifndef DISCORDBOT_HPP
#define DISCORDBOT_HPP

// Channels
#include <DiscordBot/Channels/IChannel.hpp>
#include <DiscordBot/Channels/TextChannel.hpp>
#include <DiscordBot/Channels/VoiceChannel.hpp>

// Commands
#include <DiscordBot/Commands/ICommand.hpp>
#include <DiscordBot/Commands/ICommandsConfig.hpp>
#include <DiscordBot/Commands/IController.hpp>

// Exceptions
#include <DiscordBot/Exceptions/DiscordException.hpp>
#include <DiscordBot/Exceptions/PermissionException.hpp>

// Messages
#include <DiscordBot/Messages/MessageBase.hpp>
#include <DiscordBot/Messages/MessageManager.hpp>
#include <DiscordBot/Messages/MessageResult.hpp>

// Models
#include <DiscordBot/Models/Action.hpp>
#include <DiscordBot/Models/Activity.hpp>
#include <DiscordBot/Models/DiscordEnums.hpp>
#include <DiscordBot/Models/DiscordObject.hpp>
#include <DiscordBot/Models/Embed.hpp>
#include <DiscordBot/Models/File.hpp>
#include <DiscordBot/Models/Guild.hpp>
#include <DiscordBot/Models/GuildMember.hpp>
#include <DiscordBot/Models/Message.hpp>
#include <DiscordBot/Models/PermissionOverwrites.hpp>
#include <DiscordBot/Models/Role.hpp>
#include <DiscordBot/Models/User.hpp>

// Properties
#include <DiscordBot/Properties/ChannelProperties.hpp>
#include <DiscordBot/Properties/GuildMemberProperties.hpp>

// Utils
#include <DiscordBot/Utils/Atomic.hpp>
#include <DiscordBot/Utils/Factory.hpp>
#include <DiscordBot/Utils/Snowflake.hpp>

// Voice
#include <DiscordBot/Voice/IAudioSource.hpp>
#include <DiscordBot/Voice/IMusicQueue.hpp>
#include <DiscordBot/Voice/IVoiceSocket.hpp>
#include <DiscordBot/Voice/SongInfo.hpp>
#include <DiscordBot/Voice/VoiceClient.hpp>
#include <DiscordBot/Voice/VoiceState.hpp>

#include <DiscordBot/config.h>
#include <DiscordBot/IDiscordClient.hpp>

#endif //DISCORDBOT_HPP