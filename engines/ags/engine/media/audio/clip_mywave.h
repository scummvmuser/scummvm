/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef AGS_ENGINE_MEDIA_AUDIO_CLIP_MYWAVE_H
#define AGS_ENGINE_MEDIA_AUDIO_CLIP_MYWAVE_H

#include "media/audio/soundclip.h"

// My new MP3STREAM wrapper
struct MYWAVE:public SOUNDCLIP
{
    SAMPLE *wave;
    int voice;

    void poll() override;

    void set_volume(int new_speed) override;

    void destroy() override;

    void seek(int pos) override;

    int get_pos() override;
    int get_pos_ms() override;

    int get_length_ms() override;

    int get_sound_type() override;

    int play() override;

    MYWAVE();

protected:
    int get_voice() override;
    void adjust_volume() override;
};

#endif
