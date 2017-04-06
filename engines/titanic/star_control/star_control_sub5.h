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

#ifndef TITANIC_STAR_CONTROL_SUB5_H
#define TITANIC_STAR_CONTROL_SUB5_H

#include "common/array.h"
#include "titanic/star_control/fvector.h"
#include "titanic/star_control/fpose.h"
#include "titanic/star_control/error_code.h"
#include "titanic/star_control/surface_area.h"

namespace Titanic {

class CStarControlSub12;

class CStarControlSub5 {
	struct Data1 {
		int _index1;
		int _index2;
		Data1() : _index1(0), _index2(0) {}
	};

	struct SubEntry {
		Common::Array<Data1> _data1;
		Common::Array<FVector> _data2;
		~SubEntry() { clear(); }

		/**
		 * Clears the entry
		 */
		void clear();
	};

	struct Entry {
		int _field0;
		byte _pixel1;
		byte _pixel2;
		byte _pixel3;
		int _field8;
		int _fieldC;
		double _field10;
		double _field14;

		Entry() : _field0(0), _pixel1(0), _pixel2(0), _pixel3(0), _field8(0),
				_fieldC(0), _field10(0), _field14(0) {}
	};

	struct GridEntry : public FVector {
		FPoint _position;
		int _field14;

		GridEntry() : FVector(), _field14(0) {}
	};

	/**
	 * Maintains a pre-calculated table of sine values
	 */
	struct SineTable {
	private:
		Common::Array<double> _data;
	public:
		SineTable() {}

		/**
		 * Sets up the table
		 */
		bool setup();

		/**
		 * Get a value
		 */
		double operator[](int idx) { return _data[idx]; }
	};
private:
	bool _flag;
	FPose _sub1, _sub2;
	SubEntry _array[5];
	Entry _entries[1284];
	int _multiplier;
	SineTable _sineTable;
	Common::Array<GridEntry> _grid;
private:
	/**
	 * Sets up the data for an array entry
	 * @return	True if success
	 */
	bool setupEntry(int width, int height, int index, double val);

	/**
	 * Secondary setup method
	 * @return	True if success
	 */
	bool setup2(int val1, int val2);
public:
	CStarControlSub5();
	virtual ~CStarControlSub5() {}

	virtual bool setup();
	virtual void proc2(FPose *pose, const FVector &vector, double v1, double v2, double v3,
		CSurfaceArea *surfaceArea, CStarControlSub12 *sub12);
	virtual void proc3(CErrorCode *errorCode);

	bool get4() const { return _flag; }
	void set4(bool val) { _flag = val; }

	void fn1();
};

} // End of namespace Titanic

#endif /* TITANIC_STAR_CONTROL_SUB5_H */
