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

#pragma once

#define SEARCH_COND_LEN 6000+1

#define DEL_RULE_LEN    9+1
#define STATUS_LEN      8+1
#define DEFERRABLE_LEN 14+1
#define DEFERRERD_LEN   9+1
#define VALIDATED_LEN  13+1
#define GENETATED_LEN  14+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct tAllConstraints
{
    char szOwner[OBJECT_NAME_LEN];
    char szConstraintName[OBJECT_NAME_LEN];
    char szConstraintType[OBJECT_TYPE_LEN];
    char szTableName[OBJECT_NAME_LEN];
    char szSearchCondition[SEARCH_COND_LEN];
    ps::lib::sql::ind_t nSearchConditionInd;
    char szROwner[OBJECT_NAME_LEN];
    ps::lib::sql::ind_t nROwnerInd;
    char szRConstraintName[OBJECT_NAME_LEN];
    ps::lib::sql::ind_t nRConstraintNameInd;
    char szDeleteRule[DEL_RULE_LEN];
    ps::lib::sql::ind_t nDeleteRuleInd;
    char szStatus[STATUS_LEN];
    char szDeferrable[DEFERRABLE_LEN];
    char szDeferred[DEFERRERD_LEN];
    char szValidated[VALIDATED_LEN];
    char szGenerated[GENETATED_LEN];
    char szIndexOwner[OBJECT_NAME_LEN];
    ps::lib::sql::ind_t nIndexOwnerInd;
    char szIndexName[OBJECT_NAME_LEN];
    ps::lib::sql::ind_t nIndexNameInd;
    bool operator<(const tAllConstraints& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szConstraintName, rhs.szConstraintName
        );
    }
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps
