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

#include "m4/burger/flags.h"
#include "m4/core/term.h"
#include "m4/adv_r/adv.h"
#include "m4/adv_r/adv_inv.h"
#include "m4/burger/vars.h"

namespace M4 {
namespace Burger {

#define FLAGS_COUNT 512

struct BoonsvilleRec {
	int32 _time;
	int32 _trigger;
	const char *_text;
};

static const BoonsvilleRec ARRAY[] = {
	{ 600, -1, "After getting neuro stuff" },
	{ 2400, -1, "After getting survival stuff" },
	{ 6000, 10028, "Burl enters town" },
	{ 6600, -1, "After getting sensory stuff" },
	{ 9600, 10029, "Burl gets fed" },
	{ 19200, -1, "After getting language stuff" },
	{ 13200, 10030, "Burl stops eating" },
	{ 22800, -1, "After getting logic stuff" },
	{ 15000, 10031, "Burl leaves town" },
	{ 18600, 10031, "Band enters town" },
	{ 33600, 10022, "Abduction time" },
	{ 55200, 10023, "Nero timeout" },
	{ 166800, 10023, "Survival timeout" },
	{ 278400, 10023, "Sensory timeout" },
	{ 390000, 10023, "Language timeout" },
	{ 501600, 10023, "Logic timeout" },
	{ 505200, 10033, "Conclusion" },
	{ 0, 0, nullptr }
};

Flags::Flags() {
	resize(FLAGS_COUNT);
}

void Flags::sync(Common::Serializer &s) {
	size_t count = size();
	for (uint i = 0; i < count; ++i)
		s.syncAsSint32LE((*this)[i]);
}

void Flags::reset() {
	Common::fill(&(*this)[0], &(*this)[0] + FLAGS_COUNT, 0);
}

int32 Flags::get_boonsville_time_and_display(bool showTime) {
	if (showTime) {
		int time = (*this)[BOONSVILLE_TIME];
		int seconds = time % 60;
		time /= 60;
		int minutes = time % 60;
		time /= 60;

		term_message("Boonsville time: %d:%d:%d", time, minutes, seconds);
	}

	return (*this)[BOONSVILLE_TIME];
}

void Flags::set_boonsville_time(int32 time) {
	(*this)[BOONSVILLE_TIME] = time;
	(*this)[BOONSVILLE_TIME2] = time - 1;
}

bool Flags::advance_boonsville_time_and_check_schedule(int32 time) {
	if (player_commands_allowed() && _G(roomVal2) && INTERFACE_VISIBLE) {
		(*this)[BOONSVILLE_TIME2] = (*this)[BOONSVILLE_TIME];
		(*this)[BOONSVILLE_TIME] = time;
		return dispatch_scheduled_boonsville_time_trigger(
			get_boonsville_time_and_display());
	} else {
		return false;
	}
}

bool Flags::dispatch_scheduled_boonsville_time_trigger(int32 time) {
	KernelTriggerType oldMode = _G(kernel).trigger_mode;
	_G(kernel).trigger_mode = KT_DAEMON;
	bool result = false;

	for (const BoonsvilleRec *rec = ARRAY; rec->_time; ++rec) {
		if ((int32)(*this)[BOONSVILLE_TIME2] > rec->_time &&
				rec->_time <= (int32)(*this)[BOONSVILLE_TIME]) {
			result = true;
			term_message("Time for: %s", rec->_text);
			schedule_boonsville_time();
			kernel_trigger_dispatch_now(rec->_trigger);
		}
	}

	_G(kernel).trigger_mode = oldMode;
	return result;
}

void Flags::schedule_boonsville_time() {
	error("TODO: Flags::schedule_boonsville_time()");
}

void Flags::reset1() {
	set_boonsville_time(0);
	(*this)[V000] = 1000;
	inv_give_to_player("money");

	(*this)[V001] = 20;
	(*this)[V005] = 0;
	(*this)[V010] = 0;
	(*this)[V011] = 0;
	(*this)[V012] = 0;
	(*this)[V013] = 0;
	(*this)[ROOM101_FLAG1] = 0;
	(*this)[V015] = 0;
	(*this)[V017] = 0;
	(*this)[V018] = 0;
	(*this)[V019] = 0;
	(*this)[V023] = 0;
	(*this)[WAKE_UP_STOLIE_COUNT] = 0;
	(*this)[V028] = 0;
	(*this)[V031] = 0;
	(*this)[V032] = 0;
	(*this)[V033] = 2;
	(*this)[V034] = 0;

	inv_move_object("deed", 105);

	(*this)[V172] = 10023;
	(*this)[V037] = 1;
	(*this)[V039] = 0;
	(*this)[kRoadOpened] = 0;
	(*this)[V046] = 0;
	(*this)[V047] = 0;
	(*this)[V048] = 0;
	(*this)[V049] = 0;

	inv_move_object("jawz o' life", 137);
	inv_move_object("keys", 138);

	(*this)[V112] = 0;
	(*this)[V058] = 0;
	(*this)[V059] = 0;
	(*this)[V245] = 10027;

	inv_move_object("pantyhose", 142);
	inv_move_object("block of ice", 142);
	inv_move_object("broken mouse trap", 143);
	inv_move_object("spring", NOWHERE);
	inv_move_object("amplifier", 145);

	(*this)[V079] = 1;
	inv_move_object("phone bill", 170);
	inv_move_object("whistle", 170);

	(*this)[V083] = 0;
	(*this)[V084] = 0;
	(*this)[V085] = 0;
	(*this)[V086] = 0;
	(*this)[V087] = 0;
	(*this)[V088] = 0;
	(*this)[V092] = 0;
	(*this)[V091] = 0;

	inv_move_object("carrot juice", 172);
	inv_move_object("laxative", 174);
	inv_move_object("broken puz dispenser", 176);
	inv_move_object("puz dispenser", NOWHERE);

	conv_reset_all();
}

void Flags::reset2() {
	if ((*this)[V245] != 10027)
		inv_give_to_player("BLOCK OF ICE");
	if ((*this)[V277] == 6003)
		inv_give_to_player("PANTYHOSE");

	(*this)[V243] = 6000;
	(*this)[V244] = 6000;
	(*this)[V245] = 10027;
	(*this)[V246] = 0;
	(*this)[V255] = 0;
	(*this)[V256] = 0;
	(*this)[V257] = 0;
	(*this)[V258] = 0;
	(*this)[V263] = 0;
	(*this)[V265] = 0;
	(*this)[V266] = 0;
	(*this)[V269] = 1;
	(*this)[V270] = 6000;
	(*this)[V273] = 0;
	(*this)[V274] = 0;
	(*this)[V277] = 6000;
	(*this)[V278] = 0;

	if (_G(executing) != WHOLE_GAME) {
		inv_give_to_player("BLOCK OF ICE");
		inv_give_to_player("PANTYHOSE");
	}
}

void Flags::reset3() {
	if (inv_player_has("BOTTLE")) {
		inv_give_to_player("CARROT JUICE");
		inv_move_object("BOTTLE", NOWHERE);
	}

	(*this)[V107] = 0;
	(*this)[V111] = 0;
	inv_move_object("BURGER MORSEL", NOWHERE);

	(*this)[V117] = 0;
	(*this)[V124] = 0;
	(*this)[V118] = 3001;
	(*this)[V119] = 0;
	(*this)[V120] = 0;
	(*this)[V121] = 3001;
	(*this)[V122] = 0;
	(*this)[V124] = 0;

	inv_move_object("JUG", 303);
	inv_move_object("DISTILLED CARROT JUICE", NOWHERE);

	(*this)[V130] = 0;
	inv_move_object("MATCHES", 304);

	(*this)[V134] = 0;
	(*this)[V140] = 0;
	(*this)[V144] = 0;
	(*this)[V145] = 0;
}

void Flags::reset4() {
	(*this)[V186] = 0;
	(*this)[V234] = 0;
	(*this)[V196] = 0;
	(*this)[V197] = 0;
	(*this)[V198] = 0;
	(*this)[V199] = 0;
	(*this)[V200] = 5000;

	inv_move_object("KINDLING", 502);
	inv_move_object("BURNING KINDLING", 502);
	inv_move_object("GIZMO", 502);

	(*this)[V203] = 0;
	(*this)[V204] = 5000;
	(*this)[V205] = 0;
	(*this)[V207] = 0;

	inv_move_object("ROLLING PIN", 503);
	inv_move_object("RUBBER GLOVES", 503);

	(*this)[V210] = 5000;
	(*this)[V211] = 5000;
	(*this)[V212] = 5001;
	(*this)[V213] = 2;
	(*this)[V214] = 0;

	inv_move_object("DIRTY SOCK", 504);

	(*this)[V218] = 5000;
	(*this)[V219] = 0;
	(*this)[V223] = 0;
	(*this)[V224] = 0;

	inv_move_object("RUBBER DUCKY", 507);

	if (inv_player_has("SOAPY WATER")) {
		inv_move_object("SOAPY WATER", 507);
		inv_give_to_player("BOTTLE");
	}

	(*this)[V227] = 0;
	(*this)[V228] = 0;
	(*this)[V229] = 1;

	inv_move_object("CHRISTMAS LIGHTS", 508);
	inv_move_object("CHRISTMAS LIGHTS ", 508);

	(*this)[V237] = 0;
}

void Flags::reset5() {
	if ((*this)[V159])
		inv_give_to_player("DEED");

	(*this)[V157] = 0;
	(*this)[V158] = 0;
	(*this)[V159] = 0;
	(*this)[V162] = 0;

	inv_move_object("FISH", 402);
	inv_move_object("QUARTER", 404);

	(*this)[V175] = 0;
	(*this)[V171] = 4000;
	(*this)[V172] = 10023;
	(*this)[V173] = 0;
	(*this)[V174] = 4000;
	(*this)[V176] = 0;

	inv_move_object("COLLAR", 406);
	inv_move_object("HOOK", 406);

	(*this)[V166] = 0;
	(*this)[V168] = 0;
	(*this)[V167] = 0;

	inv_move_object("RECORDS", 405);

	(*this)[V180] = 0;
	(*this)[V181] = 0;
}

} // namespace Burger
} // namespace M4
