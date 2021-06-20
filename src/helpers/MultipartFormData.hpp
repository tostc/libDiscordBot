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

#ifndef MULTIPARTFORMDATA_HPP
#define MULTIPARTFORMDATA_HPP

#include <DiscordBot/Models/File.hpp>
#include <string>
#include <map>

namespace DiscordBot    
{
    class CMultipartFormData
    {
        public:
            CMultipartFormData(/* args */) {}

            static std::string CreateFormData(const CFile &File, const std::string Payload)
            {
                std::string Ret = "\r\n--libDiscordBot\r\n";
                Ret += "Content-Disposition: form-data; name=\"payload_json\"\r\nContent-Type: application/json\r\n\r\n";
                Ret += Payload + "\r\n";
                Ret += "--libDiscordBot\r\n";
                Ret += "Content-Disposition: form-data; name=\"file\"; filename=\"" + File.Name + "\"";
                Ret += "Content-Type:" + GetContentType(File.Name) + "\r\n\r\n";

                for (auto &&e : File.Data)
                {
                    Ret += e;
                }
                
                Ret += "\r\n--libDiscordBot--\r\n\r\n";

                return Ret;
            }

            ~CMultipartFormData() {}

        private:
            static std::string GetContentType(const std::string &Filename)
            {
                static const std::map<std::string, std::string> MimeTypes = {
                    {"jpg", "image/jpeg"},
                    {"jpeg", "image/jpeg"},
                    {"png", "image/png"},
                    {"pdf", "application/pdf"}
                };

                size_t Pos = Filename.find_last_of(".");
                if(Pos == std::string::npos)
                    return "application/octet-stream";

                auto Ext = Filename.substr(Pos + 1);
                auto IT = MimeTypes.find(Ext);
                if(IT == MimeTypes.end())
                    return "application/octet-stream";

                return IT->second;
            }
    };
} // namespace DiscordBot   


#endif //MULTIPARTFORMDATA_HPP