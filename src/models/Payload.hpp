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

#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include <JSON.hpp>

namespace DiscordBot
{
    /**
     * @brief This payload object will be received and sended to the discord servers.
     */
    struct SPayload
    {
        public:
            SPayload() : S(0) {}

            uint32_t OP;
            std::string D;
            uint32_t S;
            std::string T;

            void Deserialize(CJSON &json)
            {
                OP = json.GetValue<uint32_t>("op");
                D = json.GetValue<std::string>("d");
                S = json.GetValue<uint32_t>("s");
                T = json.GetValue<std::string>("t");
            }

            void Serialize(CJSON &json) const
            {
                json.AddPair("op", OP);

                if(D.empty())
                    json.AddPair("d", nullptr);
                else if(IsNumber(D))
                    json.AddPair("d", std::stoi(D));
                else
                    json.AddJSON("d", D);

                if(S != 0)
                    json.AddPair("s", S);

                if(!T.empty())
                    json.AddPair("t", T);
            }

            bool IsNumber(const std::string &Val) const
            {
                bool Ret = true;

                for (auto &&e : Val)
                {
                    if(!isdigit(e))
                    {
                        Ret = false;
                        break;
                    }
                }
                
                return Ret;
            }
    };

    /**
     * @brief Returns the name of a JSONErrorType enum value as string. Needed for logging.
     */
    inline std::string GetEnumName(JSONErrorType err)
    {
        switch (err)
        {
            case JSONErrorType::NAME_ALREADY_EXITS: return "NAME_ALREADY_EXITS";
            case JSONErrorType::NAME_NOT_FOUND: return "NAME_NOT_FOUND";
            case JSONErrorType::INVALID_JSON_OBJECT: return "INVALID_JSON_OBJECT";
            case JSONErrorType::INVALID_ARRAY: return "INVALID_ARRAY";
            case JSONErrorType::INVALID_TYPE: return "INVALID_TYPE";
            case JSONErrorType::INVALID_CAST: return "INVALID_CAST";
            case JSONErrorType::EXPECTED_BOOL: return "EXPECTED_BOOL";
            case JSONErrorType::EXPECTED_NULL: return "EXPECTED_NULL";
            case JSONErrorType::EXPECTED_NUM: return "EXPECTED_NUM";
            case JSONErrorType::WRONG_PLACED_SEPERATOR: return "WRONG_PLACED_SEPERATOR";
            case JSONErrorType::MISSING_KEY: return "MISSING_KEY";
            case JSONErrorType::MISSING_VALUE: return "MISSING_VALUE";
            case JSONErrorType::MISSING_SEPERATOR: return "MISSING_SEPERATOR";
        }

        return "";
    }
} // namespace DiscordBot


#endif //PAYLOAD_HPP