
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

#ifndef BAGEL_BOFLIB_BOF_DEBUG_H
#define BAGEL_BOFLIB_BOF_DEBUG_H

#include "bagel/boflib/bof_opt.h"
#include "bagel/boflib/bof_log.h"
#include "bagel/boflib/boffo.h"
#include "bagel/boflib/misc.h"

namespace Bagel {

/**
 * Declare a debug-options (.INI) file
 */
class CBofDebugOptions: public CBofOptions {
public:
	/**
	 * Constructor
	 * @param pszFileName		Name of debug options file
	 */
    CBofDebugOptions(const CHAR *pszFileName);

    INT  m_nDebugLevel;
    BOOL m_bAbortsOn;
    BOOL m_bMessageBoxOn;
    BOOL m_bRandomOn;
    BOOL m_bShowIO;
    BOOL m_bShowMessages;
};

extern CBofLog *g_pDebugLog;
extern CBofDebugOptions *g_pDebugOptions;


#if BOF_DEBUG

#define LogInfo(p)    { if ((g_pDebugLog != NULL) && (g_pDebugOptions != NULL) && (g_pDebugOptions->m_nDebugLevel >= 4)) g_pDebugLog->WriteMessage(LOG_INFO, p, 0, __FILE__, __LINE__); }
#define LogWarning(p) { if ((g_pDebugLog != NULL) && (g_pDebugOptions != NULL) && (g_pDebugOptions->m_nDebugLevel >= 3)) g_pDebugLog->WriteMessage(LOG_WARN, p, 0, __FILE__, __LINE__); }
#define LogError(p)   { if ((g_pDebugLog != NULL) && (g_pDebugOptions != NULL) && (g_pDebugOptions->m_nDebugLevel >= 2)) g_pDebugLog->WriteMessage(LOG_ERROR, p, 0, __FILE__, __LINE__); }
#define LogFatal(p)   { if ((g_pDebugLog != NULL) && (g_pDebugOptions != NULL) && (g_pDebugOptions->m_nDebugLevel >= 1)) g_pDebugLog->WriteMessage(LOG_FATAL, p, 0, __FILE__, __LINE__); }

#else

#define LogInfo(p)      ;
#define LogWarning(p)   ;
#define LogError(p)     ;
#define LogFatal(p)     ;

#endif

/**
 * Terminates current application and displays abort message
 * @param pszInfo		Message to be displayed
 * @param pszFile		Filename of where Abort takes place
 * @param				Line number in file
**/
extern VOID BofAbort(const CHAR *pszInfo, const CHAR *pszFile, INT nLine);

} // namespace Bagel

#endif
