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

#include "action_talk.h"
#include <pink/archive.h>
#include <pink/objects/actors/actor.h>
#include <pink/objects/pages/game_page.h>
#include <pink/cel_decoder.h>
#include <pink/sound.h>

namespace Pink {

void ActionTalk::deserialize(Archive &archive) {
    ActionLoop::deserialize(archive);
    archive >> _vox;
}

void ActionTalk::toConsole() {
    debug("\tActionTalk: _name = %s, _fileName = %s, z = %u, _startFrame = %u,"
                  " _endFrame = %u, _intro = %u, _style = %u, _vox = %s",
          _name.c_str(), _fileName.c_str(), _z, _startFrame, _stopFrame, _intro, _style, _vox.c_str());
}

void ActionTalk::onStart() {
    ActionPlay::onStart();
    _sound = _actor->getPage()->loadSound(_vox);
    _sound->play(Audio::Mixer::SoundType::kSpeechSoundType, 100, 0);
}

void ActionTalk::update() {
    ActionLoop::update();
    if (!_sound->isPlaying()) {
        _decoder->stop();
        _actor->endAction();
    }
}

void ActionTalk::end() {
    ActionPlay::end();
    delete _sound;
    _sound = nullptr;
}

void ActionTalk::pause() {
    ActionCEL::pause();
    if (_sound)
        _sound->pause();
}

void ActionTalk::unpause() {
    ActionCEL::unpause();
    if (_sound)
        _sound->resume();
}

} // End of namespace Pink