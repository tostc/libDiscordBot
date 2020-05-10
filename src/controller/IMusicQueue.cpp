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

#include <controller/IMusicQueue.hpp>

namespace DiscordBot
{
    /**
     * @brief Adds a song to the queue. Calls OnUpdate after the song is added. Called from the client.
     * 
     * @param Info: The new song to insert.
     */
    void IMusicQueue::AddSong(SongInfo Info)
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        m_Queue.push_back(Info);

        m_QueueSize = m_Queue.size();

        OnUpdate(Info, m_Queue.size() - 1);
    }

    /**
     * @brief Removes a song at a given index. Calls OnRemove after removing the song.
     * 
     * @param Index: The song index to remove.
     */
    void IMusicQueue::RemoveSong(size_t Index)
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);

    }

    /**
     * @brief Removes a song with a given name. Calls OnRemove after removing the song.
     * 
     * @param Name: Name or part of the name to remove.
     */
    void IMusicQueue::RemoveSong(std::string Name)
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
    }

    /**
     * @brief Called if the queue finished playing. Calls OnRemove for each element.
     */
    void IMusicQueue::ClearQueue()
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        auto IT = m_Queue.begin();
        while (IT != m_Queue.end())
        {
            OnRemove(*IT, 0);
            IT = m_Queue.erase(IT);
        }

        m_QueueIndex = 0;
        m_QueueSize = 0;
    }

    /**
     * @return Returns a audio source for the next song. If a song is not ready call Wait and return null.
     */
    AudioSource IMusicQueue::Next()
    {
        if(m_NeedWait)
            return nullptr;

        SongInfo Info;

        {
            std::lock_guard<std::mutex> lock(m_QueueLock);
            size_t Prev = m_QueueIndex - 1;
            if(Prev != (size_t)-1)
                OnFinishPlaying(GetSongInternal(Prev));

            Info = GetSongInternal(m_QueueIndex++);
        }

        AudioSource Ret = OnNext(Info);
        if(!Ret)
            Wait(Info);
        
        return Ret;
    }

    /**
     * @return Returns true if there is a next song.
     */
    bool IMusicQueue::HasNext()
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        return m_QueueIndex < m_Queue.size();
    }

    /**
     * @return Gets the song at a given index. Returns null if the index is out of bounds.
     */
    SongInfo IMusicQueue::GetSong(size_t Index)
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        return GetSongInternal(Index);
    }

    /**
     * @return Gets the current queue index.
     */
    size_t IMusicQueue::GetQueueIndex()
    {
        return m_QueueIndex;
    }

    /**
     * @return Gets the current queue size.
     */
    size_t IMusicQueue::GetQueueSize()
    {
        return m_QueueSize;
    }

    /**
     * @brief Should called after Wait, if the song becomes ready.
     */
    void IMusicQueue::WaitFinished()
    {
        if(m_NeedWait && m_WaitFinishCallback)
            m_WaitFinishCallback(m_GuildID, OnNext(m_WaitSong));

        m_NeedWait = false;
        m_WaitSong = nullptr;
    }

    /**
     * @brief Should called after Wait, if the song becomes not ready. Calls indirect Next.
     */
    void IMusicQueue::WaitFailed()
    {
        if(m_NeedWait && m_WaitFinishCallback)
            m_WaitFinishCallback(m_GuildID, nullptr);

        m_NeedWait = false;
        m_WaitSong = nullptr;
    }

    /**
     * @brief Needs to call if a song is not ready to play. For example if you download a song on the fly. Should called in Next.
     */
    void IMusicQueue::Wait(SongInfo Info)
    {
        m_NeedWait = true;
        m_WaitSong = Info;
    }

    /**
     * @return Gets the song at a given index. Returns null if the index is out of bounds.
     */
    SongInfo IMusicQueue::GetSongInternal(size_t Index)
    {
        if(Index < m_Queue.size())
            return m_Queue[Index];

        return nullptr;
    }
} // namespace DiscordBot
