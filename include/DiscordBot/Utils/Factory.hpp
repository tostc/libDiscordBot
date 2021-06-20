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

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <memory>
#include <tuple>
#include <DiscordBot/config.h>

namespace DiscordBot
{
    template<class Base>
    class DISCORDBOT_EXPORT IFactory
    {
        public:
            IFactory() {}

            virtual std::shared_ptr<Base> Create() = 0;

            ~IFactory() {}
    };

    template<class Base, class Derive, class Tuple>
    class DISCORDBOT_EXPORT CFactory : public IFactory<Base>
    {
        public:
            CFactory(Tuple Params) : m_Params(Params) {}

            std::shared_ptr<Base> Create() override
            {
                return ImplCreate(typename gen<std::tuple_size<Tuple>::value>::Seq());
            }

            ~CFactory() {}
        private:
            template <int...>
            struct seq
            {
            };

            template <int N, int... S>
            struct gen : public gen<N - 1, N - 1, S...>
            {
            };

            template <int... S>
            struct gen<0, S...>
            {
                using Seq = seq<S...>;
            };

            template <int... S>
            std::shared_ptr<Base> ImplCreate(seq<S...>)
            {
                return std::shared_ptr<Base>(new Derive(std::get<S>(m_Params)...));
            }

            Tuple m_Params;
    };
} // namespace DiscordBot


#endif //FACTORY_HPP