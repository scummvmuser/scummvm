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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ENGINES_ADVANCED_DETECTOR_H
#define ENGINES_ADVANCED_DETECTOR_H

#include "engines/metaengine.h"

namespace Common {
class Error;
class FSList;
}


struct ADGameFileDescription {
	const char *fileName;
	uint16 fileType; // Optional. Not used during detection, only by engines.
	const char *md5; // Optional. May be NULL.
	int32 fileSize;  // Optional. Set to -1 to ignore.
};

#define AD_LISTEND {NULL, 0, NULL, 0}

#define AD_ENTRY1(f, x) {{ f, 0, x, -1}, AD_LISTEND}
#define AD_ENTRY1s(f, x, s) {{ f, 0, x, s}, AD_LISTEND}

enum ADGameFlags {
	ADGF_NO_FLAGS = 0,
	ADGF_PIRATED = (1 << 23), // flag to designate well known pirated versions with cracks
	ADGF_ADDENGLISH = (1 << 24), // always add English as language option
	ADGF_MACRESFORK = (1 << 25), // the md5 for this entry will be calculated from the resource fork
	ADGF_USEEXTRAASTITLE = (1 << 26), // Extra field value will be used as main game title, not gameid
	ADGF_DROPLANGUAGE = (1 << 28), // don't add language to gameid
	ADGF_CD = (1 << 29),    	// add "-cd" to gameid
	ADGF_DEMO = (1 << 30)   	// add "-demo" to gameid
};

struct ADGameDescription {
	const char *gameid;
	const char *extra;
	ADGameFileDescription filesDescriptions[14];
	Common::Language language;
	Common::Platform platform;

	/**
	 * A bitmask of extra flags. The top 16 bits are reserved for generic flags
	 * defined in the ADGameFlags. This leaves 16 bits to be used by client
	 * code.
	 */
	uint32 flags;

	uint32 guioptions;
};

/**
 * A list of pointers to ADGameDescription structs (or subclasses thereof).
 */
typedef Common::Array<const ADGameDescription *> ADGameDescList;

/**
 * End marker for a table of ADGameDescription structs. Use this to
 * terminate a list to be passed to the AdvancedDetector API.
 */
#define AD_TABLE_END_MARKER	\
	{ NULL, NULL, { { NULL, 0, NULL, 0 } }, Common::UNK_LANG, Common::kPlatformUnknown, ADGF_NO_FLAGS, Common::GUIO_NONE }

struct ADFileBasedFallback {
	/**
	 * Pointer to an ADGameDescription or subclass thereof which will get
	 * returned if there's a detection match.
	 */
	const void *desc;

	/**
	 * A zero-terminated list of filenames used for matching. All files in
	 * the list must be present to get a detection match.
	 */
	const char *filenames[10];
};


enum ADFlags {
	/**
	 * Warn user about new variant if his version was detected with fallback
	 */
	kADFlagPrintWarningOnFileBasedFallback = (1 << 1),
	/**
	 * Store value of extra field in config file, and use it as a hint
	 * on subsequent runs. Could be used when there is no way to autodetect
	 * game (when more than one game sits in same directory), and user picks
	 * up a variant manually.
	 */
	kADFlagUseExtraAsHint = (1 << 2)
};


/**
 * A MetaEngine implementation based around the advanced detector code.
 */
class AdvancedMetaEngine : public MetaEngine {
protected:
	/**
	 * Pointer to an array of objects which are either ADGameDescription
	 * or superset structures (i.e. start with an ADGameDescription member.
	 * The list is terminated by an entry with a gameid equal to 0
	 * (see AD_TABLE_END_MARKER).
	 */
	const byte *_gameDescriptors;

	/**
	 * The size of a single entry of the above descs array. Always
	 * must be >= sizeof(ADGameDescription).
	 */
	uint _descItemSize;

	/**
	 * The number of bytes to compute MD5 sum for. The AdvancedDetector
	 * is primarily based on computing and matching MD5 checksums of files.
	 * Since doing that for large files can be slow, it can be restricted
	 * to a subset of all files.
	 * Typically this will be set to something between 5 and 50 kilobyte,
	 * but arbitrary non-zero values are possible.
	 */
	uint _md5Bytes;

	/**
	 * A list of all gameids (and their corresponding descriptions) supported
	 * by this engine.
	 */
	const PlainGameDescriptor *_gameids;

	/**
	 * Name of single gameid (optional).
	 *
	 * @todo Properly explain this -- what does it do?
	 */
	const char *_singleid;

	/**
	 * List of files for file-based fallback detection (optional).
	 * This is used if the regular MD5 based detection failed to
	 * detect anything.
	 * As usual this list is terminated by an all-zero entry.
	 *
	 * @todo Properly explain this
	 */
	const ADFileBasedFallback *_fileBasedFallback;

	/**
	 * A bitmask of flags which can be used to configure the behavior
	 * of the AdvancedDetector. Refer to ADFlags for a list of flags
	 * that can be ORed together and passed here.
	 */
	uint32 _flags;

	/**
	 * A bitmask of game GUI options which will be added to each
	 * entry in addition to per-game options. Refer to GameGUIOption
	 * enum for the list.
	 */
	uint32 _guioptions;

	/**
	 * Maximum depth of directories to look up.
	 * If set to 0, the depth is 1 level
	 */
	uint32 _maxScanDepth;

	/**
	 * Case-insensitive list of directory globs which could be used for
	 * going deeper into the directory structure.
	 * @see String::matchString() method for format description.
	 *
	 * @note Last item must be 0
	 */
	const char * const *_directoryGlobs;

public:
	AdvancedMetaEngine(const void *descs, uint descItemSize, const PlainGameDescriptor *gameids);

	/**
	 * Returns list of targets supported by the engine.
	 * Distinguishes engines with single ID
	 */
	virtual GameList getSupportedGames() const;

	virtual GameDescriptor findGame(const char *gameid) const;

	virtual GameList detectGames(const Common::FSList &fslist) const;

	virtual Common::Error createInstance(OSystem *syst, Engine **engine) const;

protected:
	// To be implemented by subclasses
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const = 0;

	/**
	 * An (optional) generic fallback detect function which is invoked
	 * if both the regular MD5 based detection as well as the file
	 * based fallback failed to detect anything.
	 */
	virtual const ADGameDescription *fallbackDetect(const Common::FSList &fslist) const {
		return 0;
	}

protected:
	typedef Common::HashMap<Common::String, Common::FSNode, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> FileMap;

	/**
	 * Detect games in specified directory.
	 * Parameters language and platform are used to pass on values
	 * specified by the user. I.e. this is used to restrict search scope.
	 *
	 * @param fslist	FSList to scan or NULL for scanning all specified
	 *					default directories.
	 * @param language	restrict results to specified language only
	 * @param platform	restrict results to specified platform only
	 * @return	list of ADGameDescription (or subclass) pointers corresponding to matched games
	 */
	ADGameDescList detectGame(const Common::FSList &fslist, Common::Language language, Common::Platform platform, const Common::String &extra) const;

	/**
	 * Check for each ADFileBasedFallback record whether all files listed
	 * in it are present. If multiple pass this test, we pick the one with
	 * the maximal number of matching files. In case of a tie, the entry
	 * coming first in the list is chosen.
	 */
	ADGameDescList detectGameFilebased(const FileMap &allFiles) const;

	// TODO
	void updateGameDescriptor(GameDescriptor &desc, const ADGameDescription *realDesc) const;

	/**
	 * Compose a hashmap of all files in fslist.
	 * Includes nifty stuff like removing trailing dots and ignoring case.
	 */
	void composeFileHashMap(const Common::FSList &fslist, FileMap &allFiles, int depth) const;
};

#endif
