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

#include "common/config-manager.h"
#include "common/system.h"
#include "common/zlib.h"

#include "common/memstream.h"
#include "common/macresman.h"

#include "graphics/fonts/macfont.h"
#include "graphics/macgui/macfontmanager.h"
#include "graphics/macgui/macwindowmanager.h"

#include "engines/util.h"

#include "director/director.h"
#include "director/archive.h"
#include "director/movie.h"
#include "director/score.h"
#include "director/lingo/lingo.h"

namespace Director {

//////////////////////
// Graphics tests
//////////////////////
void DirectorEngine::testFontScaling() {
	int x = 10;
	int y = 10;
	int w = 640;
	int h = 480;

	initGraphics(w, h);
	setPalette(-1);

	Graphics::ManagedSurface surface;

	surface.create(w, h);
	surface.clear(255);

	Graphics::MacFont origFont(Graphics::kMacFontNewYork, 18);

	const Graphics::MacFONTFont *font1 = (const Graphics::MacFONTFont *)_wm->_fontMan->getFont(origFont);

	Graphics::MacFONTFont::testBlit(font1, &surface, 0, x, y + 200, 500);

	Graphics::MacFont bigFont(Graphics::kMacFontNewYork, 15);

	font1 = (const Graphics::MacFONTFont *)_wm->_fontMan->getFont(bigFont);

	Graphics::MacFONTFont::testBlit(font1, &surface, 0, x, y + 50 + 200, 500);

	const char *text = "d";

	for (int i = 9; i <= 20; i++) {
		Graphics::MacFont macFont(Graphics::kMacFontNewYork, i);

		const Graphics::Font *font = _wm->_fontMan->getFont(macFont);

		int width = font->getStringWidth(text);

		Common::Rect bbox = font->getBoundingBox(text, x, y, w);
		surface.frameRect(bbox, 15);

		font->drawString(&surface, text, x, y, width, 0);

		x += width + 1;
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int y1 = 80 + i * 7;
			int x1 = 80 + j * 7;

			for (x = x1; x < x1 + 6; x++)
				for (y = y1; y < y1 + 6; y++)
					*((byte *)surface.getBasePtr(x, y)) = transformColor(i * 16 + j);
		}
	}

	g_system->copyRectToScreen(surface.getPixels(), surface.pitch, 0, 0, w, h); // testing fonts

	Common::Event event;

	while (true) {
		if (g_system->getEventManager()->pollEvent(event))
			if (event.type == Common::EVENT_QUIT)
				break;

		g_system->updateScreen();
		g_system->delayMillis(10);
	}
}

void DirectorEngine::testFonts() {
	Common::String fontName("Helvetica");

	Common::MacResManager *fontFile = new Common::MacResManager();
	if (!fontFile->open(fontName))
		error("testFonts(): Could not open %s as a resource fork", fontName.c_str());

	Common::MacResIDArray fonds = fontFile->getResIDArray(MKTAG('F','O','N','D'));
	if (fonds.size() > 0) {
		for (Common::Array<uint16>::iterator iterator = fonds.begin(); iterator != fonds.end(); ++iterator) {
			Common::SeekableReadStream *stream = fontFile->getResource(MKTAG('F', 'O', 'N', 'D'), *iterator);
			Common::String name = fontFile->getResName(MKTAG('F', 'O', 'N', 'D'), *iterator);

			debug("Font: %s", name.c_str());

			Graphics::MacFontFamily font;
			font.load(*stream);
		}
	}

	delete fontFile;
}

//////////////////////
// Movie iteration
//////////////////////
Common::HashMap<Common::String, Movie *> *DirectorEngine::scanMovies(const Common::String &folder) {
	Common::FSNode directory(folder);
	Common::FSList movies;
	const char *sharedMMMname;

	if (getPlatform() == Common::kPlatformWindows)
		sharedMMMname = "SHARDCST.MMM";
	else
		sharedMMMname = "Shared Cast";


	Common::HashMap<Common::String, Movie *> *nameMap = new Common::HashMap<Common::String, Movie *>();
	if (!directory.getChildren(movies, Common::FSNode::kListFilesOnly))
		return nameMap;

	if (!movies.empty()) {
		for (Common::FSList::const_iterator i = movies.begin(); i != movies.end(); ++i) {
			debugC(2, kDebugLoading, "File: %s", i->getName().c_str());

			if (Common::matchString(i->getName().c_str(), sharedMMMname, true)) {
				_sharedCastFile = i->getName();

				debugC(2, kDebugLoading, "Shared cast detected: %s", i->getName().c_str());
				continue;
			}

			Archive *arc = createArchive();

			warning("name: %s", i->getName().c_str());
			arc->openFile(i->getName());
			Movie *m = new Movie(this);
			m->setArchive(arc);
			nameMap->setVal(m->getMacName(), m);

			debugC(2, kDebugLoading, "Movie name: \"%s\"", m->getMacName().c_str());
		}
	}

	return nameMap;
}

void DirectorEngine::enqueueAllMovies() {
	Common::FSNode dir(ConfMan.get("path"));
	Common::FSList files;
	if (!dir.getChildren(files, Common::FSNode::kListFilesOnly)) {
		warning("DirectorEngine::enqueueAllMovies(): Failed inquiring file list");
		return;
	}

	for (Common::FSList::const_iterator file = files.begin(); file != files.end(); ++file)
		_movieQueue.push_back((*file).getName());

	Common::sort(_movieQueue.begin(), _movieQueue.end());

	debug(1, "=========> Enqueued %d movies", _movieQueue.size());
}

MovieReference DirectorEngine::getNextMovieFromQueue() {
	MovieReference res;

	if (_movieQueue.empty())
		return res;

	res.movie = _movieQueue.front();

	debug(0, "=======================================");
	debug(0, "=========> Next movie is %s", res.movie.c_str());
	debug(0, "=======================================");

	_movieQueue.remove_at(0);

	return res;
}

const byte testMovie[] = {
	0x1F, 0x8B, 0x08, 0x08, 0x2C, 0x1B, 0xD8, 0x5E, 0x02, 0x03, 0x74, 0x65,
	0x73, 0x74, 0x31, 0x00, 0xB5, 0x54, 0xCD, 0x6B, 0x13, 0x51, 0x10, 0x9F,
	0xFD, 0x48, 0xB3, 0x6D, 0xA3, 0xA6, 0x36, 0x16, 0x0B, 0x2A, 0x1B, 0x28,
	0x52, 0x4A, 0xB5, 0x7E, 0xA1, 0xB6, 0x14, 0x69, 0x0D, 0x0D, 0x29, 0x92,
	0x5A, 0x92, 0xB4, 0xD6, 0x0A, 0xD2, 0x42, 0xB7, 0x9F, 0x69, 0x36, 0xA4,
	0x69, 0x2D, 0xF4, 0xD0, 0xA5, 0x27, 0xF1, 0x54, 0xF0, 0xA0, 0x88, 0x37,
	0xC1, 0x83, 0x28, 0xD8, 0x9C, 0x3C, 0x49, 0x05, 0x0F, 0xFD, 0x23, 0x14,
	0x4F, 0x42, 0xF1, 0xA0, 0x5E, 0x04, 0x0F, 0x92, 0xF8, 0x7B, 0x1F, 0xBB,
	0x59, 0xCB, 0xEA, 0xCD, 0xD9, 0x4C, 0xE6, 0xB7, 0xBF, 0x9D, 0xF7, 0xDE,
	0xBC, 0x79, 0xF3, 0x26, 0x33, 0x9C, 0x9C, 0x20, 0x6A, 0xE8, 0x4B, 0x8F,
	0xF7, 0x5E, 0x5C, 0x58, 0x9E, 0x2E, 0x12, 0xD1, 0x71, 0xA8, 0x02, 0xED,
	0xA6, 0x03, 0xB2, 0xCC, 0xBF, 0x6B, 0x57, 0xE1, 0x11, 0xC3, 0x6B, 0x27,
	0xB4, 0xAD, 0x26, 0x05, 0xF8, 0x68, 0x46, 0xCE, 0xC5, 0x9D, 0x15, 0x31,
	0xC6, 0x37, 0x67, 0x84, 0x71, 0x5A, 0x65, 0x31, 0xE6, 0xCD, 0x23, 0xD7,
	0x00, 0x57, 0xBE, 0x31, 0x74, 0xBB, 0x0B, 0xF0, 0x17, 0xDE, 0xF2, 0xEE,
	0x7A, 0xE3, 0xB7, 0x12, 0x49, 0x98, 0x51, 0x22, 0xBD, 0xE8, 0x72, 0x89,
	0xC1, 0x2C, 0xF3, 0x33, 0xC0, 0xED, 0xBB, 0xDC, 0x6C, 0xC9, 0xB2, 0xBC,
	0x20, 0x23, 0x44, 0x2C, 0x9E, 0x83, 0x1C, 0xA4, 0x21, 0x80, 0x0B, 0x07,
	0x70, 0x46, 0xD6, 0x2E, 0xCD, 0x88, 0x98, 0xF5, 0xBD, 0x7A, 0x2C, 0xC9,
	0x61, 0x18, 0x9B, 0x28, 0x94, 0xAC, 0x73, 0xD9, 0x04, 0xCC, 0x35, 0x70,
	0x6F, 0xFF, 0x16, 0x0B, 0x24, 0x1A, 0xC0, 0x35, 0x62, 0xBE, 0xB4, 0x20,
	0x64, 0xBE, 0x82, 0xC7, 0x1E, 0x0E, 0xE0, 0x0E, 0x05, 0x70, 0x2D, 0xC8,
	0x4B, 0x19, 0xB6, 0x03, 0x31, 0xFF, 0x74, 0x3F, 0x65, 0x73, 0x13, 0x39,
	0x98, 0xB3, 0x58, 0x43, 0xF7, 0xE5, 0x8F, 0xF9, 0xB5, 0x23, 0xE6, 0x0E,
	0x97, 0xDB, 0xAC, 0xA9, 0x2B, 0xF4, 0x4E, 0x4B, 0x6C, 0xD6, 0x36, 0x6B,
	0xF1, 0xC8, 0x1F, 0x47, 0xBE, 0xAD, 0xED, 0x7C, 0x78, 0x2D, 0x0E, 0x53,
	0x6D, 0x81, 0x85, 0xD2, 0x96, 0xB6, 0xF3, 0x71, 0xDF, 0x68, 0x03, 0x3A,
	0x41, 0x5B, 0xA4, 0xCB, 0x53, 0xEA, 0x82, 0xCD, 0xC0, 0x5E, 0x96, 0x59,
	0xB9, 0x2E, 0x33, 0x36, 0x22, 0xA3, 0x98, 0xD4, 0x89, 0x57, 0x09, 0x17,
	0x89, 0x2F, 0xF9, 0xF0, 0x92, 0x0F, 0x3F, 0xE2, 0x58, 0xE1, 0xF8, 0x93,
	0x88, 0x50, 0x0E, 0x3C, 0x27, 0xE2, 0x09, 0x16, 0x87, 0x92, 0xE7, 0xD9,
	0xE3, 0xA3, 0x84, 0xBF, 0xA2, 0x55, 0xC6, 0xBA, 0xB5, 0x4A, 0xEE, 0x3E,
	0x74, 0xAF, 0x56, 0xC3, 0xFF, 0xCB, 0x17, 0x63, 0xE2, 0x89, 0x1B, 0x71,
	0xC3, 0xDE, 0xB0, 0x37, 0xA4, 0xFF, 0x00, 0xF3, 0xD7, 0xDE, 0xBC, 0x9F,
	0x72, 0x4C, 0x71, 0x76, 0x78, 0x5C, 0x49, 0x49, 0x65, 0x9C, 0x46, 0x21,
	0x51, 0xB9, 0xB9, 0xA7, 0xCC, 0x7A, 0xE3, 0xFF, 0x19, 0x9F, 0x57, 0xE3,
	0x11, 0x7E, 0x6A, 0xEC, 0x0E, 0x85, 0xA5, 0x6D, 0x95, 0x59, 0x0A, 0xB1,
	0x1C, 0xC8, 0x1A, 0x6F, 0x62, 0x58, 0xD6, 0xA2, 0xCE, 0x7E, 0x32, 0xD3,
	0xCD, 0x02, 0xF3, 0xEC, 0x1E, 0x91, 0x38, 0x2D, 0x2A, 0x41, 0x77, 0x6B,
	0xD2, 0x8F, 0x5B, 0x04, 0x2E, 0x2F, 0xFD, 0x0F, 0x5C, 0x6C, 0xEE, 0xA1,
	0xD4, 0xBA, 0xD2, 0x98, 0x2A, 0x54, 0xAB, 0x23, 0xAF, 0xB4, 0x27, 0x83,
	0x5F, 0xAB, 0xD5, 0x4C, 0xAA, 0xC7, 0x00, 0x9E, 0xBC, 0xF9, 0x8D, 0xA2,
	0x53, 0x44, 0x8F, 0xCD, 0x02, 0x19, 0xE6, 0xE8, 0xE9, 0x79, 0x32, 0x7A,
	0x9A, 0xBD, 0x3B, 0x3D, 0xAA, 0xDF, 0xA1, 0x6D, 0xDA, 0x55, 0x8B, 0xDA,
	0x00, 0xCA, 0x4B, 0x45, 0xC0, 0x1A, 0x2F, 0xB3, 0x0A, 0xCF, 0x30, 0xDB,
	0x72, 0x27, 0x3C, 0xDC, 0xEC, 0xEF, 0xF2, 0xFF, 0x1F, 0xD8, 0xAF, 0x32,
	0xC7, 0x3C, 0xFA, 0x2D, 0xE3, 0xF9, 0x2C, 0xA9, 0xF5, 0xDC, 0xD6, 0xEF,
	0xAC, 0x27, 0x2A, 0xD7, 0x18, 0x14, 0xF3, 0xD7, 0xFB, 0x06, 0xB5, 0x42,
	0x8F, 0x79, 0xF7, 0xE5, 0x14, 0x3F, 0x47, 0x39, 0xC4, 0xEB, 0x65, 0x04,
	0x3E, 0x4A, 0x6B, 0x7C, 0xFD, 0xA8, 0x77, 0x67, 0x5A, 0x49, 0x4C, 0xA1,
	0xD1, 0x14, 0x7D, 0xA7, 0x2F, 0xCA, 0x43, 0x52, 0x3E, 0x3B, 0x8A, 0x1A,
	0xF2, 0x75, 0x07, 0x7E, 0x9A, 0x52, 0x14, 0x47, 0xF4, 0x18, 0x5F, 0x4C,
	0x6C, 0xFD, 0x93, 0xD0, 0x2B, 0xD0, 0x5E, 0x57, 0xC3, 0x8B, 0x65, 0xF3,
	0x8C, 0xB9, 0x58, 0x8E, 0x0D, 0xAD, 0xCE, 0x59, 0x05, 0xCB, 0xCC, 0x4E,
	0x17, 0x66, 0x56, 0xF3, 0x56, 0x61, 0xC9, 0x06, 0xDD, 0x3E, 0x56, 0x58,
	0x58, 0xEF, 0x4B, 0xDB, 0x6B, 0x0B, 0xD6, 0x8A, 0x39, 0x6B, 0x97, 0xCC,
	0xC4, 0xFC, 0x74, 0xB1, 0x6C, 0x95, 0x56, 0xFA, 0xB0, 0xAD, 0x7A, 0x07,
	0xE2, 0x1A, 0xE3, 0x7D, 0x57, 0x87, 0xBD, 0x80, 0x54, 0x76, 0x21, 0xFA,
	0x4E, 0x0A, 0x3B, 0x0E, 0x2E, 0x86, 0x42, 0x77, 0x91, 0xF3, 0x26, 0x9A,
	0x00, 0xD7, 0x4F, 0x51, 0x8B, 0x54, 0x07, 0x79, 0x61, 0xBB, 0x78, 0x40,
	0xCF, 0xEA, 0x7D, 0x82, 0xD7, 0x25, 0xD3, 0xB6, 0x94, 0x95, 0xCF, 0xDB,
	0xE6, 0x3D, 0xBB, 0x94, 0x9F, 0x89, 0xCB, 0x5E, 0x1E, 0xC5, 0x17, 0xCD,
	0xEB, 0x38, 0x6E, 0xD5, 0xD1, 0x6F, 0xD2, 0x74, 0x2D, 0xFA, 0x42, 0x06,
	0x00, 0x00
};

void DirectorEngine::runTests() {
	Common::MemoryReadStream *movie = new Common::MemoryReadStream(testMovie, ARRAYSIZE(testMovie));
	Common::SeekableReadStream *stream = Common::wrapCompressedReadStream(movie);

	_mainArchive = new RIFXArchive();
	if (!_mainArchive->openStream(stream, 0)) {
		error("DirectorEngine::runTests(): Bad movie data");
	}
	_currentMovie = new Movie(this);
	_currentMovie->setArchive(_mainArchive);
	_currentMovie->loadArchive();

	if (debugChannelSet(-1, kDebugText)) {
		testFontScaling();
		testFonts();
	}

	_lingo->runTests();
}

} // End of namespace Director
