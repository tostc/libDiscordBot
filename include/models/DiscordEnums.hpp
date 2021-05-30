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

#ifndef DISCORDENUMS_HPP
#define DISCORDENUMS_HPP

namespace DiscordBot
{
    //Discord Gateway intents https://discordapp.com/developers/docs/topics/gateway#gateway-intents
    enum class Intent
    {
        GUILDS = (1 << 0),
        //- GUILD_CREATE
        //- GUILD_UPDATE
        //- GUILD_DELETE
        //- GUILD_ROLE_CREATE
        //- GUILD_ROLE_UPDATE
        //- GUILD_ROLE_DELETE
        //- CHANNEL_CREATE
        //- CHANNEL_UPDATE
        //- CHANNEL_DELETE
        //- CHANNEL_PINS_UPDATE

        GUILD_MEMBERS = (1 << 1),           //!< Please visit <a href="https://discord.com/developers/docs/topics/gateway#privileged-intents">this</a> website to use this intent.
        //- GUILD_MEMBER_ADD
        //- GUILD_MEMBER_UPDATE
        //- GUILD_MEMBER_REMOVE

        GUILD_BANS = (1 << 2),
        //- GUILD_BAN_ADD
        //- GUILD_BAN_REMOVE

        GUILD_EMOJIS = (1 << 3),
        //- GUILD_EMOJIS_UPDATE

        GUILD_INTEGRATIONS = (1 << 4),
        //- GUILD_INTEGRATIONS_UPDATE

        GUILD_WEBHOOKS = (1 << 5),
        //- WEBHOOKS_UPDATE

        GUILD_INVITES = (1 << 6),
        //- INVITE_CREATE
        //- INVITE_DELETE

        GUILD_VOICE_STATES = (1 << 7),
        //- VOICE_STATE_UPDATE

        GUILD_PRESENCES = (1 << 8),         //!< Please visit <a href="https://discord.com/developers/docs/topics/gateway#privileged-intents">this</a> website to use this intent.
        //- PRESENCE_UPDATE

        GUILD_MESSAGES = (1 << 9),
        //- MESSAGE_CREATE
        //- MESSAGE_UPDATE
        //- MESSAGE_DELETE
        //- MESSAGE_DELETE_BULK

        GUILD_MESSAGE_REACTIONS = (1 << 10),
        //- MESSAGE_REACTION_ADD
        //- MESSAGE_REACTION_REMOVE
        //- MESSAGE_REACTION_REMOVE_ALL
        //- MESSAGE_REACTION_REMOVE_EMOJI

        GUILD_MESSAGE_TYPING = (1 << 11),
        //- TYPING_START

        DIRECT_MESSAGES = (1 << 12),
        //- CHANNEL_CREATE
        //- MESSAGE_CREATE
        //- MESSAGE_UPDATE
        //- MESSAGE_DELETE
        //- CHANNEL_PINS_UPDATE

        DIRECT_MESSAGE_REACTIONS = (1 << 13),
        //- MESSAGE_REACTION_ADD
        //- MESSAGE_REACTION_REMOVE
        //- MESSAGE_REACTION_REMOVE_ALL
        //- MESSAGE_REACTION_REMOVE_EMOJI

        DIRECT_MESSAGE_TYPING = (1 << 14),
        //- TYPING_START

        DEFAULTS = GUILDS | GUILD_VOICE_STATES | GUILD_MESSAGES | DIRECT_MESSAGES | GUILD_MEMBERS
    };

    enum class ActivityType
    {
        GAME,
        STREAMING,
        LISTENING,
        CUSTOM
    };

    enum class ActivityFlags
    {
        INSTANCE = 1 << 0,
        JOIN = 1 << 1,
        SPECTATE = 1 << 2,
        JOIN_REQUEST = 1 << 3,
        SYNC = 1 << 4,
        PLAY = 1 << 5
    };
    
    enum class ChannelTypes
    {
        GUILD_TEXT,
        DM,
        GUILD_VOICE,
        GROUP_DM,
        GUILD_CATEGORY,
        GUILD_NEWS,
        GUILD_STORE
    };

    enum class MentionTypes
    {
        USER,
        NICKNAME,
        CHANNEL,
        ROLE,
        EMOJI,
        CUSTOM_EMOJI,
        CUSTOM_ANIM_EMOJI
    };

    /** Online state of the bot. */
    enum class OnlineState
    {
        ONLINE,
        DND,
        IDLE,
        INVISIBLE,
        OFFLINE
    };

/**
     * @brief Permission bits for different roles. For more info see <a href="https://discord.com/developers/docs/topics/permissions#permissions-bitwise-permission-flags">here</a>.
     */
    enum class Permission
    {
        //Permission            Value	           Description                         Channel Type
        CREATE_INSTANT_INVITE   = 0x00000001, //!< Allows creation of instant invites, T, V
        KICK_MEMBERS            = 0x00000002, //!< Allows kicking members
        BAN_MEMBERS             = 0x00000004, //!< Allows banning members
        ADMINISTRATOR           = 0x00000008, //!< Allows all permissions and bypasses channel permission overwrites
        MANAGE_CHANNELS         = 0x00000010, //!< Allows management and editing of channels T, V
        MANAGE_GUILD            = 0x00000020, //!< Allows management and editing of the guild
        ADD_REACTIONS           = 0x00000040, //!< Allows for the addition of reactions to messages T
        VIEW_AUDIT_LOG          = 0x00000080, //!< Allows for viewing of audit logs
        PRIORITY_SPEAKER        = 0x00000100, //!< Allows for using priority speaker in a voice channel V
        STREAM                  = 0x00000200, //!< Allows the user to go live V
        VIEW_CHANNEL            = 0x00000400, //!< Allows guild members to view a channel, which includes reading messages in text channels T, V
        SEND_MESSAGES           = 0x00000800, //!< Allows for sending messages in a channel T
        SEND_TTS_MESSAGES       = 0x00001000, //!< Allows for sending of /tts messages T
        MANAGE_MESSAGES         = 0x00002000, //!< Allows for deletion of other users messages T
        EMBED_LINKS             = 0x00004000, //!< Links sent by users with this permission will be auto-embedded T
        ATTACH_FILES            = 0x00008000, //!< Allows for uploading images and files T
        READ_MESSAGE_HISTORY    = 0x00010000, //!< Allows for reading of message history T
        MENTION_EVERYONE        = 0x00020000, //!< Allows for using the @everyone tag to notify all users in a channel, and the @here tag to notify all online users in a channel T
        USE_EXTERNAL_EMOJIS     = 0x00040000, //!< Allows the usage of custom emojis from other servers T
        VIEW_GUILD_INSIGHTS     = 0x00080000, //!< Allows for viewing guild insights
        CONNECT                 = 0x00100000, //!< Allows for joining of a voice channel V
        SPEAK                   = 0x00200000, //!< Allows for speaking in a voice channel V
        MUTE_MEMBERS            = 0x00400000, //!< Allows for muting members in a voice channel V
        DEAFEN_MEMBERS          = 0x00800000, //!< Allows for deafening of members in a voice channel V
        MOVE_MEMBERS            = 0x01000000, //!< Allows for moving of members between voice channels V
        USE_VAD                 = 0x02000000, //!< Allows for using voice-activity-detection in a voice channel V
        CHANGE_NICKNAME         = 0x04000000, //!< Allows for modification of own nickname
        MANAGE_NICKNAMES        = 0x08000000, //!< Allows for modification of other users nicknames
        MANAGE_ROLES            = 0x10000000, //!< Allows management and editing of roles T, V
        MANAGE_WEBHOOKS         = 0x20000000, //!< Allows management and editing of webhooks T, V
        MANAGE_EMOJIS           = 0x40000000 //!< Allows management and editing of emojis 
    };

    enum class UserFlags
    {
        NONE = 0,
        DISCORD_EMPLOYEE = (1 << 0),
        DISCORD_PARTNER = (1 << 1),
        HYPESQUAD_EVENTS = (1 << 2),
        BUG_HUNTER_LEVEL_1 = (1 << 3),
        HOUSE_BRAVERY = (1 << 6),
        HOUSE_BRILLIANCE = (1 << 7),
        HOUSE_BALANCE = (1 << 8),
        EARLY_SUPPORTER = (1 << 9),
        TEAM_USER = (1 << 10),
        SYSTEM = (1 << 12),
        BUG_HUNTER_LEVEL_2 = (1 << 14),
        VERIFIED_BOT = (1 << 16),
        VERIFIED_BOT_DEVELOPER = (1 << 17)
    };

    enum class PremiumTypes
    {
        NONE = 0,
        NITRO_CLASSIC = 1,
        NITRO = 2
    };

    inline Permission operator| (Permission lhs, Permission rhs)  
    {
        return static_cast<Permission>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
    }  

    inline Permission operator& (Permission lhs, Permission rhs)  
    {
        return static_cast<Permission>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
    }

    inline Permission& operator|= (Permission &lhs, Permission rhs)  
    {
        return lhs = static_cast<Permission>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
    }  

    inline Permission& operator&= (Permission &lhs, Permission rhs)  
    {
        return lhs = static_cast<Permission>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
    }

    inline Permission operator~ (const Permission p)  
    {
        return static_cast<Permission>(~static_cast<unsigned>(p));
    }

    inline ActivityFlags operator| (ActivityFlags lhs, ActivityFlags rhs)  
    {
        return static_cast<ActivityFlags>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }
} // namespace DiscordBot

#endif //DISCORDENUMS_HPP