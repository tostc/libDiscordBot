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

#ifndef IMUSICQUEUE_HPP
#define IMUSICQUEUE_HPP

#include <memory>
#include <models/voice/SongInfo.hpp>
#include <controller/IAudioSource.hpp>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <config.h>

namespace DiscordBot
{
    using OnWaitFinish = std::function<void(const std::string&, AudioSource)>;

    /**
     * @brief Thread safe music queue interface.
     */
    class DISCORDBOT_EXPORT IMusicQueue
    {
        public:
            IMusicQueue() : m_NeedWait(false), m_QueueIndex(0), m_QueueSize(0) {}
            
            /**
             * @brief Adds a song to the queue. Calls OnUpdate after the song is added. Called from the client.
             * 
             * @param Info: The new song to insert.
             */
            virtual void AddSong(SongInfo Info);

            /**
             * @brief Removes a song at a given index. Calls OnRemove after removing the song.
             * 
             * @param Index: The song index to remove.
             */
            virtual void RemoveSong(size_t Index);

            /**
             * @brief Removes a song with a given name. Calls OnRemove after removing the song.
             * 
             * @param Name: Name or part of the name to remove.
             */
            virtual void RemoveSong(std::string Name);

            /**
             * @brief Called if the queue finished playing. Calls OnRemove for each element.
             */
            virtual void ClearQueue();

            /**
             * @return Returns a audio source for the next song. If a song is not ready call Wait and return null.
             */
            virtual AudioSource Next();

            /**
             * @return Returns true if there is a next song.
             */
            bool HasNext();

            /**
             * @brief Returns true if a song is not ready.
             */
            inline bool NeedWait()
            {
                return m_NeedWait;
            }

            inline void SetGuildID(const std::string &ID)
            {
                m_GuildID = ID;
            }

            /**
             * @brief Called by the client to the on wait finish callback.
             */
            inline void SetOnWaitFinishCallback(OnWaitFinish Call)
            {
                m_WaitFinishCallback = Call;
            }

            virtual ~IMusicQueue() 
            {
                ClearQueue();
            }
        protected:
            /**
             * @return Gets the song at a given index. Returns null if the index is out of bounds.
             */
            SongInfo GetSong(size_t Index);

            /**
             * @return Gets the current queue index.
             */
            size_t GetQueueIndex();

            /**
             * @return Gets the current queue size.
             */
            size_t GetQueueSize();

            /**
             * @brief Should called after Wait, if the song becomes ready. Calls OnNext.
             */
            void WaitFinished();

            /**
             * @brief Should called after Wait, if the song becomes not ready. Calls indirect Next.
             */
            void WaitFailed();

            /**
             * @brief Called after adding songs to the queue.
             * 
             * @param Info: Song which was added.
             * @param Index: Index inside the queue.
             */
            virtual void OnUpdate(SongInfo Info, size_t Index) {}

            /**
             * @brief Called after removing a song from the queue.
             * 
             * @param Info: Song which was removed.
             * @param Index: Old index inside the queue.
             */
            virtual void OnRemove(SongInfo Info, size_t Index) {}

            /**
             * @brief Called if the song is finished playing.
             * 
             * @param Info: Song which finished playing.
             */
            virtual void OnFinishPlaying(SongInfo Info) {}

            /**
             * @brief Called by Next, if a audiosource if needed.
             * 
             * @param Info: Song to play.
             * 
             * @return Returns a new audio source to play or null if the song isn't ready.
             */
            virtual AudioSource OnNext(SongInfo Info) = 0;
        private:
            /**
             * @brief Needs to call if a song is not ready to play. For example if you download a song on the fly. Should called in Next.
             */
            void Wait(SongInfo Info);

            /**
             * @return Gets the song at a given index. Returns null if the index is out of bounds.
             */
            SongInfo GetSongInternal(size_t Index);

            OnWaitFinish m_WaitFinishCallback;
            std::atomic<bool> m_NeedWait;
            std::string m_GuildID;
            std::atomic<size_t> m_QueueIndex; 
            std::atomic<size_t> m_QueueSize;    //!< Used to prevent dead locks.
            SongInfo m_WaitSong;

            std::mutex m_QueueLock;
            std::vector<SongInfo> m_Queue;
    };

    using MusicQueue = std::shared_ptr<IMusicQueue>;
} // namespace DivineBot


#endif //IMUSICQUEUE_HPP