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

#ifndef JSONCMDSCONFIG_HPP
#define JSONCMDSCONFIG_HPP

#include <DiscordBot/Commands/ICommandsConfig.hpp>
#include <JSON.hpp>

namespace DiscordBot
{
    class CJSONCmdsConfig : public ICommandsConfig
    {
        public:
            CJSONCmdsConfig();

            void AddRoles(const std::string &Guild, const std::string &Command, const std::vector<std::string> &Roles) override;
            std::vector<std::string> GetRoles(const std::string &Guild, const std::string &Command) override;
            void DeleteCommand(const std::string &Guild, const std::string &Command) override;
            void RemoveRoles(const std::string &Guild, const std::string &Command, const std::vector<std::string> &Roles) override;
            
            void ChangePrefix(const std::string &Guild, const std::string &Prefix) override;
            void RemovePrefix(const std::string &Guild) override;
            std::string GetPrefix(const std::string &Guild, const std::string &Default) override;

            ~CJSONCmdsConfig() {}
        private:
            void SaveCmdDB();
            void SavePrefixDB();

            using CmdDatabase = std::map<std::string, std::map<std::string, std::vector<std::string>>>;
            CmdDatabase m_CmdDatabase;

            using PrefixDatabase = std::map<std::string, std::string>;
            PrefixDatabase m_PrefixDatabase;
    };
} // namespace DiscordBot


#endif //JSONCMDSCONFIG_HPP