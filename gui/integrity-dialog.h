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

#ifndef GUI_INTEGRITY_DIALOG_H
#define GUI_INTEGRITY_DIALOG_H

#include "backends/networking/curl/postrequest.h"

#include "common/array.h"
#include "common/formats/json.h"
#include "common/str.h"

#include "gui/dialog.h"
#include "gui/widget.h"

namespace GUI {

enum {
	OK = 0,
	MISSING = 1,
	CHECKSUM_MISMATCH = 2,
	SIZE_MISMATCH = 3,
	UNKNOWN = 4
};

enum IconProcessState {
	kChecksumStateNone,
	kChecksumStateCalculating,
	kChecksumComplete
};

class IntegrityDialog : Dialog {
	StaticTextWidget *_statusText;
	StaticTextWidget *_errorText;
	StaticTextWidget *_percentLabel;
	SliderWidget *_progressBar;
	ButtonWidget *_cancelButton;

	bool _close;

	void refreshWidgets();

public:
	IntegrityDialog(Common::String endpoint, Common::String gameConfig);
	~IntegrityDialog();

	void sendJSON();
	void checksumResponseCallback(const Common::JSONValue *r);
	void errorCallback(const Networking::ErrorResponse &error);

	void calculateTotalSize(Common::Path gamePath);

	static Common::Array<Common::StringArray> generateChecksums(Common::Path gamePath, Common::Array<Common::StringArray> &fileChecksums);
	static Common::JSONValue *generateJSONRequest(Common::Path gamePath, Common::String gameid, Common::String engineid, Common::String extra, Common::String platform, Common::String language);
	static void parseJSON(const Common::JSONValue *response);

	void open() override;
	void close() override;
	void handleCommand(CommandSender *sender, uint32 cmd, uint32 data) override;
	void handleTickle() override;
	void reflowLayout() override;

	void setError(Common::U32String &msg);

private:
	void setState(IconProcessState state);
};

} // End of namespace GUI

#endif // GUI_INTEGRITY_DIALOG_H
