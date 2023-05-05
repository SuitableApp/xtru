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

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cSetCurrentSchema::cSetCurrentSchema(ps::lib::sql::occi::cSvc::tPtr& oSvc)
    : oSvc_(oSvc), szkey_("CURRENT_SCHEMA")
{
    oSvc_->vAddSessionParam(
        ps::lib::sql::occi::cFailover::tStmtType::INT_PARAM
        , szkey_, oSvc_->sGetSchemaName()
    );
}

void cSetCurrentSchema::vAlterCurrent(const std::string& sSchemaName)
{
    oSvc_->vModSessionParam(szkey_, sSchemaName.c_str());
}

cSetCurrentSchema::~cSetCurrentSchema()
{
    oSvc_->vDelSessionParam(szkey_);
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

