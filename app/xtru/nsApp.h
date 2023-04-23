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

struct tFmtOfData
{
    static tFmtOfData FMT_VAR;
    static tFmtOfData FMT_FIX;
    const int32_t iDataFmt_;
    const std::string sDefaultFn_;
    const std::string sConfParam_;
    tFmtOfData(const int32_t& iDataFmt, const std::string& sDefaultFn, const std::string& sConfParam)
        :iDataFmt_(iDataFmt), sDefaultFn_(sDefaultFn), sConfParam_ (sConfParam)
    {}
};

struct tTabName
{
//    enum {FMT_VAR = 0, FMT_FIX = 1};
    std::string sOwner;
    std::string sTable;
    std::string sIotType;
    int32_t iDataFmt;
    int32_t iNumLongs;
    int32_t iNumLobs;
    tTabName(
        const char* o
        , const char* t
        , const char* i
        , int32_t d
        , int32_t l
        , int32_t b
    ) : sOwner(o)
        , sTable(t)
        , sIotType(i)
        , iDataFmt(d)
        , iNumLongs(l)
        , iNumLobs(b)
    {}
    /**
     * sGetConcatenatedName() returns "<owner_name>"."<table_name>"
     */
    std::string sGetConcatenatedName(const std::string& sEncloser) const
    {
        return (boost::format("%s%s%s.%s%s%s") 
            % sEncloser % sOwner % sEncloser
            % sEncloser % sTable % sEncloser
        ).str();
    }
    std::string sGetConcatenatedName(void) const
    {
        return sGetConcatenatedName("");
    }
    template<typename T>
    bool operator()(const T& rRowBuf) const
    {
        return rRowBuf.iEqual(*this);
    }
};
    
typedef ps::lib::cVector<tTabName> cTableList;

} // ps::app::xtru

} // ps::app

} // ps

