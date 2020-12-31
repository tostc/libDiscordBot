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

#ifndef MESSAGEMANAGER_HPP
#define MESSAGEMANAGER_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <map>
#include <functional>
#include <atomic>
#include <memory>
#include "../helpers/Helper.hpp"

namespace DiscordBot
{
    class IMessageBase
    {
        public:
            IMessageBase() : Event(0), Handled(false) {}

            size_t Event;         //!< User defined Messagetype.
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

    class CMessageManager
    {
        public:
            using OnMessageReceive = std::function<void(const MessageBase Msg)>;

            CMessageManager(/* args */) : m_Terminated(false), m_Thread(&CMessageManager::Executor, this) {}

            /**
             * @brief Subscribes a message type.
             */
            void SubscribeMessage(size_t Event, OnMessageReceive callback)
            {
                std::lock_guard<std::mutex> lock(m_CallbackLock);
                m_Callbacks.insert({ Event, callback });
            }

            template<class T>
            void SendMessage(size_t Event, T Value)
            {
                using Message = std::shared_ptr<TMessage<T>>;

                std::lock_guard<std::mutex> lock(m_CallbackLock);
                Message Msg = Message(new TMessage<T>());
                Msg->Value = Value;
                Msg->Event = Event;

                SendMessage(Msg);
            }

            template<class T>
            void PostMessage(size_t Event, T Value, int Timeout = 0)
            {
                using Message = std::shared_ptr<TMessage<T>>;
                std::lock_guard<std::mutex> lock(m_QueueLock);

                Message Msg = Message(new TMessage<T>());
                Msg->Value = Value;
                Msg->Event = Event;
                Msg->Timeout = Timeout;
                Msg->CreateddMs = GetTimeMillis();

                m_Queue.push(std::static_pointer_cast<IMessageBase>(Msg));
            }

            ~CMessageManager() 
            {
                m_Terminated = true;
                if(m_Thread.joinable())
                    m_Thread.join();
            }

        private:
            void Executor()
            {
                while (!m_Terminated)
                {
                    {
                        std::lock_guard<std::mutex> lock(m_QueueLock);
                        if(!m_Queue.empty() && m_Queue.size() > 0)
                        {
                            MessageBase Data = m_Queue.front();
                            m_Queue.pop();

                            if(GetTimeMillis() - Data->CreateddMs > Data->Timeout)
                                SendMessage(Data);
                            else
                                m_Queue.push(Data);
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

            void SendMessage(MessageBase Msg)
            {
                auto IT = m_Callbacks.begin();
                while (IT != m_Callbacks.end())
                {
                    if((*IT).first == Msg->Event)
                        (*IT).second(Msg);

                    if(Msg->Handled)
                        break;

                    IT++;
                }
            }

            std::atomic<bool> m_Terminated;
            std::queue<MessageBase> m_Queue;
            std::mutex m_QueueLock;
            std::mutex m_CallbackLock;
            std::thread m_Thread;
            std::multimap<size_t, OnMessageReceive> m_Callbacks;
    };
} // namespace DiscordBot


#endif //MESSAGEMANAGER_HPP