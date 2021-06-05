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
#include <vector>
#include <models/Activity.hpp>
#include <atomic>
#include <models/atomic.hpp>
#include <models/DiscordEnums.hpp>
#include <models/DiscordObject.hpp>

namespace DiscordBot
{
    class IChannel;
    using Channel = std::shared_ptr<IChannel>;

    class CDiscordClient;
    class CUser : public CDiscordObject
    {
        public:
            CUser(IDiscordClient *client, Internal::CMessageManager *MsgMgr) : CDiscordObject(client, MsgMgr), Bot(false), System(false), MFAEnabled(false), Verified(false) {}

            atomic<std::string> Username;
            atomic<std::string> Discriminator;
            atomic<std::string> Avatar;
            std::atomic<bool> Bot;
            std::atomic<bool> System;
            std::atomic<bool> MFAEnabled;
            atomic<std::string> Locale;
            std::atomic<bool> Verified;
            atomic<std::string> Email;
            UserFlags Flags;
            PremiumTypes PremiumType;
            UserFlags PublicFlags;

            Activity Game;
            OnlineState State;
            OnlineState Desktop;
            OnlineState Mobile;
            OnlineState Web;
            atomic<std::vector<Activity>> Activities;

            Channel CreateDM();

            ~CUser() {}

        private:
    };

    using User = std::shared_ptr<CUser>;
}

#endif //USER_HPP