
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

#ifndef BAGEL_BAGLIB_INV_H
#define BAGEL_BAGLIB_INV_H

#include "bagel/baglib/storage_dev_bmp.h"

namespace Bagel {

class CBagInv : public CBagStorageDevBmp {
protected:
	static bool m_bFirstStash;

public:
	CBagInv() : CBagStorageDevBmp() {}
	virtual ~CBagInv() {}
	static void initialize() {
		m_bFirstStash = true;
	}

	ERROR_CODE ActivateLocalObject(const CBofString &sName);

	/**
	 * This is called when a object is removed from the inventory
	 * The wand on the PDA should animate
	 */
	ERROR_CODE DeactivateLocalObject(const CBofString &sName);
};

} // namespace Bagel

#endif
