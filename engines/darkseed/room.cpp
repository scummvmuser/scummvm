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

#include "room.h"
#include "darkseed.h"

const static int roomDescriptionTextTbl[] = {
	0, 138, 165, 165,
	181, 243, 254, 292,
	369, 397, 420, 429,
	438, 447, 463, 473,
	502, 520, 546, 564,
	564, 564, 564, 0,
	569, 574, 574, 574,
	563, 0, 748, 739,
	738, 723, 704, 612,
	619, 0, 758, 0,
	770, 777, 779, 790,
	778, 801, 809, 0,
	810, 811, 814, 821,
	0, 828, 810, 810,
	850, 860, 867, 874,
	886, 651, 682, 701,
	702, 701, 898, 899,
	903};

Darkseed::Room::Room(int roomNumber) : _roomNumber(roomNumber) {
	room1.resize(8);
	walkableLocationsMap.resize(16);
	_roomObj.resize(30);

	_locObjFrame.resize(30);
	_locObjFrameTimer.resize(30);

	if(!load()) {
		error("Failed to load room %d", roomNumber);
	}
}

void Darkseed::Room::initRoom() {
	for (int iVar1 = 0; iVar1 < 30; iVar1 = iVar1 + 1) {
		_locObjFrame[iVar1] = 0;
		g_engine->animIndexTbl[iVar1] = 0;
	}
	if (g_engine->_objectVar[141] == 10) {
		g_engine->_objectVar[141] = 11;
	}
	if (g_engine->_objectVar[45] < 3) {
		g_engine->_objectVar.setMoveObjectX(19, 1000);
	}
	g_engine->_objectVar.setMoveObjectX(45, 230);
	if (g_engine->_objectVar[45] < 3) {
		g_engine->_objectVar[45] = 0;
		g_engine->_objectVar.setMoveObjectPosition(19, {230,205});
	}
	if (g_engine->_objectVar[141] == 8) {
		g_engine->_objectVar[141] = 7;
	}
}

bool Darkseed::Room::load() {
	Common::String filenameBase = getRoomFilenameBase(_roomNumber);
	Common::String romFilename;
	Common::File file;
	romFilename = g_engine->getRoomFilePath(Common::String::format("%s.rom", filenameBase.c_str(), _roomNumber));
	if(!file.open(romFilename)) {
		return false;
	}

	Common::String nspFilename = stripSpaces(file.readString());
	file.seek(0xd);
	Common::String picFilename = stripSpaces(file.readString());
	file.seek(0x1a);
	Common::String palFilename = stripSpaces(file.readString());
	file.seek(0x27);

	for (int i = 0; i < 8; i++) {
		room1[i].x = file.readUint16BE();
		room1[i].y = file.readUint16BE();
		if (room1[i].y > 233) {
			room1[i].y = 233;
		}
		room1[i].width = file.readUint16BE();
		room1[i].height = file.readUint16BE();
		room1[i].roomNumber = file.readUint16BE();
		room1[i].direction = file.readByte();
	}

	for (int i = 0; i < 16; i++) {
		file.read(walkableLocationsMap[i].strip, 40);
	}

	for (int i = 0; i < 30; i++) {
		_roomObj[i].type = file.readUint16BE();
		_roomObj[i].objNum = file.readUint16BE();
		_roomObj[i].xOffset = file.readUint16BE();
		_roomObj[i].yOffset = file.readUint16BE();
		_roomObj[i].width = file.readUint16BE();
		_roomObj[i].height = file.readUint16BE();
		_roomObj[i].depth = file.readByte();
		_roomObj[i].spriteNum = file.readByte();

		if (_roomObj[i].spriteNum >= 0x29 && _roomObj[i].type != 0 && _roomObj[i].type != 1000) {
			_roomObj[i].height = 0x14;
			_roomObj[i].width = 0x14;
			_roomObj[i].type = 0;
		}

		if (_roomObj[i].objNum == 0 && _roomObj[i].type == 1) {
			if (_connectors.size() == MAX_CONNECTORS) {
				error("Too many connectors in this room, max of %d", MAX_CONNECTORS);
			}
			Common::Point connector;
			connector.x = _roomObj[i].xOffset;
			connector.y = _roomObj[i].yOffset;

			if (connector.x > 565) {
				connector.x = 565;
			}
			if (connector.x < 75) {
				connector.x = 75;
			}
			if (connector.y > 235) {
				connector.y = 235;
			}
			if (connector.y < 45) {
				connector.y = 45;
			}

			debug("Room Connector: %d %d", connector.x, connector.y);
			_connectors.push_back(connector);
			_roomObj[i].type = 0xff;
		}
	}

	file.close();

	_collisionType = 0;
	if(!pic.load(picFilename)) {
		return false;
	}

	_pal.load(g_engine->getPictureFilePath(Common::String::format("%s.pal", filenameBase.c_str())));

	_locationSprites.load(Common::String::format("%s.nsp", filenameBase.c_str()));

	return true;
}

Common::String Darkseed::Room::stripSpaces(Common::String source) {
	Common::String out;
	const char *src = source.c_str();
	for (int i = 0; i < source.size(); i++) {
		if (src[i] != ' ') {
			out += src[i];
		}
	}
	return out;
}

void Darkseed::Room::draw() {
	pic.draw(0x45, 0x28);

	// print walkable area map.
	if (g_engine->_debugShowWalkPath) {
		for (int y = 0x28; y < pic.getHeight() + 0x28; y++) {
			for (int x = 0x45; x < pic.getWidth() + 0x45; x++) {
				if (canWalkAtLocation(x, y)) {
					g_engine->_screen->drawLine(x, y, x, y, 14);
				}
			}
		}

		for (int i = 0; i < _connectors.size(); i++) {
			g_engine->_baseSprites.getSpriteAt(0).draw(_connectors[i].x, _connectors[i].y);
		}
	}
}

int Darkseed::Room::checkCursorAndMoveableObjects() {
	int actionMode = g_engine->_actionMode;
	const Sprite &cursorSprite = (actionMode == LookAction)
									 ? g_engine->_cursor.getSpriteForType(ExclamationMark)
									 : g_engine->_cursor.getSprite();
	bool hasObject = false;
	int objNum = -1;
	_collisionType = 0;
	for (int i = 0; i < _roomObj.size(); i++) {
		if ((_roomObj[i].type == 1 || _roomObj[i].type == 3)
			&& _roomObj[i].xOffset <= cursorSprite.width + g_engine->_cursor.getX()
			&& g_engine->_cursor.getX() <= _roomObj[i].width + _roomObj[i].xOffset
			&& _roomObj[i].yOffset <= cursorSprite.height + g_engine->_cursor.getY()
			&& g_engine->_cursor.getY() <= _roomObj[i].height + _roomObj[i].yOffset
		) {
			if (_roomObj[i].objNum == 25) {
				if (g_engine->_objectVar.getVar(80) < 3) {
					hasObject = false;
				} else {
					hasObject = true;
				}
			}

			if (_roomObj[i].objNum == 14 && g_engine->_cursor.getY() > 40 && g_engine->_objectVar.getVar(86) == 0) {
				hasObject = false;
			}

			if (hasObject) {
				objNum = i;
			}
		}
	}
	return hasObject ? objNum : -1;
}

int Darkseed::Room::checkCursorAndStaticObjects(int x, int y) {
	int actionMode = g_engine->_actionMode;
	const Sprite &cursorSprite = (actionMode == LookAction)
									 ? g_engine->_cursor.getSpriteForType(ExclamationMark)
									 : g_engine->_cursor.getSprite();
	bool hasObject = false;
	_collisionType = 0;
	for (int i = 0; i < _roomObj.size(); i++) {
		if (_roomObj[i].type == 0
			&& _roomObj[i].xOffset <= cursorSprite.width + g_engine->_cursor.getX()
			&& g_engine->_cursor.getX() <= _roomObj[i].width + _roomObj[i].xOffset
			&& _roomObj[i].yOffset <= cursorSprite.height + g_engine->_cursor.getY()
			&& g_engine->_cursor.getY() <= _roomObj[i].height + _roomObj[i].yOffset
		) {
			if (actionMode != PointerAction && _roomObj[i].objNum >= 5) {
				hasObject = true;
			}

			if (actionMode == PointerAction && _roomObj[i].objNum < 6) {
				hasObject = true;
			}

			if (_roomObj[i].objNum == 59 || _roomObj[i].objNum == 78) {
				if (g_engine->_objectVar.getVar(34) == 1) {
					hasObject = true;
				} else {
					hasObject = false;
				}
			}

			if (_roomObj[i].objNum == 0x19 && hasObject) {
				if (g_engine->_objectVar.getVar(80) < 2) {
					hasObject = false;
				} else {
					hasObject = true;
				}
			}

			if (_roomObj[i].objNum == 0x74 && hasObject && (int)actionMode != 0x13) {
				hasObject = false;
			}

			if (hasObject) {
				return i;
			}
		}
	}
	return -1;
}

int Darkseed::Room::CheckCursorAndMovedObjects() {
	int actionMode = g_engine->_actionMode;
	const Sprite &cursorSprite = (actionMode == LookAction)
									 ? g_engine->_cursor.getSpriteForType(ExclamationMark)
									 : g_engine->_cursor.getSprite();
	_collisionType = 1;
	for (int i = 0; i < Objects::MAX_MOVED_OBJECTS; i++) {
		if (g_engine->_objectVar.getMoveObjectRoom(i) == _roomNumber) {
			Common::Point movedObjPos = g_engine->_objectVar.getMoveObjectPosition(i);
			int16 spriteWidth = 0;
			int16 spriteHeight = 0;
			if (i == 22) {
				uint8 spriteIdx = g_engine->_objectVar.getVar(5) != 0 ? 1 : 0;
				const Sprite &sprite = _locationSprites.getSpriteAt(spriteIdx);
				spriteWidth = sprite.width;
				spriteHeight = sprite.height;
			} else {
				const Sprite &sprite = g_engine->_baseSprites.getSpriteAt(i);
				spriteWidth = sprite.width;
				spriteHeight = sprite.height;
			}
			calculateScaledSpriteDimensions(spriteWidth, spriteHeight, movedObjPos.y);

			if (
				((spriteWidth / 2 + movedObjPos.x) - g_engine->scaledSpriteWidth / 2 <= cursorSprite.width + g_engine->_cursor.getX()) &&
				(g_engine->_cursor.getX() <= ((spriteWidth / 2 + movedObjPos.x) - g_engine->scaledSpriteWidth / 2) + g_engine->scaledSpriteWidth) &&
				((movedObjPos.y + spriteHeight) - g_engine->scaledSpriteHeight <= cursorSprite.height + g_engine->_cursor.getY()) &&
				g_engine->_cursor.getY() <= movedObjPos.y + spriteHeight) {
				return i;
			}
		}
	}
	return -1;
}

void Darkseed::Room::update() {
	if (g_engine->_actionMode == HandAction || g_engine->_actionMode > 4) {
		int moveableObj = checkCursorAndMoveableObjects();
		if (moveableObj == -1) {
			int objectUnderCursor = checkCursorAndStaticObjects(g_engine->_cursor.getX(), g_engine->_cursor.getY());
			if (objectUnderCursor == -1) {
				int movedObject = CheckCursorAndMovedObjects();
				if (movedObject == -1) {
					g_engine->_cursor.setCursorType(Hand);
				} else {
					g_engine->_cursor.setCursorType(HandPointing);
				}
			} else {
				g_engine->_cursor.setCursorType(HandPointing);
			}
		} else {
			g_engine->_cursor.setCursorType(HandPointing);
		}
	} else if (g_engine->_actionMode == LookAction) {
		if (checkCursorAndMoveableObjects() != -1 || CheckCursorAndMovedObjects() != -1) {
			g_engine->_cursor.setCursorType(ExclamationMark);
		} else {
			int objIdx = checkCursorAndStaticObjects(0,0);
			if (objIdx != -1 && _roomObj[objIdx].objNum > 7) {
				g_engine->_cursor.setCursorType(ExclamationMark);
			} else {
				g_engine->_cursor.setCursorType(Look);
			}
		}
	} else {
		if (g_engine->_actionMode != PointerAction) {
			g_engine->_cursor.setCursorType(Pointer);
		}

		int objectUnderCursor = checkCursorAndStaticObjects(g_engine->_cursor.getX(), g_engine->_cursor.getY());
		if (objectUnderCursor == -1 ||
			((_roomObj[objectUnderCursor].objNum > 5 || _roomObj[objectUnderCursor].type != 0) &&
			(g_engine->_objectVar[59] != 2 || _roomObj[objectUnderCursor].objNum != 59) &&
			(g_engine->_objectVar[78] != 2 || _roomObj[objectUnderCursor].objNum != 78))
			) {
			g_engine->_cursor.setCursorType(Pointer);
		} else {
			int roomExitObjNum = getRoomExitAtCursor();
			uint16 targetRoomNumber = getDoorTargetRoom(roomExitObjNum);
			if ((((((_roomNumber == 59) && (g_engine->_objectVar[190] < 2)) ||
				   ((_roomNumber == 61 && ((g_engine->_objectVar[22] < 3 && (targetRoomNumber == 13)))))) ||
				  ((_roomNumber == 7 && ((targetRoomNumber == 38 && (g_engine->_objectVar[137] == 0)))))) ||
				 ((((_roomNumber == 7 && ((targetRoomNumber == 38 && (g_engine->_objectVar[57] == 1)))) ||
					((_roomNumber == 46 && ((targetRoomNumber == 60 && (g_engine->_objectVar[57] == 1)))))) ||
				   ((((_roomNumber == 13 && ((targetRoomNumber == 31 && (g_engine->_objectVar[23] != 1)))) ||
					  ((_roomNumber == 2 && ((targetRoomNumber == 0 && (g_engine->_objectVar[78] != 2)))))) ||
					 ((_roomNumber == 0 && ((targetRoomNumber == 2 && (g_engine->_objectVar[78] != 2)))))))))) ||
				(((_roomNumber == 32 && ((targetRoomNumber == 13 && (g_engine->_objectVar[23] != 1)))) ||
				  (((((_roomNumber == 13 && ((targetRoomNumber == 32 && (g_engine->_objectVar[23] != 1)))) ||
					  ((_roomNumber == 39 && ((targetRoomNumber == 46 && (g_engine->_objectVar[117] == 0)))))) ||
					 ((_roomNumber == 3 && ((targetRoomNumber == 9 && (g_engine->_objectVar[59] != 2)))))) ||
					((_roomNumber == 9 && ((targetRoomNumber == 3 && (g_engine->_objectVar[59] != 2)))))))))) {
				g_engine->_cursor.setCursorType(Pointer);
			} else {
				g_engine->_cursor.setCursorType(ConnectorEntrance);
			}
		}
	}
}

bool Darkseed::Room::exitRoom() {
	return false;
}
int Darkseed::Room::getExitRoomNumberAtPoint(int x, int y) {
	int obj = checkCursorAndStaticObjects(x, y);
	for (int i = 0; i < room1.size(); i++) {
		if (
			room1[i].roomNumber != 0xff
			&& _roomObj[obj].xOffset <= room1[i].x
			&& room1[i].x <= _roomObj[obj].width + _roomObj[obj].xOffset
			&& _roomObj[obj].yOffset <= room1[i].y
			&& room1[i].y <= _roomObj[obj].yOffset + _roomObj[obj].height
			) {
			return room1[i].roomNumber;
		}
	}
	return -1;
}
Common::String Darkseed::Room::getRoomFilenameBase(int roomNumber) {
	if (roomNumber == 20 || roomNumber == 22) {
		return "room19";
	}
	return Common::String::format("room%d", roomNumber);
}

bool Darkseed::Room::canWalkAtLocation(int x, int y) {
	if (x < 69 || x >= 570 || y < 40 || y >= 239) {
		return false;
	}

	int t = (x - 69) / 5;

	return (walkableLocationsMap[t / 8].strip[(y - 40) / 5] >> (7 - (t % 8) & 0x1f) & 1);
}

bool Darkseed::Room::canWalkInLineToTarget(int srcX, int srcY, int destX, int destY) {
	int iVar1;
	int iVar2;
	int iVar4;

	iVar1 = destX - srcX;
	destY = destY - srcY;
	iVar4 = 0;
	if ((iVar1 < 0) || (0 < destY)) {
		if ((iVar1 < 1) && (destY < 1)) {
			iVar2 = -iVar1;
			if (-iVar1 <= -destY) { //destY == iVar1 || SBORROW2(iVar2,-destY) != iVar2 + destY < 0) { //-iVar1 <= -destY
				while (srcX != destX) {
					iVar4 = iVar4 - iVar1;
					if (-iVar4 == destY || -destY < iVar4) {
						iVar4 = iVar4 + destY;
						srcX = srcX + -1;
					}
					srcY = srcY + -1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
			else {
				while (srcX != destX) {
					iVar4 = iVar4 - destY;
					if (-iVar4 == iVar1 || -iVar1 < iVar4) {
						iVar4 = iVar4 + iVar1;
						srcY = srcY + -1;
					}
					srcX = srcX + -1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
		}
		else if ((iVar1 < 1) && (0 < destY)) {
			iVar2 = -iVar1;
			if (-destY == iVar1 || iVar2 < destY) {
				while (srcX != destX) {
					iVar4 = iVar4 - iVar1;
					if (destY <= iVar4) {
						iVar4 = iVar4 - destY;
						srcX = srcX + -1;
					}
					srcY = srcY + 1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
			else {
				while (srcX != destX) {
					iVar4 = iVar4 + destY;
					if (-iVar4 == iVar1 || -iVar1 < iVar4) {
						iVar4 = iVar4 + iVar1;
						srcY = srcY + 1;
					}
					srcX = srcX + -1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
		}
		else {
			iVar2 = iVar1;
			if (destY < iVar1) {
				while (srcX != destX) {
					iVar4 = iVar4 + destY;
					if (iVar1 <= iVar4) {
						iVar4 = iVar4 - iVar1;
						srcY = srcY + 1;
					}
					srcX = srcX + 1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
			else {
				while (srcX != destX) {
					iVar4 = iVar4 + iVar1;
					if (destY <= iVar4) {
						iVar4 = iVar4 - destY;
						srcX = srcX + 1;
					}
					srcY = srcY + 1;
					iVar2 = canWalkAtLocation(srcX,srcY);
					if (iVar2 == 0) {
						return 0;
					}
				}
			}
		}
	}
	else {
		iVar2 = -destY;
		if (iVar2 < iVar1) {
			while (srcX != destX) {
				iVar4 = iVar4 - destY;
				if (iVar1 <= iVar4) {
					iVar4 = iVar4 - iVar1;
					srcY = srcY + -1;
				}
				srcX = srcX + 1;
				iVar2 = canWalkAtLocation(srcX,srcY);
				if (iVar2 == 0) {
					return 0;
				}
			}
		}
		else {
			while (srcX != destX) {
				iVar4 = iVar4 + iVar1;
				if (-iVar4 == destY || -destY < iVar4) {
					iVar4 = iVar4 + destY;
					srcX = srcX + 1;
				}
				srcY = srcY + -1;
				iVar2 = canWalkAtLocation(srcX,srcY);
				if (iVar2 == 0) {
					return 0;
				}
			}
		}
	}
	return true;
//	int iVar2 = targetX - x;
//	int iVar3 = targetY - y;
//
//	if (iVar2 < 0 || iVar3 > 0) {
//		if (iVar2 < 1 && iVar3 < 1) {
//			if (-iVar2 <= -iVar3) {
//				int iVar4 = 0;
//				do {
//					if (x == targetX) {
//						return true;
//					}
//					iVar4 -= iVar2;
//					if (-iVar4 == iVar3 || -iVar3 < iVar4) {
//						iVar4 += iVar3;
//						x--;
//					}
//					y--;
//				} while (canWalkAtLocation(x, y));
//			}
//		} else {
//			int iVar4 = 0;
//			do {
//				if (x == targetX) {
//					return true;
//				}
//				iVar4 -= iVar3;
//				if (-iVar4 == iVar2 || -iVar2 < iVar4) {
//					iVar4 += iVar2;
//					y--;
//				}
//				x--;
//			} while (canWalkAtLocation(x, y));
//		}
//	} else if (-iVar3 < iVar2) {
//
//	} else {
//
//	}
//	return false;
}

void Darkseed::Room::printRoomDescriptionText() const {
	int textId = roomDescriptionTextTbl[_roomNumber];
	if (textId != 0) {
		g_engine->_console->printTosText(textId);
	}
}

int Darkseed::Room::getRoomExitAtCursor() {
	for (int i = 0; i < _roomObj.size(); i++) {
		Common::Rect roomRect(_roomObj[i].xOffset, _roomObj[i].yOffset, _roomObj[i].xOffset + _roomObj[i].width, _roomObj[i].yOffset + _roomObj[i].height);
		if (_roomObj[i].type == 0 && _roomObj[i].objNum < 6 && roomRect.contains(g_engine->_cursor.getPosition())) {
			selectedObjIndex = i;
			return _roomObj[i].objNum;
		}
	}
	return 0;
}

void Darkseed::Room::getWalkTargetForObjectType_maybe(int objId) {
	for (int i = 0; i < _roomObj.size(); i++) {
		if (_roomObj[i].objNum == objId && _roomObj[i].type == 4) {
			g_engine->_player->_walkTarget.x = _roomObj[i].xOffset;
			g_engine->_player->_walkTarget.y = _roomObj[i].yOffset;
			for (int j = 0; j < room1.size(); j++) {
				if (room1[j].roomNumber != 0xff
					&& _roomObj[selectedObjIndex].xOffset < room1[j].x
					&& room1[j].x < _roomObj[selectedObjIndex].xOffset + _roomObj[selectedObjIndex].width
					&& _roomObj[selectedObjIndex].yOffset < room1[j].y
					&& room1[j].y < _roomObj[selectedObjIndex].yOffset + _roomObj[selectedObjIndex].height
					) {
					if (_roomNumber != 0x3d || room1[j].roomNumber == 5 || g_engine->trunkPushCounter > 2) {
						g_engine->useDoorTarget = true;
					}
					g_engine->targetRoomNumber = room1[j].roomNumber;
					g_engine->targetPlayerDirection = room1[j].direction;
					break;
				}
			}
			return;
		}
	}
	g_engine->_player->_walkTarget.x = g_engine->_cursor.getX();
	g_engine->_player->_walkTarget.y = g_engine->_cursor.getY();
}

static const int scaleTbl[] = {
	 1000,          1000,          1000,          1000,
	 1000,          1000,          1000,          1000,
	 1000,          1000,           400,           750,
	  800,          1000,          1000,          1000,
	 1000,          1000,          1000,          1000,
	 1000,          1000,          1000,          1000,
	  750,           850,          1000,          1000,
	 1000,          1000,          1000,           800,
	 1000,          1000,          1000,           900,
	 1000,          1000,          1000,          1000,
	 1000,          1000,          1000,           830,
	 1000,           750,           550,           500,
	  650,          1000,           950,          1000,
	  500,           750,           700,           800,
	  800,          1000,          1000,          1000,
	 1000,          1000,          1000,           245,
	  750,           800,           500,           700,
	  800
};

static const uint8 room_sprite_related_2c85_4303[] = {
	 13,            13,            25,            25,
	28,            15,            22,            18,
	18,            13,            15,            15,
	35,            18,            40,            45,
	25,            22,            20,            10,
	10,            10,            10,            10,
	40,            20,            50,            30,
	25,            10,            10,            35,
	55,            35,            10,            45,
	15,            20,            13,            20,
	20,            15,            25,            30,
	20,            20,            30,            40,
	40,            60,            20,            15,
	5,            20,            10,            35,
	40,            15,            45,            10,
	34,            20,            25,             5,
	15,            25,            10,            10,
	15
};

void Darkseed::Room::calculateScaledSpriteDimensions(int width, int height, int curYPosition) {
	int local_6 = (g_engine->sprite_y_scaling_threshold_maybe - 2) - curYPosition;
	if (local_6 <= 0) {
		local_6 = 0;
	}
	g_engine->scaledWalkSpeed_maybe = scaleTbl[_roomNumber] - ((room_sprite_related_2c85_4303[_roomNumber] * local_6) / 5);
	g_engine->scaledSpriteWidth = (width * g_engine->scaledWalkSpeed_maybe) / 1000;
	g_engine->scaledSpriteHeight = (height * g_engine->scaledWalkSpeed_maybe) / 1000;
}

uint16 Darkseed::Room::getDoorTargetRoom(int objId) {
	for (int i = 0; i < _roomObj.size(); i++) {
		if (_roomObj[i].objNum == objId && _roomObj[i].type == 4) {
			for (int j = 0; j < room1.size(); j++) {
				if (room1[j].roomNumber != 0xff
					&& _roomObj[selectedObjIndex].xOffset < room1[j].x
					&& room1[j].x < _roomObj[selectedObjIndex].xOffset + _roomObj[selectedObjIndex].width
					&& _roomObj[selectedObjIndex].yOffset < room1[j].y
					&& room1[j].y < _roomObj[selectedObjIndex].yOffset + _roomObj[selectedObjIndex].height
				) {
					return room1[j].roomNumber;
				}
			}
		}
	}
	return g_engine->targetRoomNumber; //TODO is this a safe fallback if no door exists?
}

int Darkseed::Room::getObjectUnderCursor() {
	_collisionType = 0;
	int objIdx = checkCursorAndMoveableObjects();
	if (objIdx == -1) {
		objIdx = CheckCursorAndMovedObjects();
		if (objIdx == -1) {
			objIdx = checkCursorAndStaticObjects(0,0);
		}
	}
	return objIdx;
}

bool Darkseed::Room::isOutside() {
	bool isRoomOutside;

	if (_roomNumber == 61) {
		isRoomOutside = true;
	}
	else if (_roomNumber < 10 || _roomNumber > 14) {
		if (_roomNumber < 24 || _roomNumber > 27) {
			if (_roomNumber >= 63 && _roomNumber <= 65) {
				isRoomOutside = true;
			}
			else if (_roomNumber == 31 || _roomNumber == 32 || _roomNumber == 36) {
				isRoomOutside = true;
			}
			else {
				isRoomOutside = false;
			}
		}
		else {
			isRoomOutside = true;
		}
	}
	else {
		isRoomOutside = true;
	}
	return isRoomOutside;
}

void Darkseed::Room::runRoomObjects() {
	if (_roomNumber == 61) {
		drawTrunk();
	}
//	for (auto &roomObj : _roomObj) {
	for (int roomObjIdx = 0; roomObjIdx < _roomObj.size(); roomObjIdx++) {
		auto &roomObj = _roomObj[roomObjIdx];
		int xPos = roomObj.xOffset;
		int yPos = roomObj.yOffset;
		switch(roomObj.type) {
		case 1:
		case 3: {
			//			debug("roomObj.objNum: %d", roomObj.objNum);
			const Sprite &sprite = g_engine->_baseSprites.getSpriteAt(roomObj.spriteNum);
			if (_roomNumber == 15 || _roomNumber == 16) {
				g_engine->scaledSpriteWidth = sprite.width;
				g_engine->scaledSpriteHeight = sprite.height;
			} else {
				calculateScaledSpriteDimensions(sprite.width, sprite.height, roomObj.yOffset + sprite.height);
			}
			if (((roomObj.spriteNum != 7) && (roomObj.spriteNum != 36)) && ((roomObj.spriteNum != 37 && (((roomObj.spriteNum != 38 && (roomObj.spriteNum != 39)) && (roomObj.spriteNum != 40))))) ) {
				xPos = (sprite.width / 2 + xPos) - g_engine->scaledSpriteHeight / 2;
			}
			if (roomObj.spriteNum == 14) {
				g_engine->_sprites.addSpriteToDrawList(
					xPos,
					(yPos + sprite.height) - g_engine->scaledSpriteHeight,
					&sprite,
					240 - (yPos + sprite.height),
					g_engine->scaledSpriteWidth,
					g_engine->scaledSpriteHeight,
					false);
			} else {
				g_engine->_sprites.addSpriteToDrawList(
					xPos,
					(yPos + sprite.height) - g_engine->scaledSpriteHeight,
					&sprite,
					255,
					sprite.width,
					sprite.height,
					false);
			}
			break;
		}
		case 2: {
			int spriteNum = 0;
			if (_roomNumber == 17 && g_engine->isPlayingAnimation_maybe && g_engine->otherNspAnimationType_maybe == 19 && _locObjFrame[roomObjIdx] == 4) {
				advanceLocAnimFrame(roomObjIdx + 1);
				spriteNum = _locationSprites.getAnimAt(1).frameNo[_locObjFrame[roomObjIdx + 1]];
			} else if (_roomNumber == 16 && g_engine->isPlayingAnimation_maybe && g_engine->otherNspAnimationType_maybe == 35) {
				// shop
				g_engine->nextFrame(5);
				spriteNum = g_engine->_player->_animations.getAnimAt(5).frameNo[g_engine->animIndexTbl[5]];
			} else if (_roomNumber == 53) {
				if (g_engine->_objectVar[79] == 4) {
					g_engine->_objectVar[79] = 2;
				}
				bool frameAdvanced = advanceFrame(g_engine->_objectVar[79]);
				const Obt &anim = _locationSprites.getAnimAt(g_engine->_objectVar[79]);
				if (frameAdvanced) {
					roomObj.xOffset += anim.deltaX[_locObjFrame[g_engine->_objectVar[79]]];
					roomObj.yOffset += anim.deltaY[_locObjFrame[g_engine->_objectVar[79]]];
				}
				spriteNum = _locationSprites.getAnimAt(g_engine->_objectVar[79]).frameNo[_locObjFrame[g_engine->_objectVar[79]]];
				if (_ObjRestarted) {
					if (g_engine->_objectVar[79] == 1) {
						g_engine->_objectVar[79] = 3;
					}
					if (g_engine->_objectVar[79] == 2) {
						g_engine->stuffPlayer();
					}
				}
			} else {
				if (_roomNumber != 64 || g_engine->_currentTimeInSeconds < 64800) {
					advanceLocAnimFrame(roomObjIdx);
					// ForwardFrameOnObj(roomObjIdx);
				}
				spriteNum = _locationSprites.getAnimAt(_roomObj[roomObjIdx].spriteNum).frameNo[_locObjFrame[roomObjIdx]];
			}

			if (_roomNumber == 16 && g_engine->isPlayingAnimation_maybe && g_engine->otherNspAnimationType_maybe == 35) {
				const Sprite &sprite = g_engine->_player->getSprite(spriteNum);
				g_engine->_sprites.addSpriteToDrawList(
					xPos,
					yPos,
					&sprite,
					255,
					sprite.width,
					sprite.height,
					false);
			} else if (_roomNumber == 15) {
				if (g_engine->_objectVar[28] == 2) {
					const Sprite &sprite = _locationSprites.getSpriteAt(spriteNum);
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						240 - (yPos + sprite.height),
						sprite.width,
						sprite.height,
						false);
				}
			} else if (_roomNumber == 57 && spriteNum < 6) {
				if (g_engine->_previousRoomNumber == 54) {
					const Sprite &sprite = _locationSprites.getSpriteAt(spriteNum);
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						255,
						sprite.width,
						sprite.height,
						false);
				}
			} else if (_roomNumber == 58) {
				const Sprite &sprite = _locationSprites.getSpriteAt(spriteNum);
				if (g_engine->_objectVar[48] == 0) {
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						240 - (yPos + sprite.height),
						sprite.width,
						sprite.height,
						false);
				}
				updateRoomObj(48, xPos, sprite.width, yPos, sprite.height);
			} else if (_roomNumber == 5 && g_engine->isPlayingAnimation_maybe && g_engine->otherNspAnimationType_maybe == 7) {
				const Sprite &sprite = _locationSprites.getSpriteAt(roomObj.spriteNum);
				g_engine->_sprites.addSpriteToDrawList(
					xPos,
					yPos,
					&sprite,
					1,
					sprite.width,
					sprite.height,
					false);
			} else if (_roomNumber == 53) {
				if (g_engine->_objectVar[79] != 3) {
					const Sprite &sprite = _locationSprites.getSpriteAt(roomObj.spriteNum);
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						255,
						sprite.width,
						sprite.height,
						false);
				}
			} else if (((((((_roomNumber == 6) || (_roomNumber == 5)) || (_roomNumber == 7)) ||
						  ((_roomNumber == 32 || (_roomNumber == 26)))) ||
						 ((_roomNumber == 64 || ((_roomNumber == 65 || (_roomNumber == 63)))))) ||
						(_roomNumber == 66)) ||
					   (((((((_roomNumber == 67 || (_roomNumber == 28)) || (_roomNumber == 37)) ||
							((_roomNumber == 39 || (_roomNumber == 32)))) || (_roomNumber == 57)) ||
						  ((_roomNumber == 60 || (_roomNumber == 44)))) ||
						 ((_roomNumber == 57 || ((_roomNumber == 38 || (_roomNumber == 25)))))))) {
				const Sprite &sprite = _locationSprites.getSpriteAt(spriteNum);
				int playerLeftXPos = g_engine->_player->_position.x - g_engine->_player->getWidth() / 2;
				int playerRightXPos = g_engine->_player->_position.x + g_engine->_player->getWidth() / 2;
				if (_roomNumber == 63) {
					playerLeftXPos = g_engine->_player->_position.x - 4;
					playerRightXPos = g_engine->_player->_position.x + 4;
				}
				if (_roomNumber == 25) {
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						1,
						sprite.width,
						sprite.height,
						false);
				} else if ((_roomNumber != 64 || g_engine->_currentTimeInSeconds < 64801 || spriteNum == 3 || spriteNum > 13) &&
						      (
							   g_engine->isPlayingAnimation_maybe ||
							   _locationSprites.getAnimAt(roomObj.spriteNum).numFrames > 1 ||
							   (playerLeftXPos <= xPos + sprite.width && xPos <= playerRightXPos && g_engine->_player->_position.x <= xPos + sprite.width) ||
							   g_engine->_objectVar[141] == 6 ||
							   g_engine->_objectVar[141] == 12
							  )
						   ) {
					g_engine->_sprites.addSpriteToDrawList(
						xPos,
						yPos,
						&sprite,
						240 - yPos + sprite.height,
						sprite.width,
						sprite.height,
						false);
				}
			} else {
				const Sprite &sprite = _locationSprites.getSpriteAt(spriteNum);
				g_engine->_sprites.addSpriteToDrawList(
					xPos,
					yPos,
					&sprite,
					255,
					sprite.width,
					sprite.height,
					false);
			}
			if (_roomNumber == 57 && g_engine->_previousRoomNumber == 54 && spriteNum < 6) {
				g_engine->_objectVar[56] = spriteNum;
				if (_ObjRestarted) {
					g_engine->_objectVar[56] = 6;
				}
			}


//			debug("type 2 objNum %d", roomObj.objNum);

			break;
		}
		default:
			break;
		}
	}
	for (int i = 0; i < Objects::MAX_MOVED_OBJECTS; i++) {
		if (g_engine->_objectVar.getMoveObjectRoom(i) == _roomNumber) {
			const Sprite &sprite = g_engine->_baseSprites.getSpriteAt(i);
			Common::Point pos = g_engine->_objectVar.getMoveObjectPosition(i);
			calculateScaledSpriteDimensions(sprite.width, sprite.height, pos.y + sprite.height);
			g_engine->_sprites.addSpriteToDrawList((pos.x + sprite.width / 2) - g_engine->scaledSpriteWidth / 2, pos.y, &sprite, 255, sprite.width, sprite.height, false);
		}
	}

	if (_roomNumber == 59 && g_engine->_objectVar[190] > 1) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(490, 70, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 56 && g_engine->_objectVar[187] == 1) {
		const Sprite &sprite = _locationSprites.getSpriteAt(3);
		g_engine->_sprites.addSpriteToDrawList(490, 70, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 30 && g_engine->_objectVar[29] == 2) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(201, 140, &sprite, 255, sprite.width, sprite.height, false);
		updateRoomObj(113, 201, sprite.width, 140, sprite.height);
	}
	if (_roomNumber == 18 && (g_engine->_objectVar[80] == 0 || g_engine->_objectVar[80] == 2)) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(361, 127, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 0 && g_engine->_objectVar[78] == 2) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(519, 80, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 0 && g_engine->isPlayingAnimation_maybe) {
		const Sprite &sprite = _locationSprites.getSpriteAt(1);
		g_engine->_sprites.addSpriteToDrawList(111, 136, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 2 && g_engine->_player->_isAutoWalkingToBed && g_engine->_player->_position.x < 150) {
			g_engine->_objectVar[78] = 2; // open door for player.
	}
	if (_roomNumber == 2 && g_engine->_objectVar[78] == 2) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(69, 104, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 9 && g_engine->_objectVar[59] == 2) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(519, 77, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (g_engine->_player->_isAutoWalkingToBed && g_engine->_player->_position.x < 150 && _roomNumber == 3) {
		g_engine->_objectVar[59] = 2;
	}
	if (_roomNumber == 3 && g_engine->_objectVar[59] == 2) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(69, 105, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 17) {
		const Sprite &sprite = _locationSprites.getSpriteAt(21);
		g_engine->_sprites.addSpriteToDrawList(69, 91, &sprite, 240 - (91 + sprite.height), sprite.width, sprite.height, false);
	}
	if (_roomNumber == 32 && g_engine->_objectVar[23] != 0) { // rope garden room
		const Sprite &sprite = _locationSprites.getSpriteAt(15);
		g_engine->_sprites.addSpriteToDrawList(338, 46, &sprite, 255, sprite.width, sprite.height, false);
		updateRoomObj(102, 338, sprite.width, 46, sprite.height);
	}
	if (_roomNumber == 13 && g_engine->_objectVar[23] != 0) { // rope balcony
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(473, 116, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 14) { // tuttle tome
		if (g_engine->_objectVar[99] == 0) {
			if (!g_engine->isPlayingAnimation_maybe || g_engine->otherNspAnimationType_maybe != 23) {
				const Sprite &sprite = _locationSprites.getSpriteAt(g_engine->_objectVar[66] != 0 ? 1 : 0);
				g_engine->_sprites.addSpriteToDrawList(245, 140, &sprite, 255, sprite.width, sprite.height, false);
				const Sprite &sprite1 = _locationSprites.getSpriteAt(g_engine->_objectVar[67] == 0 ? 2 : 3);
				g_engine->_sprites.addSpriteToDrawList(295, 93, &sprite1, 255, sprite1.width, sprite1.height, false);
				const Sprite &sprite2 = _locationSprites.getSpriteAt(g_engine->_objectVar[68] == 0 ? 4 : 5);
				g_engine->_sprites.addSpriteToDrawList(334, 153, &sprite2, 255, sprite2.width, sprite2.height, false);
			} else {
				advanceLocAnimFrame(0);
				if (!_ObjRestarted) {
					const Sprite &sprite = _locationSprites.getSpriteAt(_locationSprites.getAnimAt(0).frameNo[_locObjFrame[0]]);
					g_engine->_sprites.addSpriteToDrawList(245, 93, &sprite, 255, sprite.width, sprite.height, false);
				} else {
					const Sprite &sprite = _locationSprites.getSpriteAt(9);
					g_engine->_sprites.addSpriteToDrawList(245, 93, &sprite, 255, sprite.width, sprite.height, false);
				}
			}
		} else {
			const Sprite &sprite = _locationSprites.getSpriteAt(9);
			g_engine->_sprites.addSpriteToDrawList(245, 93, &sprite, 255, sprite.width, sprite.height, false);
		}
	}
	if (_roomNumber == 34) { // in car
		if (g_engine->_objectVar[86] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(0);
			g_engine->_sprites.addSpriteToDrawList(470, 124, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[71] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(26);
			g_engine->_sprites.addSpriteToDrawList(261, 165, &sprite, 255, sprite.width, sprite.height, false);
		}
	}
	if (_roomNumber == 33) { // in garage
		if (g_engine->isPlayingAnimation_maybe && g_engine->otherNspAnimationType_maybe == 25) {
			const Sprite &sprite = _locationSprites.getSpriteAt(0);
			g_engine->_sprites.addSpriteToDrawList(370, 128, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[101] >= 1 && g_engine->_objectVar[101] <= 3) {
			const Sprite &sprite = _locationSprites.getSpriteAt(1);
			g_engine->_sprites.addSpriteToDrawList(488, 127, &sprite, 255, sprite.width, sprite.height, false);
		}
	}
	if (_roomNumber == 9) { // study
		int moveObj = g_engine->_objectVar.getMoveObjectRoom(34);
		if (moveObj == 100 || (moveObj >= 252 && moveObj <= 254)) {
			const Sprite &sprite = _locationSprites.getSpriteAt(2);
			g_engine->_sprites.addSpriteToDrawList(322, 147, &sprite, 255, sprite.width, sprite.height, false);
		}
	}
	if (_roomNumber == 7 && g_engine->_objectVar[137] == 0) { // mirror
		const Sprite &sprite = _locationSprites.getSpriteAt(8);
		g_engine->_sprites.addSpriteToDrawList(455, 149, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 62 && g_engine->_objectVar[25] != 0) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(291, 185, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 11 && g_engine->_currentTimeInSeconds > 64800) { // street
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(424, 182, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 64 && g_engine->_currentTimeInSeconds > 64800) {
		const Sprite &sprite = _locationSprites.getSpriteAt(0);
		g_engine->_sprites.addSpriteToDrawList(212, 124, &sprite, 255, sprite.width, sprite.height, false);
		const Sprite &sprite2 = _locationSprites.getSpriteAt(1);
		g_engine->_sprites.addSpriteToDrawList(305, 124, &sprite2, 255, sprite2.width, sprite2.height, false);
		const Sprite &sprite3 = _locationSprites.getSpriteAt(2);
		g_engine->_sprites.addSpriteToDrawList(322, 138, &sprite3, 255, sprite3.width, sprite3.height, false);
	}
	if (_roomNumber == 39 && g_engine->_objectVar[117] != 0) {
		const Sprite &sprite = _locationSprites.getSpriteAt(12);
		g_engine->_sprites.addSpriteToDrawList(190, 68, &sprite, 255, sprite.width, sprite.height, false);
	}
	if (_roomNumber == 8) { // kitchen
		if (g_engine->_objectVar[104] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(0);
			g_engine->_sprites.addSpriteToDrawList(194, 162, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[105] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(1);
			g_engine->_sprites.addSpriteToDrawList(230, 162, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[106] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(2);
			g_engine->_sprites.addSpriteToDrawList(266, 162, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[107] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(3);
			g_engine->_sprites.addSpriteToDrawList(302, 162, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[108] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(4);
			g_engine->_sprites.addSpriteToDrawList(452, 158, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[109] != 0) {
			const Sprite &sprite = _locationSprites.getSpriteAt(5);
			g_engine->_sprites.addSpriteToDrawList(300, 151, &sprite, 255, sprite.width, sprite.height, false);
		}
		if (g_engine->_objectVar[110] != 0) { // tap drip
			const Sprite &sprite = _locationSprites.getSpriteAt((g_engine->counter_2c85_888b & 1) + 6); // TODO double check counter_2c85_888b is actually _ConstantTick
			g_engine->_sprites.addSpriteToDrawList(266, 141, &sprite, 255, sprite.width, sprite.height, false);
		}
	}
	if (_roomNumber == 7 && g_engine->_objectVar[137] == 2) {
		const Sprite &sprite = g_engine->_player->_animations.getSpriteAt(12);
		g_engine->_sprites.addSpriteToDrawList(428, 78, &sprite, 255, sprite.width, sprite.height, false);
	}
}

bool Darkseed::Room::isGiger() {
	return _roomNumber >= 38 && (_roomNumber <= 60 || _roomNumber >= 66);
}

void Darkseed::Room::removeObjectFromRoom(int16 objNum) {
	if (_collisionType == 0) {
		for (auto &roomObj : _roomObj) {
			if (roomObj.objNum == objNum) {
				roomObj.type = 255;
				roomObj.objNum = 999;
				break;
			}
		}
	} else {
		g_engine->_objectVar.setMoveObjectRoom(objNum, 253);
	}
}

void Darkseed::Room::updateRoomObj(int16 objNum, int16 x, int16 width, int16 y, int16 height) {
	for (auto &roomObj : _roomObj) {
		if (roomObj.type == 0 && roomObj.objNum == objNum) {
			roomObj.xOffset = x;
			roomObj.yOffset = y;
			roomObj.width = width;
			roomObj.height = height;
			return;
		}
	}

	for (auto &roomObj : _roomObj) {
		if (roomObj.type > 10) {
			roomObj.type = 0;
			roomObj.objNum = objNum;
			roomObj.xOffset = x;
			roomObj.yOffset = y;
			roomObj.width = width;
			roomObj.height = height;
			return;
		}
	}
}

void Darkseed::Room::drawTrunk() {
	int trunkXPos;
	int trunkYPos;
	int spriteIdx;
  
	if (g_engine->_objectVar[22] == 0) {
		trunkXPos = 460;
		trunkYPos = 132;
	} else if (g_engine->_objectVar[22] == 1) {
		trunkXPos = 458;
		trunkYPos = 127;
	} else if (g_engine->_objectVar[22] == 2) {
		trunkXPos = 451;
		trunkYPos = 117;
	} else {
		trunkXPos = 410;
		trunkYPos = 98;
	}

	if (g_engine->_objectVar[42] == 1 || g_engine->_objectVar[42] == 2 || g_engine->_objectVar[42] == 3) {
		spriteIdx = 1;
	} else {
		spriteIdx = 0;
	}

	const Sprite &sprite = _locationSprites.getSpriteAt(spriteIdx);
	g_engine->sprite_y_scaling_threshold_maybe = 211;
	calculateScaledSpriteDimensions(sprite.width, sprite.height, trunkYPos + sprite.height);
	g_engine->sprite_y_scaling_threshold_maybe = 240;
	updateRoomObj(42, trunkXPos + 20,6, trunkYPos + 34,8);
	updateRoomObj(22, trunkXPos + 60,12, trunkYPos + 46,8);
	g_engine->_sprites.addSpriteToDrawList(
		trunkXPos,
		trunkYPos + sprite.height - g_engine->scaledSpriteHeight,
		&sprite,
		254,
		g_engine->scaledSpriteWidth,
		g_engine->scaledSpriteHeight,
		false);
	return;
}

void Darkseed::Room::advanceLocAnimFrame(int roomObjIdx) {
	const Obt &anim = _locationSprites.getAnimAt(_roomObj[roomObjIdx].spriteNum);
	_ObjRestarted = false;
	_locObjFrameTimer[roomObjIdx]--;
	if (_locObjFrameTimer[roomObjIdx] < 1) {
		_locObjFrame[roomObjIdx]++;
		if (_locObjFrame[roomObjIdx] == anim.numFrames) {
			_locObjFrame[roomObjIdx] = 0;
			_ObjRestarted = true;
		}
		_locObjFrameTimer[roomObjIdx] = anim.frameDuration[_locObjFrame[roomObjIdx]];
	}
}

bool Darkseed::Room::advanceFrame(int animIdx) {
	bool frameAdvanced = false;
	const Obt &anim = _locationSprites.getAnimAt(animIdx);
	_ObjRestarted = false;
	_locObjFrameTimer[animIdx]--;
	if (_locObjFrameTimer[animIdx] < 1) {
		frameAdvanced = true;
		_locObjFrame[animIdx]++;
		if (_locObjFrame[animIdx] == anim.numFrames) {
			_locObjFrame[animIdx] = 0;
			_ObjRestarted = true;
		}
		_locObjFrameTimer[animIdx] = anim.frameDuration[_locObjFrame[animIdx]];
	}
	return frameAdvanced;
}

void Darkseed::Room::runAnim47() {
	advanceFrame(2);
	if (!_ObjRestarted) {
		g_engine->_player->_frameIdx = _locationSprites.getAnimAt(2).frameNo[_locObjFrame[2]];
	} else {
		g_engine->_objectVar[79] = 1;
		g_engine->isPlayingAnimation_maybe = false;
		g_engine->_inventory.removeItem(19);
		g_engine->_objectVar.setMoveObjectRoom(19, 100);
	}
}
