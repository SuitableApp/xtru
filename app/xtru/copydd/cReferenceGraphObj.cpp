/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
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

#include <pslib.h>
#include "cReferenceGraphObj.h"

#define UPDATABLE_LEN                      (1+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

const char cReferenceGraphObj::szCreStmt[] = {
/* Old name is APLDDT04 */
"CREATE TABLE REFERENCE_GRAPH_OBJ\n"
"( OWNER                          TEXT NOT NULL\n"
", NAME                           TEXT NOT NULL\n"
", TYPE                           TEXT\n"
", REFERENCED_OWNER               TEXT\n"
", REFERENCED_NAME                TEXT\n"
", REFERENCED_TYPE                TEXT\n"
", REFERENCED_LINK_NAME           TEXT\n"
", DEPENDENCY_TYPE                TEXT\n"
")"
};

const char cReferenceGraphObj::szIdxStmt[] = {
"CREATE INDEX REFERENCE_GRAPH_OBJ_IE01 ON REFERENCE_GRAPH_OBJ (REFERENCED_NAME)"
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


