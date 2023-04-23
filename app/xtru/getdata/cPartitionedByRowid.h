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

namespace app
{

namespace xtru
{

namespace getdata
{

class cPartitionedByRowidImpl;

class cPartitionedByRowid
{
public:
    cPartitionedByRowid(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , ps::lib::sql::occi::cSvc::tPtr& oSvc
        , const uint32_t& iBulkSize
        , const int32_t& iRows
        , ps::lib::tPtrFstream& st_make_sh
    );
    ~cPartitionedByRowid();
    bool iFind(const ps::app::xtru::cTableList::value_type& tbl) const ;
    /**
     * sGenerateSql() returns like following list of cascaded SQL-SELECT statements:
     * @code
     SELECT * FROM "<owner_name>"."<table_name>" WHERE ROWID BETWEEN '<predb>' AND '<prede>';
     SELECT * FROM "<owner_name>"."<table_name>" WHERE ROWID BETWEEN '<predb>' AND '<prede>';
     SELECT * FROM "<owner_name>"."<table_name>" WHERE ROWID BETWEEN '<predb>' AND '<prede>'; ....
     * @endcode
     */
    std::string sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl);
    /**
     * @brief
     * @param[in] tbl
     * @param[out] unldrs
     * @param[out] param_f
     */
    void vInsertUnloadTasks(
        const ps::app::xtru::tTabName& tbl
        , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
        , const boost::filesystem::path& param_f
    );
private:
    std::unique_ptr<cPartitionedByRowidImpl> oImpl_;
    cPartitionedByRowid(const cPartitionedByRowid&) =delete;
    cPartitionedByRowid& operator=(const cPartitionedByRowid&) =delete;
};

} // ps::app::xtru::getdata

} // ps::app::xtru

} /* namespace app */

} /* namespace ps */
