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

#include <Log.hpp>
#include <JSON.hpp>
#include <models/User.hpp>
#include <models/channels/IChannel.hpp>
#include "../controller/DiscordClient.hpp"
#include "../helpers/Factory/ObjectFactory.hpp"

namespace DiscordBot
{
    Channel CUser::CreateDM()
    {
        CJSON json;
        json.AddPair("recipient_id", (std::string)ID);

        auto res = dynamic_cast<CDiscordClient*>(m_Client)->Post("/users/@me/channels", json.Serialize());
        if (res->statusCode != 200)
            llog << lerror << "Failed to send message HTTP: " << res->statusCode << " MSG: " << res->errorMsg << lendl;
        else
        {
            // TODO: Broken
            //return CObjectFactory::Deserialize<IChannel>(m_Client, res->body);
        }

        return nullptr;
    }
} // namespace DiscordBot