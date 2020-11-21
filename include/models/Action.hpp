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

#ifndef ACTION_HPP
#define ACTION_HPP

#include <models/Channel.hpp>

namespace DiscordBot
{
    enum class ActionType : uint32_t
    {
        NONE = 0,
        USER_JOIN = 1,      //!< Action must be from the type GuildMember
        USER_LEAVE = 2,     //!< Action must be from the type GuildMember

        RESERVED_1 = 4,
        RESERVED_2 = 8,
        RESERVED_3 = 16,
        RESERVED_4 = 32,

        MESSAGE_CREATED = 64,
        MESSAGE_EDITED = 128,
        MESSAGE_DELETED = 256,

        TOTAL_ACTIONS
    };

    inline ActionType operator| (ActionType lhs, ActionType rhs)  
    {
        return static_cast<ActionType>(static_cast<unsigned>(lhs) |static_cast<unsigned>(rhs));
    }  

    inline ActionType operator& (ActionType lhs, ActionType rhs)  
    {
        return static_cast<ActionType>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
    }  

    class IAction
    {
        public:
            IAction(ActionType Type) : m_Types(Type) {}

            inline ActionType GetTypes() const
            {
                return m_Types;
            }

            virtual ~IAction() = default;

        private:
            ActionType m_Types;
    };

    template <typename T>
    class CAction : public IAction
    {    
        public:
            CAction(ActionType Types) : IAction(Types) {}

            /**
             * @brief Filters the current event and checks if this event should be fired.
             * 
             * @param Type: Type of the action
             * @param c: Channel which triggered this action
             * @param val: Discord object which had triggered the channel.
             * 
             * @return Returns true if the event should be fired.
             */
            virtual bool Filter(ActionType Type, Channel c, T val)
            {
                return true;
            }

            /**
             * @brief Fires/executes the action. Here you can wirte your logic.
             * 
             * @param Type: Type of the action
             * @param c: Channel which triggered this action
             * @param val: Discord object which had triggered the channel.
             */
            virtual void FireAction(ActionType Type, Channel c, T val) = 0;

            virtual ~CAction() = default;
    };
    
    using Action = std::shared_ptr<IAction>;

} // namespace DiscordBot


#endif //ACTION_HPP