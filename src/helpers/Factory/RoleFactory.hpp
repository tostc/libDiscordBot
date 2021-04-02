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

#ifndef ROLEFACTORY_HPP
#define ROLEFACTORY_HPP

#include "ISerializeFactory.hpp"
#include <models/Role.hpp>
#include "../../controller/DiscordClient.hpp"

namespace DiscordBot
{
    class CRoleFactory : public TSerializeFactory<CRole>
    {
        public:
            CRoleFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            Role Deserialize(CJSON &json) override
            {
                Role ret = Role(new CRole());

                ret->ID = json.GetValue<std::string>("id");
                ret->Name = json.GetValue<std::string>("name");
                ret->Color = json.GetValue<uint32_t>("color");
                ret->Hoist = json.GetValue<bool>("hoist");
                ret->Position = json.GetValue<int>("position");
                ret->Permissions = (Permission)json.GetValue<uint32_t>("permissions");
                ret->Managed = json.GetValue<bool>("managed");
                ret->Mentionable = json.GetValue<bool>("mentionable");

                return ret;
            }

            ~CRoleFactory() {}
    };
} // namespace DiscordBot


#endif //CROLEFACTORY_HPP