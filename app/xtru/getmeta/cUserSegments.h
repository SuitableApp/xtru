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

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cUserSegments
 *
 * @brief
 * provides the subset data of DBA_SEGMENTS
 *   or USER_SEGMENTS if "DBA_..." is not accessivle for user.
 *
 * @details
 * This class plays following role.
 * -# collects the information of the audit trail on database
 *   , and keeps them in this instance as a list container
 *   to generate the SQL statements.
 *   Following items are included in the information:
 *   -# Name of segment owner.
 *   -# Name of segment.
 *   -# Name of partition if object is partitioned.
 *   -# Type of segment.
 *   -# Name of tablespace.
 *   -# Size of segment.
 *   -# Size of initial extent.
 *   -# Size of next extent.
 *   -# Number of minimum extents.
 *   -# Number of maximum extents.
 *   -# Percentage of increase.
 *   -# Number of free lists.
 *   -# Number of free list groups.
 *   -# Name of buffer pool.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    ...
    ps::app::xtru::getmeta::cUserSegments oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, conf_.as<bool>("compress")));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "TABLE", ""), true);
    @endcode
*/
class cUserSegments
{
private:
    /**
     * @struct tAttributes
     * The structure of the row data is defined.
     * Details are defined in cpp.
     */
    struct tAttributes;
    /**
     * @class cRetriever
     * Details are defined in cpp.
     */
    class cRetriever;
    /**
     * @copybrief cAllSequences::tKeyTuple
     *
     * @details
     * - first value is passed to tAttributes::szOwner.
     * - second value is passed to tAttributes::szSegmentName.
     * - third value is passed to tAttributes::szSegmentType.
     * - fourth value is passed to tAttributes::szPartitionName.
     */
    typedef std::tuple<const char*, const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cUserSegments::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cUserSegments&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cUserSegments::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief
    /// A SQL-select to select DBA_SEGMENTS that
    /// requires 'SELECT ANY TABLE' privilege to access.
    static const char szDbaSegments[];
    static const char szUserSegments[];
public:
    /**
     * @brief
     */
    cUserSegments();
    virtual ~cUserSegments();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     * @param [in] iCompress
     *   - is -1 or more and less or equal INT32_MAX.
     *   - Set a positive number of days to go back to check the audit trail.
     *   - When -1 is given, it can suppress to be listed the trail.
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const bool& iCompress
    );
    /**
     * @copydoc cAllSequences::vPrintAllItems()
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
private:
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream& os, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
