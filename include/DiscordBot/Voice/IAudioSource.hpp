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

#ifndef IAUDIOSOURCE_HPP
#define IAUDIOSOURCE_HPP

#include <stdint.h>
#include <memory>

namespace DiscordBot
{
    /**
     * @brief Interface for voice connections. 
     */
    class IAudioSource
    {
        public:
            IAudioSource() {}

            /**
             * @brief Called if more audio data is needed.
             * 
             * @param Buf: Buffer to fill.
             * @param Samples: Samples per channel. The complete buffer size if Samples * Channelcount. The channel count used by this library is 2.
             * 
             * @note The audio samplerate must 48000 Hz and stereo.
             * 
             * @return Must return the filled samples. If the return value is smaller than the Samples parameter, the playback is stopped after sending.
             */
            virtual uint32_t OnRead(uint16_t *Buf, uint32_t Samples) = 0;

            ~IAudioSource() {}
    };

    using AudioSource = std::shared_ptr<IAudioSource>;
} // namespace DiscordBot


#endif //IAUDIOSOURCE_HPP