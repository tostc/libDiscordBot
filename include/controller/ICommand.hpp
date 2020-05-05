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

#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <models/Message.hpp>
#include <functional>

namespace DiscordBot
{
    /**
     * @brief Contains all informations about a command which was sended.
     */
    class CCommandContext
    { 
        public:
            CCommandContext(/* args */) = default;

            Message Msg;                        //!< Message wich contains the command.
            std::string Command;                //!< Command without prefix.
            std::vector<std::string> Params;    //!< Command parameters.

            ~CCommandContext() = default;
    };

    using CommandContext = std::shared_ptr<CCommandContext>;

    /**
     * @brief Interface to handle commands.
     */
    class ICommand
    {
        public:
            using OnExecuteCommand = std::function<void(CommandContext)>;

            ICommand() = default;

            /**
             * @brief Called by the controller if a command with is associated with this instance, is called.
             */
            void OnExecute(CommandContext Context);

            virtual ~ICommand() = default;

        protected:
            /**
             * @brief Registers a new command handler function.
             * 
             * @param cmd: Command name.
             * @param call: Method to call.
             */
            void RegisterCommandHandler(const std::string &Cmd, OnExecuteCommand call);

        private:
            std::map<std::string, OnExecuteCommand> m_CommandHandler;
    };
} // namespace DiscordBot


#endif //ICOMMAND_HPP