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

namespace getdata
{

class cPartitionedBySchemeImpl;

class cPartitionedByScheme
{
public:
    cPartitionedByScheme(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , ps::lib::sql::occi::cSvc::tPtr& oSvc
        , const uint32_t& iBulkSize
        , const int32_t& iRows
        , ps::lib::tPtrFstream& st_make_sh
    );
    ~cPartitionedByScheme();
    bool iFind(const ps::app::xtru::cTableList::value_type& tbl) const ;
    /**
     * sGenerateSql() returns like following list of cascaded SQL-SELECT statements:
     * SELECT * FROM "<owner_name>"."<table_name>" PARTITION("<partition_name>");
     * SELECT * FROM "<owner_name>"."<table_name>" PARTITION("<partition_name>");
     * SELECT * FROM "<owner_name>"."<table_name>" PARTITION("<partition_name>"); ....
     */
    std::string sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl);
    /**
     * @brief
     * @param [in] tbl
     * @param [out] unldrs
     * @param [in] param_f
     */
    void vInsertUnloadTasks(
        const ps::app::xtru::tTabName& tbl
        , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
        , const boost::filesystem::path& param_f
    );
private:
    std::unique_ptr<cPartitionedBySchemeImpl> oImpl_;
    cPartitionedByScheme(const cPartitionedByScheme&) =delete;
    cPartitionedByScheme& operator=(const cPartitionedByScheme&) =delete;
};

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps 

