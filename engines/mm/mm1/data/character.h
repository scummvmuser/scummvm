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

#ifndef MM1_DATA_CHAR_H
#define MM1_DATA_CHAR_H

#include "common/array.h"
#include "common/serializer.h"
#include "mm/mm1/data/items.h"

namespace MM {
namespace MM1 {

#define INVENTORY_COUNT 6
#define MAX_LEVEL 200

enum CharacterClass {
	KNIGHT = 1, PALADIN = 2, ARCHER = 3, CLERIC = 4,
	SORCERER = 5, ROBBER = 6, NONE = 0
};

enum Race {
	HUMAN = 1, ELF = 2, DWARF = 3, GNOME = 4, HALF_ORC = 5
};

enum Alignment {
	GOOD = 1, NEUTRAL = 2, EVIL = 3
};

enum Sex {
	MALE = 1, FEMALE = 2, YES_PLEASE = 3
};

enum Condition {
	FINE = 0, BAD_CONDITION = 0x80, ERADICATED = 0xff,
	DEAD = 0x40, STONE = 0x20,
	UNCONSCIOUS = 0x40, PARALYZED = 0x20, POISONED = 0x10,
	DISEASED = 8,  SILENCED = 4, BLINDED = 2, ASLEEP = 1
};

enum Resistance {
	RESISTANCE_MAGIC = 0, RESISTANCE_FIRE = 1, RESISTANCE_COLD = 2,
	RESISTANCE_ELECTRICITY = 3, RESISTANCE_ACID = 4,
	RESISTANCE_FEAR = 5, RESISTANCE_POISON = 6,
	RESISTANCE_PSYCHIC = 7
};

enum CharFlags0 {
	CHARFLAG0_COURIER1 = 1,
	CHARFLAG0_COURIER2 = 2,
	CHARFLAG0_COURIER3 = 4,
	CHARFLAG0_ZOM_CLUE = 8,
	CHARFLAG0_ZAM_CLUE = 0x10,
	CHARFLAG0_20 = 0x20,
	CHARFLAG0_40 = 0x40,
	CHARFLAG0_80 = 0x80
};

enum CharFlags1 {
	 CHARFLAG1_1 = 1
};

enum CharFlags2 {
	CHARFLAG2_1 = 1,
	CHARFLAG2_2 = 2,
	CHARFLAG2_4 = 4,
	CHARFLAG2_8 = 8,
	CHARFLAG2_10 = 0x10,
	CHARFLAG2_20 = 0x20,
	CHARFLAG2_40 = 0x40,
	CHARFLAG2_80 = 0x80
};

enum CharFlags4 {
	CHARFLAG4_COLOR = 0xf,
	CHARFLAG4_80 = 0x80
};

enum CharFlags5 {
	CHARFLAG5_1 = 1,
	CHARFLAG5_2 = 2,
	CHARFLAG5_4 = 4,
	CHARFLAG5_8 = 8
};

enum CharFlags7 {
	CHARFLAG7_20 = 0x20
};

enum CharFlags11 {
	CHARFLAG11_GOT_ENDURANCE = 1,
	CHARFLAG11_2 = 2,
	CHARFLAG11_4 = 4,
	CHARFLAG11_GOT_MIGHT = 8,
	CHARFLAG11_GOT_ACCURACY = 0x10,
	CHARFLAG11_GOT_SPEED = 0x20
};

enum CharFlags14 {
	CHARFLAG14_80 = 0x80
};

class Inventory {
public:
	struct Entry {
		byte _id = 0;
		byte _charges = 0;
		operator bool() const { return _id != 0; }
//		bool operator!() const { return !_id; }
//		operator byte() const { return _id; }
	};
private:
	Common::Array<Entry> _items;

	/**
	 * Used to test if the inventory has a category of item
	 */
	typedef bool (*CategoryFn)(byte id);
	bool hasCategory(CategoryFn fn) const;

	/**
	 * Returns the index of a free slot
	 */
	int getFreeSlot() const;

public:
	Inventory() {
		clear();
	}
	Entry &operator[](uint idx) {
		assert(idx < INVENTORY_COUNT);
		return _items[idx];
	}
	const Entry &operator[](uint idx) const {
		assert(idx < INVENTORY_COUNT);
		return _items[idx];
	}

	/**
	 * Saves or loads the inventory data
	 */
	void synchronize(Common::Serializer &s, bool ids);

	/**
	 * Clears the inventory
	 */
	void clear();

	/**
	 * Returns true if the inventory is empty
	 */
	bool empty() const;

	/**
	 * Returns true if the inventory is full
	 */
	bool full() const;

	/**
	 * Returns the size of the backpack that's filled in
	 */
	uint size() const;

	/**
	 * Adds an item to the inventory
	 */
	uint add(byte id, byte charges);

	/**
	 * Removes an index from the inventory
	 */
	void removeAt(uint idx);

	/**
	 * Remove an entry from the inventory
	 */
	void remove(Entry *e);

	/**
	 * Returns the index of a given entry
	 */
	int indexOf(Entry *e) const;

	/**
	 * Returns the index of an entry with a given id
	 */
	int indexOf(byte itemId) const;

	/**
	 * Decreases the charge on a magic item, and removes
	 * it if the charges have run out
	 */
	void removeCharge(Entry *e);

	/**
	 * The following methods return true if any of
	 * the contained items are of the given category
	 */
	bool hasWeapon() const { return hasCategory(isWeapon); }
	bool hasMissile() const { return hasCategory(isMissile); }
	bool hasTwoHanded() const { return hasCategory(isTwoHanded); }
	bool hasArmor() const { return hasCategory(isArmor); }
	bool hasShield() const { return hasCategory(isShield); }
};

/**
 * Attribute pair representing it's base value and the
 * current temporary value
 */
struct AttributePair {
	uint8 _current = 0;
	uint8 _base = 0;

	operator uint8() const { return _current; }
	AttributePair &operator=(byte v) {
		_base = _current = v;
		return *this;
	}
	AttributePair &operator++() {
		if (_base < 255)
			_current = ++_base;
		return *this;
	}
	AttributePair &operator--() {
		if (_base > 0)
			_current = --_base;
		return *this;
	}
	void clear() { _current = _base = 0; }
	void reset() { _current = _base; }
	void synchronize(Common::Serializer &s) {
		s.syncAsByte(_base);
		s.syncAsByte(_current);
	}
};

struct AttributePair16 {
	uint16 _current = 0;
	uint16 _base = 0;

	void clear() { _current = _base = 0; }
	AttributePair16 &operator=(byte v) {
		_base = _current = v;
		return *this;
	}
	void synchronize(Common::Serializer &s) {
		s.syncAsUint16LE(_base);
		s.syncAsUint16LE(_current);
	}
};

struct ResistanceFields {
	AttributePair _magic;
	AttributePair _fire;
	AttributePair _cold;
	AttributePair _electricity;
	AttributePair _acid;
	AttributePair _fear;
	AttributePair _poison;
	AttributePair _psychic;
};

union Resistances {
	ResistanceFields _s;
	AttributePair _arr[8];

	Resistances();

	/**
	 * Handles save/loading resistences
	 */
	void synchronize(Common::Serializer &s);
};

struct Character {
	char _name[16] = { 0 };
	Sex _sex = MALE;
	Alignment _alignmentInitial = GOOD;
	Alignment _alignment = GOOD;
	Race _race = HUMAN;
	CharacterClass _class = NONE;

	AttributePair _intelligence;
	AttributePair _might;
	AttributePair _personality;
	AttributePair _endurance;
	AttributePair _speed;
	AttributePair _accuracy;
	AttributePair _luck;

	AttributePair _level;
	AttributePair _age;
	AttributePair16 _sp;
	AttributePair _spellLevel;
	AttributePair _ac;

	uint32 _exp = 0;
	uint16 _gems = 0;
	uint16 _hpBase = 0, _hp = 0, _hpMax = 0;
	uint32 _gold = 0;
	uint8 _food = 0;
	uint8 _condition = 0;
	Inventory _equipped;
	Inventory _backpack;
	Resistances _resistances;

	// TODO: Figure out what these are
	AttributePair _physicalAttr, _missileAttr;
	int _v6c;
	byte _quest = 0;
	int _v6e;
	int _alignmentCtr;
	byte _flags[14];

	// Non persistent fields
	byte _numDrinks = 0;

	/**
	 * Handles save/loading a character
	 */
	void synchronize(Common::Serializer &s);

	/**
	 * Equality test
	 */
	bool operator==(const Character &rhs) const {
		return !strcmp(_name, rhs._name);
	}

	/**
	 * Clearing the character
	 */
	void clear();

	/**
	 * Gathers the party gold into the character
	 */
	void gatherGold();

	/**
	 * Trade an item to another
	 */
	enum TradeResult { TRADE_SUCCESS, TRADE_NO_ITEM, TRADE_FULL };
	TradeResult trade(int whoTo, int itemIndex);

	/**
	 * Increase the character's level by 1 at a trainer
	 */
	struct LevelIncrease {
		int _numHP;
		int _numSpells;
	};
	LevelIncrease increaseLevel();

	/**
	 * Buy an item
	 */
	enum BuyResult { BUY_SUCCESS, BUY_NOT_ENOUGH_GOLD, BUY_BACKPACK_FULL };
	BuyResult buyItem(byte itemId);

	/**
	 * Updates the current attribute levels to match
	 * their base values
	 */
	void updateAttributes();

	/**
	 * Updates the character's AC
	 */
	void updateAC();

	/**
	 * Updates the character's SP
	 */
	void updateSP();

	void updateResistances();

	/**
	 * Gets a character's condition string
	 */
	Common::String getConditionString() const;

	void castUnknown();

	/**
	 * Rest the character
	 */
	void rest();

	/**
	 * Returns true if the character has a given item
	 */
	bool hasItem(byte itemId) const;
};

} // namespace MM1
} // namespace MM

#endif
