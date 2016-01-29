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

#define FORBIDDEN_SYMBOL_EXCEPTION_getenv
#define FORBIDDEN_SYMBOL_EXCEPTION_mkdir
#define FORBIDDEN_SYMBOL_EXCEPTION_exit
#define FORBIDDEN_SYMBOL_EXCEPTION_unistd_h
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h	//On IRIX, sys/stat.h includes sys/time.h

#include "common/scummsys.h"

#ifdef POSIX

#include "backends/platform/sdl/posix/posix.h"
#include "backends/saves/posix/posix-saves.h"
#include "backends/fs/posix/posix-fs-factory.h"
#include "backends/taskbar/unity/unity-taskbar.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {
/**
 * Assure that a directory path exists.
 *
 * @param dir The path which is required to exist.
 * @param prefix An (optional) prefix which should not be created if non existent.
 *               prefix is prepended to dir if supplied.
 * @return true in case the directoy exists (or was created), false otherwise.
 */
bool assureDirectoryExists(const Common::String &dir, const char *prefix = nullptr) {
	struct stat sb;

	// Check whether the prefix exists if one is supplied.
	if (prefix) {
		if (stat(prefix, &sb) != 0) {
			return false;
		} else if (!S_ISDIR(sb.st_mode)) {
			return false;
		}
	}

	// Obtain absolute path.
	Common::String path;
	if (prefix) {
		path = prefix;
		path += '/';
		path += dir;
	} else {
		path = dir;
	}

	path = Common::normalizePath(path, '/');

	const Common::String::iterator end = path.end();
	Common::String::iterator cur = path.begin();
	if (*cur == '/')
		++cur;

	do {
		if (cur + 1 != end) {
			if (*cur != '/') {
				continue;
			}

			// It is kind of ugly and against the purpose of Common::String to
			// insert 0s inside, but this is just for a local string and
			// simplifies the code a lot.
			*cur = '\0';
		}

		if (mkdir(path.c_str(), 0755) != 0) {
			if (errno == EEXIST) {
				if (stat(path.c_str(), &sb) != 0) {
					return false;
				} else if (!S_ISDIR(sb.st_mode)) {
					return false;
				}
			} else {
				return false;
			}
		}

		*cur = '/';
	} while (cur++ != end);

	return true;
}
} // End of anonymous namespace

OSystem_POSIX::OSystem_POSIX(Common::String baseConfigName)
	:
	_baseConfigName(baseConfigName) {
}

void OSystem_POSIX::init() {
	// Initialze File System Factory
	_fsFactory = new POSIXFilesystemFactory();

#if defined(USE_TASKBAR) && defined(USE_UNITY)
	// Initialize taskbar manager
	_taskbarManager = new UnityTaskbarManager();
#endif

	// Invoke parent implementation of this method
	OSystem_SDL::init();
}

void OSystem_POSIX::initBackend() {
	// Create the savefile manager
	if (_savefileManager == 0)
		_savefileManager = new POSIXSaveFileManager();

	// Invoke parent implementation of this method
	OSystem_SDL::initBackend();

#if defined(USE_TASKBAR) && defined(USE_UNITY)
	// Register the taskbar manager as an event source (this is necessary for the glib event loop to be run)
	_eventManager->getEventDispatcher()->registerSource((UnityTaskbarManager *)_taskbarManager, false);
#endif
}

bool OSystem_POSIX::hasFeature(Feature f) {
	if (f == kFeatureDisplayLogFile)
		return true;
	return OSystem_SDL::hasFeature(f);
}

Common::String OSystem_POSIX::getDefaultConfigFileName() {
	Common::String configFile;

	Common::String prefix;
#ifdef MACOSX
	prefix = getenv("HOME");
#elif !defined(SAMSUNGTV)
	const char *envVar;
	// Our old configuration file path for POSIX systems was ~/.scummvmrc.
	// If that file exists, we still use it.
	envVar = getenv("HOME");
	if (envVar && *envVar) {
		configFile = envVar;
		configFile += '/';
		configFile += ".scummvmrc";

		if (configFile.size() < MAXPATHLEN) {
			struct stat sb;
			if (stat(configFile.c_str(), &sb) == 0) {
				return configFile;
			}
		}
	}

	// On POSIX systems we follow the XDG Base Directory Specification for
	// where to store files. The version we based our code upon can be found
	// over here: http://standards.freedesktop.org/basedir-spec/basedir-spec-0.8.html
	envVar = getenv("XDG_CONFIG_HOME");
	if (!envVar || !*envVar) {
		envVar = getenv("HOME");
		if (!envVar) {
			return 0;
		}

		if (assureDirectoryExists(".config", envVar)) {
			prefix = envVar;
			prefix += "/.config";
		}
	} else {
		prefix = envVar;
	}

	if (!prefix.empty() && assureDirectoryExists("scummvm", prefix.c_str())) {
		prefix += "/scummvm";
	}
#endif

	if (!prefix.empty() && (prefix.size() + 1 + _baseConfigName.size()) < MAXPATHLEN) {
		configFile = prefix;
		configFile += '/';
		configFile += _baseConfigName;
	} else {
		configFile = _baseConfigName;
	}

	return configFile;
}

Common::WriteStream *OSystem_POSIX::createLogFile() {
	// Start out by resetting _logFilePath, so that in case
	// of a failure, we know that no log file is open.
	_logFilePath.clear();

	const char *prefix = nullptr;
	Common::String logFile;
#ifdef MACOSX
	prefix = getenv("HOME");
	if (prefix == nullptr) {
		return 0;
	}

	logFile = "Library/Logs";
#elif SAMSUNGTV
	prefix = nullptr;
	logFile = "/mtd_ram";
#else
	// On POSIX systems we follow the XDG Base Directory Specification for
	// where to store files. The version we based our code upon can be found
	// over here: http://standards.freedesktop.org/basedir-spec/basedir-spec-0.8.html
	prefix = getenv("XDG_CACHE_HOME");
	if (prefix == nullptr || !*prefix) {
		prefix = getenv("HOME");
		if (prefix == nullptr) {
			return 0;
		}

		logFile = ".cache/";
	}

	logFile += "scummvm/logs";
#endif

	if (!assureDirectoryExists(logFile, prefix)) {
		return 0;
	}

	if (prefix) {
		logFile = Common::String::format("%s/%s", prefix, logFile.c_str());
	}

	logFile += "/scummvm.log";

	Common::FSNode file(logFile);
	Common::WriteStream *stream = file.createWriteStream();
	if (stream)
		_logFilePath = logFile;
	return stream;
}

bool OSystem_POSIX::displayLogFile() {
	if (_logFilePath.empty())
		return false;

	// FIXME: This may not work perfectly when in fullscreen mode.
	// On my system it drops from fullscreen without ScummVM noticing,
	// so the next Alt-Enter does nothing, going from windowed to windowed.
	// (wjp, 20110604)

	pid_t pid = fork();
	if (pid < 0) {
		// failed to fork
		return false;
	} else if (pid == 0) {

		// Try xdg-open first
		execlp("xdg-open", "xdg-open", _logFilePath.c_str(), (char *)0);

		// If we're here, that clearly failed.

		// TODO: We may also want to try detecting the case where
		// xdg-open is successfully executed but returns an error code.

		// Try xterm+less next

		execlp("xterm", "xterm", "-e", "less", _logFilePath.c_str(), (char *)0);

		// TODO: If less does not exist we could fall back to 'more'.
		// However, we'll have to use 'xterm -hold' for that to prevent the
		// terminal from closing immediately (for short log files) or
		// unexpectedly.

		exit(127);
	}

	int status;
	// Wait for viewer to close.
	// (But note that xdg-open may have spawned a viewer in the background.)

	// FIXME: We probably want the viewer to always open in the background.
	// This may require installing a SIGCHLD handler.
	pid = waitpid(pid, &status, 0);

	if (pid < 0) {
		// Probably nothing sensible to do in this error situation
		return false;
	}

	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}


#endif
