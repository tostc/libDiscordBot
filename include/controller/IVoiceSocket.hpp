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

#ifndef IVOICESOCKET_HPP
#define IVOICESOCKET_HPP

#include <controller/IAudioSource.hpp>
#include <functional>
#include <string>

namespace DiscordBot
{
    class IVoiceSocket
    {
        public:
            using OnStopSpeaking = std::function<void(const std::string&)>;

            /**
             * @brief Sets the callback which is called if the audio source finished.
             */
            virtual void SetOnSpeakFinish(OnStopSpeaking call) = 0;
            
            /**
             * @brief Starts a new audio stream. Stops the old one.
             * 
             * @param Source: Audiosource which is send to discord.
             */
            virtual void StartSpeaking(AudioSource Source) = 0;

            /**
             * @brief Pause the sending of audio.
             */
            virtual void PauseSpeaking() = 0;

            /**
             * @brief Resumes the sending of audio.
             */
            virtual void ResumeSpeaking() = 0;

            /**
             * @brief Stops the sending of audio. Raise a OnSpeakFinish event.
             */
            virtual void StopSpeaking() = 0;

            /**
             * @return Gets the current playing audio source or null.
             */
            virtual AudioSource GetAudioSource() = 0;
    };

    using VoiceSocket = std::shared_ptr<IVoiceSocket>;
} // namespace DiscordBot


#endif //IVOICESOCKET_HPP