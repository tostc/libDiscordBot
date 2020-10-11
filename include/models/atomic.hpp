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

#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#include <iostream>
#include <mutex>

namespace DiscordBot
{
    template<class T>
    class atomic
    {
        struct locked_ref
        {
            public:
                locked_ref(std::recursive_mutex &lock, T* ref) : m_Lock(lock), m_Ref(ref)
                {
                    m_Lock.lock();
                }

                inline T *operator->() 
                {
                    return m_Ref;
                }

                ~locked_ref()
                {
                    m_Lock.unlock();
                }

            private:
                std::recursive_mutex &m_Lock;
                T *m_Ref;
        };
        

        public:
            atomic(/* args */) {}
            atomic(const atomic<T>& val) 
            {
                m_Value = val.load();
            }

            inline operator T() const noexcept
            {
                return load();
            }

            inline atomic &operator=(const T& val)
            {
                std::lock_guard<std::recursive_mutex> lock(m_Lock);
                m_Value = val;

                return *this;
            }

            inline atomic &operator=(const atomic<T>& val)
            {
                std::lock_guard<std::recursive_mutex> lock(m_Lock);
                m_Value = val.load();

                return *this;
            }

            inline bool operator==(const T &rhs)
            {
                std::lock_guard<std::recursive_mutex> lock(m_Lock);
                return m_Value == rhs;
            }

            inline bool operator==(const atomic<T> &rhs)
            {
                std::lock_guard<std::recursive_mutex> lock(m_Lock);
                return m_Value == rhs.load();
            }

            inline T load() const noexcept
            {
                std::lock_guard<std::recursive_mutex> lock(m_Lock);
                return m_Value;
            }

            inline locked_ref operator->() 
            {
                return {m_Lock, &m_Value};
            }

            ~atomic() {}

        private:
            mutable std::recursive_mutex m_Lock;

            T m_Value;
    };

    template<class lT, class rT>
    inline rT operator+(const lT &lhs, const atomic<rT> &rhs)
    {
        return lhs + rhs.load();
    }

    template<class lT, class rT>
    inline rT operator-(const lT &lhs, const atomic<rT> &rhs)
    {
        return lhs - rhs.load();
    }

    template<class lT, class rT>
    inline rT operator*(const lT &lhs, const atomic<rT> &rhs)
    {
        return lhs * rhs.load();
    }

    template<class lT, class rT>
    inline rT operator/(const lT &lhs, const atomic<rT> &rhs)
    {
        return lhs / rhs.load();
    }

    template<class lT, class rT>
    inline bool operator==(const lT &lhs, const atomic<rT> &rhs)
    {
        return lhs == rhs.load();
    }

    //---------------------iostream operators---------------------//

    template<class T>
    inline std::ostream &operator<<(std::ostream &of, const atomic<T> &rhs)
    {
        of << rhs.load();
        return of;
    }

    template<class T>
    inline std::istream &operator>>(std::istream &in, atomic<T> &rhs)
    {
        T val;
        in >> val;
        rhs = val;
        return in;
    }
} // namespace DiscordBot


#endif //ATOMIC_HPP