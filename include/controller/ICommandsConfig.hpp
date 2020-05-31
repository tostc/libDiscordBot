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

#ifndef ICOMMANDSCONFIG_HPP
#define ICOMMANDSCONFIG_HPP

#include <vector>
#include <string>
#include <memory>

namespace DiscordBot
{
    class ICommandsConfig
    {
        public:
            ICommandsConfig() {}

            /**
             * @brief Adds roles to a given command for a given server.
             * 
             * @param Guild: Server to save the config for.
             * @param Command: Command name. e.g. 'h'
             * @param Roles: List of roles to apply to the command. Only users with these roles can access the command.
             */
            virtual void AddRoles(const std::string &Guild, const std::string &Command, const std::vector<std::string> &Roles) = 0;

            /**
             * @return Returns a list of a command for a given guild.
             */
            virtual std::vector<std::string> GetRoles(const std::string &Guild, const std::string &Command) = 0;

            /**
             * @brief Deletes the config for a given command on a given server.
             */
            virtual void DeleteCommand(const std::string &Guild, const std::string &Command) = 0;

            /**
             * @brief Removes given roles of a command.
             */
            virtual void RemoveRoles(const std::string &Guild, const std::string &Command, const std::vector<std::string> &Roles) = 0;

            virtual ~ICommandsConfig() {}
    };

    using CommandsConfig = std::shared_ptr<ICommandsConfig>;
} // namespace DiscordBot

#endif //ICOMMANDSCONFIG_HPP