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

#pragma once

namespace ps
{

namespace lib
{

namespace sql
{

/**
 * @class cFetchable
 * @brief
 * This class provides following roles.
 * -# a common interface for inheriting to classes that instantiate unloading derived from this class,
 */
class cFetchable
{
public:
    /**
     * @brief
     */
    virtual ~cFetchable() =0;
    /**
     * @brief
     */
    virtual void vExecuteAndFetch() =0;
    /**
     * @brief
     */
    virtual void vPreRepeatAction() =0;
    /**
     * @brief
     */
    virtual void vPostRepeatAction() =0;
    /**
     * @brief
     */
    virtual void vNotFoundAction() =0;
    /**
     * @brief
     * - It is invoked by ps::lib::sql::occi::cStmt::iFetch.
     */
    virtual void vPreBulkAction(const uint32_t& iBulkSize) =0;
    /**
     * @brief
     * - It is invoked by ps::lib::sql::occi::cStmt::iFetch.
     */
    virtual void vPostBulkAction(const uint32_t& iNumIter) =0;
    /**
     * @brief
     * - It is invoked by ps::lib::sql::occi::cStmt::iFetch.
     */
    virtual void vFeedbackAction() =0;
    /**
     * @brief
     */
    virtual void vFinalizeAction() =0;
};

} // ps::lib::sql

} // ps::lib

} // ps

