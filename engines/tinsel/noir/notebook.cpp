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

#include "tinsel/noir/notebook.h"

#include "tinsel/background.h"
#include "tinsel/dialogs.h"
#include "tinsel/film.h"
#include "tinsel/handle.h"
#include "tinsel/multiobj.h"
#include "tinsel/noir/sysreel.h"
#include "tinsel/pdisplay.h"
#include "tinsel/timers.h"

namespace Tinsel {

void Notebook::AddHyperlink(int32 id1, int32 id2) {
	auto *invObject = _vm->_dialogs->GetInvObjectT3(id1);

	if (invObject->getTitle() != 0) {
		error("A clue can only be hyperlinked if it only has one title!");
		return;
	}

	invObject = _vm->_dialogs->GetInvObjectT3(id2);

	if (invObject->getTitle() != 0) {
		error("A clue can only be hyperlinked if it only has one title!");
		return;
	}

	uint32 i;
	for (i = 0; i < MAX_HYPERS; ++i) {
		int32 curr_id1 = _hyperlinks[i].id1;
		if (curr_id1 == 0) {
			_hyperlinks[i].id1 = id1;
			_hyperlinks[i].id2 = id2;
			return;
		}

		if ((curr_id1 == id1) || (id1 == _hyperlinks[i].id2)) {
			if ((curr_id1 != id2) && (id2 != _hyperlinks[i].id2)) {
				error("A clue/title can only be hyperlinked to one other clue/title!");
			}
			return;
		}
	}

	// No free hyperlink entry was found
	error("Too many hyperlinks");
}

void Notebook::ClearNotebookPage() {
	if (_prevPage != -1) {
		_pages[_prevPage].Clear();
	}
	_prevPage = -1;
	_pages[_currentPage].Clear();
}

void Notebook::Refresh() {
	auto reel = (_currentPage == 0 ? SysReel::NOTEPAD_CLOSED : SysReel::NOTEPAD_OPEN);
	auto film = GetSystemReelFilm(reel);
	InitStepAnimScript(&_anim, _object, film->reels->script, ONE_SECOND / film->frate);
	ClearNotebookPage();
	if (_currentPage != 0) {
		_pages[_currentPage].FillIn();
	}
}

int Notebook::AddTitle(const InventoryObjectT3 &invObject) {
	int id = invObject.getId();
	assert(invObject.isNotebookTitle());
	for (int i = 0; i < _numPages; i++) {
		if (_pages[i].GetTitle() == id) {
			return i;
		}
	}
	int linkedFromPage = invObject.getUnknown();

	// 0 page is the closed notebook, has no entries.
	if (linkedFromPage != 0) {
		// Allocate a line on the linked from page.
		assert(_pages[linkedFromPage].GetTitle() != 0);
		_pages[linkedFromPage].AddLine(id);
	}
	int pageIndex = _numPages++;
	_pages[pageIndex].SetTitle(id);
	return pageIndex;
}

void Notebook::AddClue(const InventoryObjectT3 &invObject) {
	// Add title if missing, otherwise just get the page it's on.
	auto titleObject = _vm->_dialogs->GetInvObjectT3(invObject.getUnknown());
	int pageIndex = AddTitle(*titleObject);
	_pages[pageIndex].AddLine(invObject.getId());
	if (invObject.getTitle() != 0) {
		auto secondTitleObject = _vm->_dialogs->GetInvObjectT3(invObject.getTitle());
		pageIndex = AddTitle(*secondTitleObject);
	 	_pages[pageIndex].AddLine(invObject.getId());
	}
}

void Notebook::AddClue(int id) {
	auto invObject = _vm->_dialogs->GetInvObjectT3(id);
	if (invObject->isNotebookTitle()) {
		AddTitle(*invObject);
	} else {
		AddClue(*invObject);
	}
}

void InitNotebookAnim(OBJECT **obj, ANIM &anim, SysReel reel, int zPosition) {
	auto film = GetSystemReelFilm(reel);
	MultiDeleteObjectIfExists(FIELD_STATUS, obj);
	*obj = InsertReelObj(film->reels);
	MultiSetZPosition(*obj, zPosition);
	InitStepAnimScript(&anim, *obj, film->reels->script, ONE_SECOND / film->frate);
}

void Notebook::SetNextPage(int pageIndex) {
	assert(_prevPage == -1 || _prevPage == _currentPage); // Check that we've cleaned any outstanding page.
	_prevPage = _currentPage;
	_currentPage = pageIndex;
}

void Notebook::Show(bool isOpen) {
	auto reel = (isOpen ? SysReel::NOTEPAD_OPEN : SysReel::NOTEPAD_OPENING);
	InitNotebookAnim(&_object, _anim, reel, Z_INV_MFRAME);

	_state = (isOpen ? BOOKSTATE::OPENED : BOOKSTATE::OPEN_ANIMATING);
	SetNextPage(1);
	Refresh();
	DisableTags(); // Tags disabled in Notebook
	DisablePointing(); // Pointing disabled in Notebook
}

bool Notebook::IsOpen() const {
	return _state != BOOKSTATE::CLOSED;
}

void Notebook::Close() {
	ClearNotebookPage();
	MultiDeleteObjectIfExists(FIELD_STATUS, &_object);
	MultiDeleteObjectIfExists(FIELD_STATUS, &_pageObject);
	_state = BOOKSTATE::CLOSED;
	if (_vm->_dialogs->InventoryOrNotebookActive()) {
		EnablePointing();
		EnableTags();
	}
}

void Notebook::StepAnimScripts() {
	if (_state == BOOKSTATE::OPEN_ANIMATING) {
		auto state = StepAnimScript(&_anim);
		if (state == ScriptFinished) {
			_state = BOOKSTATE::OPENED;
			Refresh();
		}
	}
}

} // End of namespace Tinsel
