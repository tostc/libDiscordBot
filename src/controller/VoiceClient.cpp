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

#include <DiscordBot/Voice/VoiceClient.hpp>

namespace DiscordBot
{
    CVoiceClient::Factory CVoiceClient::m_QueueFactory;

    CVoiceClient::CVoiceClient(VoiceSocket socket)
    {
        m_Socket = socket;
        m_Queue = m_QueueFactory->Create();
    }

    AudioSource CVoiceClient::GetAudioSource()
    {
        return m_Socket->GetAudioSource();
    }   

    MusicQueue CVoiceClient::GetMusicQueue()
    {
        return m_Queue;
    }

    bool CVoiceClient::IsPlaying()
    {
        return GetAudioSource() != nullptr;
    }

    void CVoiceClient::AddToQueue(SongInfo Info)
    {
        m_Queue->AddSong(Info);
    }

    void CVoiceClient::StartSpeaking()
    {
        AudioSource Source;

        if(m_Queue->HasNext())
            Source = m_Queue->Next();
        else
            m_Queue->ClearQueue();

        StartSpeaking(Source);
    }

    void CVoiceClient::StartSpeaking(AudioSource source)
    {
        m_Socket->StartSpeaking(source);
    }

    void CVoiceClient::PauseSpeaking()
    {
        m_Socket->PauseSpeaking();
    }

    void CVoiceClient::ResumeSpeaking() 
    {
        m_Socket->ResumeSpeaking();
    }

    void CVoiceClient::StopSpeaking()
    {
        m_Socket->StopSpeaking();
    }

    void CVoiceClient::RemoveSong(size_t Index)
    {
        m_Queue->RemoveSong(Index);
    }

    void CVoiceClient::RemoveSong(const std::string &Name)
    {
        m_Queue->RemoveSong(Name);
    }

    /*void CVoiceClient::OnSpeakFinish(const std::string &Guild)
    {
        if(m_Controller)
        {
            m_EVManger.PostMessage(QUEUE_NEXT_SONG, Guild);

            auto IT = m_Guilds->find(Guild);
            if(IT != m_Guilds->end())
                m_Controller->OnEndSpeaking(IT->second);
        }
    }

    void CVoiceClient::OnQueueWaitFinish(const std::string &Guild, AudioSource Source)
    {
        if(!Source)
        {
            m_EVManger.PostMessage(QUEUE_NEXT_SONG, Guild);
            return;
        }

        VoiceSockets::iterator IT = m_VoiceSockets->find(Guild);
        if(IT != m_VoiceSockets->end())
            IT->second->StartSpeaking(Source);
    }*/
} // namespace DiscordBot
