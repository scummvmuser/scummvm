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

#include "base/plugins.h"
#include "common/translation.h"
#include "engines/advancedDetector.h"
#include "hypno/hypno.h"

static const DebugChannelDef debugFlagList[] = {
	{Hypno::kHypnoDebugFunction, "functions", "Function execution debug channel"},
	{Hypno::kHypnoDebugCode, "code", "Code execution debug channel"},
	{Hypno::kHypnoDebugScript, "script", "Script execution debug channel"},
	DEBUG_CHANNEL_END};

namespace Hypno {
static const PlainGameDescriptor hypnoGames[] = {
	{"sinister-six", "Marvel Comics Spider-Man: The Sinister Six"},
	{"wetlands", "Wetlands"},
	{0, 0}};

static const ADGameDescription gameDescriptions[] = {
	{"sinister-six", // Demo from the US release
	 "Demo",
	 AD_ENTRY2s("COMBAT.EXE", "15a6b1b3819ef002438df340509b5373", 643471,
				"SPIDER.EXE", "15a6b1b3819ef002438df340509b5373", 603587),
	 Common::EN_USA,
	 Common::kPlatformDOS,
	 ADGF_TESTING | ADGF_DEMO,
	 GUIO1(GUIO_NOMIDI)},
	{"sinister-six", // US release
	 "Not yet implemented",
	 AD_ENTRY2s("COMBAT.EXE", "bac1d734f2606dbdd0816dfa7a5cf518", 255115,
				"SPIDER.EXE", "bac1d734f2606dbdd0816dfa7a5cf518", 248056),
	 Common::EN_USA,
	 Common::kPlatformDOS,
	 ADGF_UNSUPPORTED | ADGF_TESTING,
	 GUIO1(GUIO_NOMIDI)},
	{"wetlands", // Wetlands Demo Disc (November 1995)
	 "Demo",
	 AD_ENTRY2s("wetlands.exe", "15a6b1b3819ef002438df340509b5373", 642231,
				"wetdemo.exe", "15a6b1b3819ef002438df340509b5373", 458319),
	 Common::EN_USA,
	 Common::kPlatformDOS,
	 ADGF_TESTING | ADGF_DEMO,
	 GUIO1(GUIO_NOMIDI)},
	{"wetlands", // Wetlands (US)
	 "",
	 AD_ENTRY2s("wetlands.exe", "15a6b1b3819ef002438df340509b5373", 647447,
				"install.exe", "15a6b1b3819ef002438df340509b5373", 477839),
	 Common::EN_USA,
	 Common::kPlatformDOS,
	 ADGF_TESTING,
	 GUIO1(GUIO_NOMIDI)},
	AD_TABLE_END_MARKER};
} // End of namespace Hypno

static const char *const directoryGlobs[] = {
	"sixdemo",
	"factory",
	"wetlands",
	"movie",
	0};

class HypnoMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	HypnoMetaEngineDetection() : AdvancedMetaEngineDetection(Hypno::gameDescriptions, sizeof(ADGameDescription), Hypno::hypnoGames) {
		_maxScanDepth = 10;
		_directoryGlobs = directoryGlobs;
	}

	const char *getEngineId() const override {
		return "hypno";
	}

	const char *getName() const override {
		return "Hypno";
	}

	const char *getOriginalCopyright() const override {
		return "Marvel Comics Spider-Man: The Sinister Six (C) Brooklyn Multimedia\n"
			   "Wetlands (C) Hypnotix, Inc.";
	}

	const DebugChannelDef *getDebugChannels() const override {
		return debugFlagList;
	}
};

REGISTER_PLUGIN_STATIC(HYPNO_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, HypnoMetaEngineDetection);
