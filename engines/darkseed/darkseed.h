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

#ifndef DARKSEED_H
#define DARKSEED_H

#include "common/scummsys.h"
#include "common/system.h"
#include "common/error.h"
#include "common/fs.h"
#include "common/hash-str.h"
#include "common/random.h"
#include "common/serializer.h"
#include "common/util.h"
#include "engines/engine.h"
#include "engines/savestate.h"
#include "graphics/screen.h"

#include "console.h"
#include "cursor.h"
#include "darkseed/detection.h"
#include "nsp.h"
#include "player.h"
#include "room.h"
#include "tostext.h"

namespace Darkseed {

struct DarkseedGameDescription;

enum ActionMode {
	PointerAction = 0,
	HandAction = 2,
	LookAction = 3
};

class DarkseedEngine : public Engine {
private:
	const ADGameDescription *_gameDescription;
	Common::RandomSource _randomSource;
	Pic _frame;

protected:
	// Engine APIs
	Common::Error run() override;
public:
	bool _isRightMouseClicked = false;
	bool _isLeftMouseClicked = false;

	Nsp _baseSprites;
	Cursor _cursor;
	Graphics::Screen *_screen = nullptr;
	TosText *_tosText = nullptr;
	Console *_console = nullptr;
	Room *_room = nullptr;
	ActionMode _actionMode = PointerAction;
	Player _player;

public:
	DarkseedEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~DarkseedEngine() override;

	uint32 getFeatures() const;

	/**
	 * Returns the game Id
	 */
	Common::String getGameId() const;

	/**
	 * Gets a random number
	 */
	uint32 getRandomNumber(uint maxNum) {
		return _randomSource.getRandomNumber(maxNum);
	}

	bool hasFeature(EngineFeature f) const override {
		return
		    (f == kSupportsLoadingDuringRuntime) ||
		    (f == kSupportsSavingDuringRuntime) ||
		    (f == kSupportsReturnToLauncher);
	};

	bool canLoadGameStateCurrently() override {
		return true;
	}
	bool canSaveGameStateCurrently() override {
		return true;
	}

	/**
	 * Uses a serializer to allow implementing savegame
	 * loading and saving using a single method
	 */
	Common::Error syncGame(Common::Serializer &s);

	Common::Error saveGameStream(Common::WriteStream *stream, bool isAutosave = false) override {
		Common::Serializer s(nullptr, stream);
		return syncGame(s);
	}
	Common::Error loadGameStream(Common::SeekableReadStream *stream) override {
		Common::Serializer s(stream, nullptr);
		return syncGame(s);
	}

	void fadeIn();
	void fadeOut();
private:
	void fadeInner(int startValue, int endValue, int increment);
	void gameloop();
	void updateEvents();
	void handleInput();
	void wait();
	void handlePointerAction();
	void changeToRoom(int newRoomNumber);
};

extern DarkseedEngine *g_engine;
#define SHOULD_QUIT ::Darkseed::g_engine->shouldQuit()

} // End of namespace Darkseed

#endif // DARKSEED_H
