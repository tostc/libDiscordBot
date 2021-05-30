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

#ifndef ACTIVITYFACTORY_HPP
#define ACTIVITYFACTORY_HPP

#include "../../controller/DiscordClient.hpp"
#include "ISerializeFactory.hpp"
#include <models/Activity.hpp>

namespace DiscordBot
{
    class CActivityFactory : public TSerializeFactory<CActivity>
    {
        public:
            CActivityFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            std::string Serialize(std::shared_ptr<CActivity> Obj) override
            {

            }

            Activity Deserialize(CJSON &json) override
            {
                Activity ret = Activity(new CActivity());

                ret->Name = json.GetValue<std::string>("name");
                ret->Type = (ActivityType)json.GetValue<int>("type");
                ret->URL = json.GetValue<std::string>("url");
                ret->CreatedAt = json.GetValue<int>("created_at");

                CJSON Timestamps;
                Timestamps.ParseObject(json.GetValue<std::string>("timestamps"));
                ret->StartTime = Timestamps.GetValue<int>("start");
                ret->EndTime = Timestamps.GetValue<int>("end");

                ret->AppID = json.GetValue<std::string>("application_id");
                ret->Details = json.GetValue<std::string>("details");

                ret->State = json.GetValue<std::string>("state");

                if(!json.GetValue<std::string>("party").empty())
                {
                    CJSON JParty;
                    JParty.ParseObject(json.GetValue<std::string>("party"));

                    ret->PartyObject = Party(new CParty());
                    ret->PartyObject->ID = JParty.GetValue<std::string>("id");
                    ret->PartyObject->Size = JParty.GetValue<std::vector<int>>("size");
                }

                if(!json.GetValue<std::string>("secrets").empty())
                {
                    CJSON JSecret;
                    JSecret.ParseObject(json.GetValue<std::string>("secrets"));

                    ret->Secret = Secrets(new CSecrets());
                    ret->Secret->Join = JSecret.GetValue<std::string>("join");
                    ret->Secret->Spectate = JSecret.GetValue<std::string>("spectate");
                    ret->Secret->Match = JSecret.GetValue<std::string>("match");
                }

                ret->Instance = json.GetValue<bool>("instance");
                ret->Flags = (ActivityFlags)json.GetValue<int>("flags");

                return ret;
            }

            ~CActivityFactory() = default;
    };
} // namespace DiscordBot


#endif //ACTIVITYFACTORY_HPP