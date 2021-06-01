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

#ifndef VOICECLIENT_HPP
#define VOICECLIENT_HPP

#include <controller/IAudioSource.hpp>
#include <controller/IMusicQueue.hpp>
#include <models/voice/SongInfo.hpp>
#include <memory>

namespace DiscordBot
{
    class CVoiceClient
    {
        public:
            CVoiceClient(/* args */) {}

            /**
             * @brief Adds a song to the music queue.
             * 
             * @param Info: Song informations.
             */
            void AddToQueue(SongInfo Info);

            /**
             * @brief Connects to the given channel and uses the queue to speak.
             * 
             * @return Returns true if the connection succeeded.
             */
            bool StartSpeaking();

            /**
             * @brief Connects to the given channel and uses the source to speak.
             * 
             * @param source: The audio source for speaking.
             * 
             * @return Returns true if the connection succeeded.
             */
            bool StartSpeaking(AudioSource source);

            /**
             * @brief Pauses the audio source. @see ResumeSpeaking to continue streaming.
             */
            void PauseSpeaking();

            /**
             * @brief Resumes the audio source.
             */
            void ResumeSpeaking();

            /**
             * @brief Stops the audio source.
             */
            void StopSpeaking();

            /**
             * @brief Removes a song from the queue by its index.
             */
            void RemoveSong(size_t Index);

            /**
             * @brief Removes a song from the queue by its title or part of the title.
             */
            void RemoveSong(const std::string &Name);

            /**
             * @return Returns the audio source for the given guild. Null if there is no audio source available.
             */
            AudioSource GetAudioSource();

            /**
             * @return Returns the music queue for the given guild. Null if there is no music queue available.
             */
            MusicQueue GetMusicQueue();

            /**
             * @return Returns true if a audio source is playing in the given guild.
             */
            bool IsPlaying();

            ~CVoiceClient() = default;

        private:
        /* data */
    };

    using VoiceClient = std::shared_ptr<CVoiceClient>;
} // namespace DiscordBot


#endif //VOICECLIENT_HPP