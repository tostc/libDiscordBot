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

#ifndef EMBEDFACTORY_HPP
#define EMBEDFACTORY_HPP

#include "ISerializeFactory.hpp"
#include <DiscordBot/Models/Embed.hpp>

#include "../../controller/DiscordClient.hpp"

namespace DiscordBot
{
    class CEmbedFactory : public TSerializeFactory<CEmbed>
    {
        public:
            CEmbedFactory(CDiscordClient *client) : TSerializeFactory(client) {}

            std::string Serialize(Embed obj) override
            {
                CJSON js;

                js.AddPair("title", obj->Title.load());
                js.AddPair("description", obj->Description.load());

                if(!obj->URL->empty())
                    js.AddPair("url", obj->URL.load());

                if(!obj->Type->empty())
                    js.AddPair("type", obj->Type.load());

                return js.Serialize();
            }

            ~CEmbedFactory() {}
    };
} // namespace DiscordBot


#endif //EMBEDFACTORY_HPP