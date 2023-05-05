/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
 *
 * Extreme Unloader(XTRU) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Extreme Unloader(XTRU) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Extreme Unloader(XTRU).  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <pslib.h>
#include "cReferenceGraphTab.h"

#define UPDATABLE_LEN                      (1+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

const char cReferenceGraphTab::szCreStmt[] = {
/* Old name is APLDDT02 */
"CREATE TABLE REFERENCE_GRAPH_TAB\n"
"( OWNER                          TEXT NOT NULL\n"
", ROW_TYPE                       TEXT DEFAULT '1' NOT NULL\n"
", OBJECT_NAME                    TEXT NOT NULL\n"
", OBJECT_TYPE                    TEXT NOT NULL\n"
", R_OBJECT_NAME                  TEXT\n"
", R_OBJECT_TYPE                  TEXT\n"
", STATUS                         TEXT\n"
", CONSTRAINT AK_REFERENCE_GRAPH_TAB_01 UNIQUE\n"
"    ( OWNER, ROW_TYPE, STATUS\n"
"    , R_OBJECT_NAME, R_OBJECT_TYPE\n"
"    , OBJECT_NAME, OBJECT_TYPE\n"
"    )\n"
")"
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


