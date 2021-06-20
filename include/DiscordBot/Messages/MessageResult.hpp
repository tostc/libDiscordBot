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

#ifndef MESSAGERESULT_HPP
#define MESSAGERESULT_HPP

#include <DiscordBot/Exceptions/DiscordException.hpp>
#include <condition_variable>
#include <memory>
#include <string>

namespace DiscordBot
{
    // Internal API don't use.
    namespace Internal
    {
        class CMessageResult
        {
            public:
                CMessageResult() = default;

                /**
                 * @throw CDiscordClientException On error.
                 * 
                 * @return Returns the value of this result.
                 */
                template<class T>
                T Value()
                {
                    std::unique_lock<std::mutex> lk(m_Lock);
                    m_Notifier.wait(lk);

                    // Throws a CDiscordClientException on error.
                    m_Result->ThrowOnError();
                    return std::static_pointer_cast<SResult<T>>(m_Result)->Value;
                }

                /**
                 * @brief Sets the value of the result.
                 */
                template<class T>
                void Value(T val)
                {
                    std::unique_lock<std::mutex> lk(m_Lock);
                    m_Result = std::shared_ptr<IResult>(new SResult<T>(val));

                    lk.unlock();
                    m_Notifier.notify_one();
                }

                /**
                 * @brief Sets the error of this result.
                 */
                void Error(const std::string &err)
                {
                    std::unique_lock<std::mutex> lk(m_Lock);
                    lk.lock();

                    m_Result = std::shared_ptr<IResult>(new IResult());
                    m_Result->Error = err;

                    lk.unlock();
                    m_Notifier.notify_one();
                }

                ~CMessageResult() = default;
            private:
                struct IResult
                {
                    public:
                        void ThrowOnError()
                        {
                            if(!Error.empty())
                                throw CDiscordClientException(Error);
                        }

                        std::string Error;
                };

                template<class T>
                struct SResult : public IResult
                {
                    public:
                        SResult(T val) : Value(val) { }

                        T Value;
                };
                
                std::mutex m_Lock;
                std::condition_variable m_Notifier;

                std::shared_ptr<IResult> m_Result;
        };

        using MessageResult = std::shared_ptr<CMessageResult>;
    }
} // namespace DiscordBot


#endif //MESSAGERESULT_HPP