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

#ifndef USER_HPP
#define USER_HPP

#include <memory>
#include <string>

namespace DiscordBot
{
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

    class CUser
    {
        public:
            CUser(/* args */) : Bot(false), System(false), MFAEnabled(false), Verified(false) {}

            std::string ID;
            std::string Username;
            std::string Discriminator;
            std::string Avatar;
            bool Bot;
            bool System;
            bool MFAEnabled;
            std::string Locale;
            bool Verified;
            std::string Email;
            UserFlags Flags;
            PremiumTypes PremiumType;
            UserFlags PublicFlags;

            ~CUser() {}

        private:
    };

    using User = std::shared_ptr<CUser>;
}

#endif //USER_HPP