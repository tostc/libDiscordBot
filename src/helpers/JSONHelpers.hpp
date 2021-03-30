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

#ifndef JSONHELPERS_HPP
#define JSONHELPERS_HPP

#include <models/Embed.hpp>
#include <models/User.hpp>
#include <models/atomic.hpp>
#include <map>
#include <JSON.hpp>
#include <string>
#include <type_traits>
#include <utility>

#include <IDiscordClient.hpp>

namespace DiscordBot
{
    //--------------------------Abstract operators for json parsing (forward)--------------------------//

    template<class T, class FN>
    typename std::result_of<FN&(T)>::type operator|(const T &obj, FN f);

    template<class T>
    T operator|(atomic<std::map<std::string, T>> &map, const std::string &js);

    template<class JSType, class T>
    T& operator>>(const JSType &js, T &obj);

    template<class T>
    std::string& operator>>(const T &obj, std::string &js);

    template<class T>
    atomic<std::map<std::string, T>>& operator>>(const T &obj, atomic<std::map<std::string, T>> &map);

    template<class T>
    std::pair<std::string, atomic<std::map<std::string, T>>&> operator&(const std::string &js, atomic<std::map<std::string, T>> &map);

    //--------------------------JSON Parsing--------------------------//

    template<class T>
    typename std::enable_if<std::is_same<T, User>::value, User>::type Deserialize(const std::string &JS)
    {
        CJSON json;
        json.ParseObject(JS);

        User Ret = User(new CUser());

        Ret->ID = json.GetValue<std::string>("id");
        Ret->Username = json.GetValue<std::string>("username");
        Ret->Discriminator = json.GetValue<std::string>("discriminator");
        Ret->Avatar = json.GetValue<std::string>("avatar");
        Ret->Bot = json.GetValue<bool>("bot");
        Ret->System = json.GetValue<bool>("system");
        Ret->MFAEnabled = json.GetValue<bool>("mfa_enabled");
        Ret->Locale = json.GetValue<std::string>("locale");
        Ret->Verified = json.GetValue<bool>("verified");
        Ret->Email = json.GetValue<std::string>("email");
        Ret->Flags = (UserFlags)json.GetValue<int>("flags");
        Ret->PremiumType = (PremiumTypes)json.GetValue<int>("premium_type");
        Ret->PublicFlags = (UserFlags)json.GetValue<int>("public_flags");

        Ret->State = OnlineState::ONLINE;
        Ret->Desktop = OnlineState::ONLINE;
        Ret->Mobile = OnlineState::OFFLINE;
        Ret->Web = OnlineState::ONLINE;

        // m_Users[Ret->ID] = Ret;

        return Ret;
    }

    template<class T>
    typename std::enable_if<std::is_same<T, Role>::value, Role>::type Deserialize(const std::string &JS)
    {
        CJSON json;
        json.ParseObject(JS);

        Role ret = Role(new CRole());

        ret->ID = json.GetValue<std::string>("id");
        ret->Name = json.GetValue<std::string>("name");
        ret->Color = json.GetValue<uint32_t>("color");
        ret->Hoist = json.GetValue<bool>("hoist");
        ret->Position = json.GetValue<int>("position");
        ret->Permissions = (Permission)json.GetValue<uint32_t>("permissions");
        ret->Managed = json.GetValue<bool>("managed");
        ret->Mentionable = json.GetValue<bool>("mentionable");

        return ret;
    }

    template<class T>
    typename std::enable_if<std::is_same<T, Channel>::value, Channel>::type Deserialize(IDiscordClient *client, std::string data, atomic<std::map<std::string, User>>& Users)
    {
        Channel Ret = Channel(new CChannel(client));

        CJSON json;
        json.ParseObject(data);

        Ret->ID = json.GetValue<std::string>("id");
        Ret->Type = (ChannelTypes)json.GetValue<int>("type");
        Ret->GuildID = json.GetValue<std::string>("guild_id");
        Ret->Position = json.GetValue<int>("position");

        std::vector<std::string> Array = json.GetValue<std::vector<std::string>>("permission_overwrites");
        for (auto &&e : Array)
        {
            PermissionOverwrites ov = PermissionOverwrites(new CPermissionOverwrites());
            CJSON jov;
            jov.ParseObject(e);

            ov->ID = jov.GetValue<std::string>("id");
            ov->Type = jov.GetValue<std::string>("type");
            ov->Allow = (Permission)jov.GetValue<int>("allow");
            ov->Deny = (Permission)jov.GetValue<int>("deny");

            Ret->Overwrites->push_back(ov);
        }

        Ret->Name = json.GetValue<std::string>("name");
        Ret->Topic = json.GetValue<std::string>("topic");
        Ret->NSFW = json.GetValue<bool>("nsfw");
        Ret->LastMessageID = json.GetValue<std::string>("last_message_id");
        Ret->Bitrate = json.GetValue<int>("bitrate");
        Ret->UserLimit = json.GetValue<int>("user_limit");
        Ret->RateLimit = json.GetValue<int>("rate_limit_per_user");

        Array = json.GetValue<std::vector<std::string>>("recipients");
        for (auto &&e : Array)
        {
            User user = Users | e;
            Ret->Recipients->push_back(user);
        }

        Ret->Icon = json.GetValue<std::string>("icon");
        Ret->OwnerID = json.GetValue<std::string>("owner_id");
        Ret->AppID = json.GetValue<std::string>("application_id");
        Ret->ParentID = json.GetValue<std::string>("parent_id");
        Ret->LastPinTimestamp = json.GetValue<std::string>("last_pin_timestamp");

        return Ret;
    }

    inline std::string Serialize(const Embed &e)
    {
        CJSON js;

        js.AddPair("title", e->Title.load());
        js.AddPair("description", e->Description.load());

        if(!e->URL->empty())
            js.AddPair("url", e->URL.load());

        if(!e->Type->empty())
            js.AddPair("type", e->Type.load());

        return js.Serialize();
    }

    //--------------------------Abstract operators for json parsing--------------------------//

    /**
     * @brief Calls a functions with the object on the left hand side. (e.g.: Obj | Serialize)
     */
    template<class T, class FN>
    inline typename std::result_of<FN&(T)>::type operator|(const T &obj, FN f)
    {
        return f(obj);
    }

    /**
     * @brief Gets or creates a object and adds the new object to the map. (e.g.: m_Users | json)
     * 
     * @return Returns the json object as c++ object.
     */
    template<class T>
    inline T operator|(atomic<std::map<std::string, T>> &map, const std::string &js)
    {
        CJSON json;
        json.ParseObject(js);

        T Ret;

        auto IT = map->find(json.GetValue<std::string>("id"));
        if(IT != map->end())
            Ret = IT->second;
        else 
        {
            Ret = Deserialize<T>(js);
            map->insert({Ret->ID, Ret});
        } 

        return Ret;
    }

    /**
     * @brief Deserializes a json to an object.
     */
    template<class JSType, class T>
    inline T& operator>>(const JSType &js, T &obj)
    {
        obj = Deserialize<T>(js);
        return obj;
    }

    /**
     * @brief Serializes an object to a json.
     */
    template<class T>
    inline std::string& operator>>(const T &obj, std::string &js)
    {
        js = Serialize(obj);
        return js;
    }

    /**
     * @brief Inserts a object to a map.
     */
    template<class T>
    inline atomic<std::map<std::string, T>>& operator>>(const T &obj, atomic<std::map<std::string, T>> &map)
    {
        map->insert({obj->ID, obj});
        return map;
    }

    /**
     * @brief Combines a json string and a map to a pair.
     */
    template<class T>
    inline std::pair<std::string, atomic<std::map<std::string, T>>&> operator&(const std::string &js, atomic<std::map<std::string, T>> &map)
    {
        return {js, map};
    }
} // namespace DiscordBot


#endif //JSONHELPERS_HPP