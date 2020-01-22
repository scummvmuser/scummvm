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

#include "common/scummsys.h"
#include "common/textconsole.h"
#include "sci/sci.h"
#include "sci/resource.h"
#include "sci/resource_patcher.h"

namespace Sci {

// Start of internal resource patcher macros. Please do not use these directly
// in resource patches.
// NOTE: The following breaks in non-C++11 compilers. It can be used to simplify the
// _BYTEOP(), and consequently the REPLACE() and INSERT() macros below.
//using int_c_array = int[];
//#define _NUMARGS(...) (sizeof(int_c_array{ __VA_ARGS__ }) / sizeof(int))
#ifdef SCUMM_LITTLE_ENDIAN
#define _PACKINT32(n) (((uint32)n) & 0xFF), (((uint32)n) >> 8 & 0xFF), (((uint32)n) >> 16 & 0xFF), (((uint32)n) >> 24 & 0xFF)
#else
#define _PACKINT32(n) (((uint32)n) >> 24 & 0xFF), (((uint32)n) >> 16 & 0xFF), (((uint32)n) >> 8 & 0xFF), (((uint32)n) & 0xFF)
#endif
#define _BYTEOP(op, numBytes, ...) op, _PACKINT32(numBytes), __VA_ARGS__
#define _NUMBEROP(op, type, value) op, sizeof(type), _PACKINT32(value)
#define _FILLOP(op, numBytes, value) op, _PACKINT32(numBytes), value
// End of internal resource patcher macros

/**
 * Advances the current position by `numBytes` bytes without changing any data.
 */
#define SKIP(numBytes) kSkipBytes, _PACKINT32(numBytes)

/**
 * Replaces data at the current position.
 */
#define REPLACE(numBytes, ...) _BYTEOP(kReplaceBytes, numBytes, __VA_ARGS__)

/**
 * Inserts new data at the current position.
 */
#define INSERT(numBytes, ...) _BYTEOP(kInsertBytes, numBytes, __VA_ARGS__)

/**
 * Replaces a number of the given type at the current position with the given
 * value.
 */
#define REPLACE_NUMBER(type, value) _NUMBEROP(kReplaceNumber, type, value)

/**
 * Adjusts a number of the given type at the current position by the given
 * delta.
 */
#define ADJUST_NUMBER(type, delta) _NUMBEROP(kAdjustNumber, type, delta)

/**
 * Inserts a number of the given type at the current position with the given
 * value.
 */
#define INSERT_NUMBER(type, value) _NUMBEROP(kInsertNumber, type, value)

/**
 * Replaces N bytes at the current position with the given value.
 */
#define REPLACE_FILL(value, numBytes) _FILLOP(kReplaceFill, numBytes, value)

/**
 * Inserts N bytes at the current position with the given value.
 */
#define INSERT_FILL(value, numBytes) _FILLOP(kInsertFill, numBytes, value)

/**
 * A required marker indicating that the end of the patch data has been reached
 * and no new patch operations will occur.
 */
#define END kEndOfPatch

#pragma mark -
#pragma mark Phantasmagoria

// Phantasmagoria view 64001 contains a bad palette that overwrites parts of the
// palette used by the background picture in room 6400, causing the black
// shadows to become tan, and many of the other background colors to end up a
// little bit off. View 64001 renders fine using the existing palette created by
// the background image, so just disable the embedded palette.
static const byte phant1View64001Palette[] = {
	SKIP(8),
	REPLACE_NUMBER(uint32, 0),
	END
};

#pragma mark -
#pragma mark Police Quest 4

// Police Quest 4 can support speech+subtitles mode but it includes no view that
// can be used to show that this mode is active in the UI, so we have to add our
// own.
static const byte pq4EnhancedAudioToggleView[] = {
	INSERT_NUMBER(uint16,  16), // header size
	INSERT_NUMBER(uint8,    1), // loop count
	INSERT(2, 0x00, 0x01),      // unused
	INSERT_NUMBER(uint8,    0), // resolution flag
	INSERT_NUMBER(uint16,   0), // unused
	INSERT_NUMBER(uint32,  70), // palette offset
	INSERT_NUMBER(uint8,   16), // loop header size
	INSERT_NUMBER(uint8,   36), // cel header size
	INSERT_NUMBER(uint16, 640), // x-resolution
	INSERT_NUMBER(uint16, 480), // y-resolution
	INSERT_NUMBER(int8,    -1), // alternate loop header
	INSERT_NUMBER(uint8,    0), // mirror flag
	INSERT_NUMBER(uint8,    1), // cel count
	INSERT_NUMBER(int32,   -1), // unused
	INSERT_NUMBER(uint8,    0), // unused
	INSERT_NUMBER(uint32,   0), // unused
	INSERT_NUMBER(uint32,  34), // cel header offset
	INSERT_NUMBER(uint16,  85), // width
	INSERT_NUMBER(uint16,  23), // height
	INSERT_NUMBER(int16,   42), // x-origin
	INSERT_NUMBER(int16,   22), // y-origin
	INSERT_NUMBER(uint8,  255), // transparent color
	INSERT_NUMBER(uint8,    0), // compression type (none)
	INSERT_NUMBER(uint16,   0), // transparency/remap flags
	INSERT_NUMBER(uint32,   0), // unused
	INSERT_NUMBER(uint32,   0), // unused
	INSERT_NUMBER(uint32,   0), // unused
	INSERT_NUMBER(uint32, 0x46b), // data offset
	INSERT_NUMBER(uint32,   0), // unused (for compressed data)
	INSERT_NUMBER(uint32,   0), // unused (for compressed data)

	// palette data
	INSERT_NUMBER(uint8,   14), // magic number
	INSERT_FILL(0x00, 9),       // garbage
	INSERT_NUMBER(uint8,    1), // number of palettes
	INSERT_NUMBER(uint16,   0), // garbage
	INSERT_NUMBER(uint8,   22), // first palette offset
	INSERT_FILL(0x00, 11),      // garbage
	INSERT_NUMBER(uint8,    0), // start color
	INSERT_FILL(0x00, 3),       // garbage
	INSERT_NUMBER(uint16, 256), // number of colors
	INSERT_NUMBER(uint8,    1), // used
	INSERT_NUMBER(uint8,    0), // shared used
	INSERT_NUMBER(uint32,   0), // version
	INSERT(152,                 // color data
	0x01, 0x00, 0x00, 0x00, 0x01, 0x1B, 0x1B, 0x1B, 0x01, 0x2B,
	0x2F, 0x2B, 0x01, 0x33, 0x33, 0x33, 0x01, 0x37, 0x3B, 0x37,
	0x01, 0x47, 0x47, 0x47, 0x01, 0x4B, 0x4B, 0x4B, 0x01, 0x53,
	0x57, 0x53, 0x01, 0x63, 0x67, 0x63, 0x01, 0x6B, 0x6B, 0x6B,
	0x01, 0x6F, 0x77, 0x6F, 0x01, 0x7B, 0x7F, 0x7B, 0x01, 0x93,
	0x9B, 0x93, 0x01, 0xAF, 0xB3, 0xAB, 0x01, 0x0F, 0x17, 0x3F,
	0x01, 0x1F, 0x27, 0x57, 0x01, 0x2B, 0x43, 0x6F, 0x01, 0x5B,
	0x87, 0xA7, 0x01, 0x63, 0x3B, 0x1B, 0x01, 0x97, 0x63, 0x3F,
	0x01, 0xCB, 0x7B, 0x4B, 0x01, 0xE3, 0xA3, 0x63, 0x01, 0x00,
	0xAF, 0x27, 0x01, 0x00, 0x87, 0x27, 0x01, 0x00, 0x5F, 0x23,
	0x01, 0x8B, 0x6B, 0x53, 0x01, 0xAB, 0x87, 0x67, 0x01, 0xC7,
	0xA3, 0x73, 0x01, 0xEF, 0xDB, 0x9B, 0x01, 0x57, 0x2B, 0x1F,
	0x01, 0x7F, 0x27, 0x1F, 0x01, 0x8F, 0x3F, 0x33, 0x01, 0xBB,
	0x3F, 0x33, 0x01, 0xCB, 0x4F, 0x33, 0x01, 0x07, 0x07, 0xCB,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF,
	0xFF, 0xFF
	),
	INSERT_FILL(0x00, 872),     // unused color entries

	// pixel data
	INSERT(1955,
	0x07, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x07, 0x05, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x04, 0x16, 0x16, 0x04, 0x04, 0x16, 0x16, 0x16, 0x04,
	0x04, 0x16, 0x16, 0x16, 0x04, 0x16, 0x16, 0x16, 0x04, 0x04,
	0x16, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x05, 0x05, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x16, 0x16, 0x16, 0x04, 0x16, 0x16, 0x16,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x16, 0x16, 0x16,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16,
	0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04,
	0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x16,
	0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x05, 0x05, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x04, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x16, 0x16, 0x16, 0x04, 0x04, 0x16, 0x16, 0x16, 0x04,
	0x16, 0x16, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x16, 0x16, 0x16, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x16, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04,
	0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x05, 0x05, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04,
	0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x16,
	0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x05, 0x05, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x16, 0x04, 0x04, 0x16, 0x04, 0x04,
	0x04, 0x04, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04, 0x16, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x16, 0x16, 0x04,
	0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x16, 0x16, 0x16, 0x04,
	0x16, 0x16, 0x16, 0x04, 0x04, 0x16, 0x16, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x16,
	0x04, 0x04, 0x16, 0x16, 0x16, 0x04, 0x16, 0x04, 0x04, 0x04,
	0x16, 0x04, 0x04, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x05, 0x05, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x00, 0x07, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x07, 0x05, 0x04, 0x00,
	0x07, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x07, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x00, 0x04, 0x04, 0x07, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x07, 0x04, 0x07, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x07,
	0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x04, 0x00, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x04, 0x04,
	0x05, 0x04, 0x04, 0x07, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x07, 0x04, 0x04, 0x05, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04
	),
	END
};

#pragma mark -
#pragma mark Patch table

static const GameResourcePatch resourcePatches[] = {
	{ GID_PHANTASMAGORIA, Common::UNK_LANG, ResourceId(kResourceTypeView, 64001), phant1View64001Palette,     false },
	{ GID_PQ4,            Common::EN_ANY,   ResourceId(kResourceTypeView, 10988), pq4EnhancedAudioToggleView, true }
};

#pragma mark -
#pragma mark ResourcePatcher

ResourcePatcher::ResourcePatcher(const SciGameId gameId, const Common::Language gameLanguage) :
	ResourceSource(kSourceScummVM, "-scummvm-") {
	for (int i = 0; i < ARRAYSIZE(resourcePatches); ++i) {
		const GameResourcePatch &patch = resourcePatches[i];
		if (patch.gameId == gameId &&
			(patch.gameLanguage == Common::UNK_LANG || patch.gameLanguage == gameLanguage)) {
			_patches.push_back(patch);
		}
	}
}

bool ResourcePatcher::applyPatch(Resource &resource) const {
	PatchList::const_iterator it;
	for (it = _patches.begin(); it != _patches.end(); ++it) {
		if (it->resourceId == resource._id) {
			debugC(kDebugLevelPatcher, "Applying resource patch to %s", resource._id.toString().c_str());
			patchResource(resource, *it);
			return true;
		}
	}

	return false;
}

void ResourcePatcher::scanSource(ResourceManager *resMan) {
	PatchList::const_iterator it;
	for (it = _patches.begin(); it != _patches.end(); ++it) {
		if (it->isNewResource && !resMan->testResource(it->resourceId)) {
			// Unlike other resources, ResourcePatcher does not have any files
			// to open to retrieve its resources, so the resource has to get
			// created and added manually instead of going through
			// `ResourceManager::addResource` or else the file validation will
			// blow up.
			Resource *res = new Resource(resMan, it->resourceId);
			res->_status = kResStatusNoMalloc;
			res->_source = this;
			res->_headerSize = 0;
			res->_fileOffset = 0;
			res->_size = 0;
			resMan->_resMap.setVal(it->resourceId, res);
		}
	}
}

void ResourcePatcher::patchResource(Resource &resource, const GameResourcePatch &patch) const {
	const byte *oldData;
	const byte *source = resource.data();
	byte *target;

	// New resources that came from ResourcePatcher need to get allocated or
	// else they will keep getting patched over themselves
	if (resource._source == this) {
		if (resource._status != kResStatusNoMalloc) {
			return;
		}

		resource._status = kResStatusAllocated;
	}

	const PatchSizes size = calculatePatchSizes(patch.patchData);
	if (size.expected > resource.size()) {
		warning("Unable to apply patch %s: patch expects at least %u bytes but resource is only %u bytes", patch.resourceId.toString().c_str(), size.expected, resource.size());
		return;
	}

	if (size.delta != 0) {
		// In the future it should be possible to have a negative size delta for
		// resources that need to be truncated, but for now just keep it
		// positive until there's a need for truncation
		assert(size.delta > 0);

		const int32 newSize = resource.size() + size.delta;
		assert(newSize > 0);

		target = new byte[newSize];
		assert(target);

		oldData = resource._data;
		resource._data = target;
		resource._size = newSize;
	} else {
		target = const_cast<byte *>(source);
		oldData = nullptr;
	}

	const byte *patchData = patch.patchData;
	ResourcePatchOp op;
	while ((op = static_cast<ResourcePatchOp>(*patchData++)) != kEndOfPatch) {
		switch (op) {
		case kSkipBytes: {
			const int32 skipSize = readBlockSize(patchData);
			if (target != source) {
				memcpy(target, source, skipSize);
			}
			source += skipSize;
			target += skipSize;
			break;
		}
		case kReplaceBytes:
		case kInsertBytes: {
			const int32 blockSize = readBlockSize(patchData);
			memcpy(target, patchData, blockSize);
			patchData += blockSize;
			if (op == kReplaceBytes) {
				source += blockSize;
			}
			target += blockSize;
			break;
		}
		case kReplaceNumber:
		case kAdjustNumber:
		case kInsertNumber: {
			const uint8 width = *patchData++;
			assert(width == 1 || width == 2 || width == 4);

			int32 value = *reinterpret_cast<const int32 *>(patchData);
			switch (width) {
			case 1:
				if (op == kAdjustNumber) {
					value += static_cast<int8>(*source);
				}
				assert(value >= -128 && value <= 255);
				*target = value;
				break;
			case 2:
				if (op == kAdjustNumber) {
					value += static_cast<int16>(READ_SCI11ENDIAN_UINT16(source));
				}
				assert(value >= -32768 && value <= 65535);
				WRITE_SCI11ENDIAN_UINT16(target, value);
				break;
			case 4:
				if (op == kAdjustNumber) {
					value += static_cast<int32>(READ_SCI11ENDIAN_UINT32(source));
				}
				WRITE_SCI11ENDIAN_UINT32(target, value);
				break;
			default:
				break;
			}

			patchData += sizeof(int32);
			if (op != kInsertNumber) {
				source += width;
			}
			target += width;
			break;
		}
		case kReplaceFill:
		case kInsertFill: {
			const int32 blockSize = readBlockSize(patchData);
			const byte value = *patchData++;
			memset(target, value, blockSize);
			if (op != kInsertFill) {
				source += blockSize;
			}
			target += blockSize;
			break;
		}
		default:
			error("Invalid control code %02x in patch data", op);
		}
	}

	if (target != source) {
		memcpy(target, source, resource._size - (target - resource._data));
	}

	delete[] oldData;
}

ResourcePatcher::PatchSizes ResourcePatcher::calculatePatchSizes(const byte *patchData) const {
	int32 deltaSize = 0;
	uint32 dataSize = 0;

	ResourcePatchOp op;
	while ((op = static_cast<ResourcePatchOp>(*patchData++)) != kEndOfPatch) {
		switch (op) {
		case kSkipBytes:
		case kReplaceBytes:
		case kInsertBytes: {
			const int32 blockSize = readBlockSize(patchData);

			if (op == kReplaceBytes || op == kInsertBytes) {
				patchData += blockSize;
			}

			if (op == kInsertBytes) {
				deltaSize += blockSize;
			} else {
				dataSize += blockSize;
			}

			break;
		}
		case kReplaceNumber:
		case kAdjustNumber:
		case kInsertNumber: {
			const uint8 width = *patchData++;
			assert(width == 1 || width == 2 || width == 4);

			if (op == kInsertNumber) {
				deltaSize += width;
			} else {
				dataSize += width;
			}

			patchData += /* value */ sizeof(int32);
			break;
		}
		case kReplaceFill:
		case kInsertFill: {
			const int32 blockSize = readBlockSize(patchData);
			/* value */ ++patchData;

			if (op == kInsertFill) {
				deltaSize += blockSize;
			} else {
				dataSize += blockSize;
			}

			break;
		}
		default:
			error("Invalid control code %02x in patch data", op);
		}
	}

	return PatchSizes(dataSize, deltaSize);
}

int32 ResourcePatcher::readBlockSize(const byte * &patchData) const {
	const int32 blockSize = *reinterpret_cast<const int32 *>(patchData);
	assert(blockSize >= 1);
	patchData += sizeof(int32);
	return blockSize;
}
} // End of namespace Sci
