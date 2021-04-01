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

#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP

#include <controller/Factory.hpp>
#include "ISerializeFactory.hpp"
#include <JSON.hpp>
#include <string>
#include <typeindex>
#include <map>
#include <memory>

namespace DiscordBot
{
    class CDiscordClient;

    class CObjectFactory
    {
        public:
            CObjectFactory(CDiscordClient *client);

            template<class T>
            std::string Serialize(std::shared_ptr<T> Obj)
            {
                auto IT = m_Factories.find(typeid(T));
                return std::dynamic_pointer_cast<T>(IT->second->Create())->Serialize(Obj);
            }

            template<class T>
            std::shared_ptr<T> Deserialize(const std::string &JS)
            {
                CJSON json;
                json.ParseObject(JS);

                auto IT = m_Factories.find(typeid(T));
                return std::dynamic_pointer_cast<T>(IT->second->Create())->Deserialize(json);
            }

            ~CObjectFactory() = default;

        private:
            using Factory = std::shared_ptr<IFactory<ISerializeFactory>>;

            CDiscordClient *m_Client;
            std::map<std::type_index, Factory> m_Factories;
    };
} // namespace DiscordBot

#endif //OBJECTFACTORY_HPP
