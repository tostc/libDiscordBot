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
#ifndef MESSAGEBASE_HPP
#define MESSAGEBASE_HPP

#include <stddef.h>
#include <stdint.h>
#include <memory>

namespace DiscordBot
{
    // Internal API don't use.
    namespace Internal
    {
        class IMessageBase
        {
            public:
                IMessageBase() : Event(0), Handled(false) {}

                size_t Event;           //!< User defined Messagetype.
                bool Handled;           //!< True if the message doesn't need to propagate.
                int Timeout;
                int64_t CreateddMs;

                virtual ~IMessageBase() {}
        };

        /**
         * @brief Contains a value for the receiver.
         */
        template <class T>
        class TMessage : public IMessageBase
        {
            public:
                TMessage() : IMessageBase() {}

                T Value;
        };

        using MessageBase = std::shared_ptr<IMessageBase>;
    }
} // namespace DiscordBot

#endif //MESSAGEBASE_HPP