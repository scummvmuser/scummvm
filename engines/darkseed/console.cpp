/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/debug.h"
#include "darkseed/console.h"
#include "darkseed/darkseed.h"

namespace Darkseed {

static constexpr Common::Rect consoleArea = {{0x70, 280}, 416, 44};

Console::Console(TosText *tosText, Sound *sound) : _tosText(tosText), _sound(sound) {
	if (!_font.load()) {
		error("Error loading tosfont.nsp");
	}
	_text.resize(10);
}

void Console::printTosText(int tosIndex) {
	const Common::String &text = _tosText->getText(tosIndex);
	debug(text.c_str());
	addLine(" ");
	addTextLine(text);
	_sound->playTosSpeech(tosIndex);
}

void Console::addTextLine(const Common::String &text) {
	auto lines = wrapText(text);
	for (auto &line : lines) {
		addLine(line);
	}
}

void Console::addToCurrentLine(const Common::String &text) {
	int curIdx = _startIdx == 0 ? _text.size() - 1 : _startIdx - 1;
	_startIdx = curIdx;
	addTextLine(_text[_startIdx] + text);
}

void Console::draw() {
	if (!redrawRequired) {
		return;
	}

	g_engine->_screen->fillRect(consoleArea, 0);
	int curIdx = _startIdx == 0 ? _text.size() - 1 : _startIdx - 1;
	int y = 0x139;
	for (int i = 0; i < 4 && curIdx != _startIdx && !_text[curIdx].empty(); i++) {
		_font.displayString(0x70, y, _text[curIdx]);
		y -= 11;
		curIdx = curIdx == 0 ? _text.size() - 1 : curIdx - 1;
	}
	redrawRequired = false;
	g_engine->_screen->addDirtyRect(consoleArea);
}

Common::Array<Common::String> Console::wrapText(const Common::String &text) {
	Common::Array<Common::String> lines;
	Common::String line;
	Common::String word;
	int lineLength = 0;

	for (int i = 0; i < text.size(); i++) {
		if (text[i] != '\r') {
			word += text[i];
		}
		if (text[i] == ' ' || text[i] == '\r') {
			int wordLength = _font.stringLength(word);
			if (lineLength + wordLength > consoleArea.width()) {
				lines.push_back(line);
				line = word;
				lineLength = wordLength;
			} else {
				line += word;
				lineLength += wordLength;
			}
			word = "";
		}
		if (text[i] == '\r') {
			break;
		}
	}

	if (!line.empty() || !word.empty()) {
		int wordLength = _font.stringLength(word);
		if (lineLength + wordLength > consoleArea.width()) {
			lines.push_back(line);
			lines.push_back(word);
		} else {
			line += word;
			lines.push_back(line);
		}
	}

	return lines;
}

void Console::addLine(const Common::String &line) {
	_text[_startIdx] = line;
	_startIdx = (_startIdx + 1) % _text.size();
	redrawRequired = true;
}

} // End of namespace Darkseed
