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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

cCompressObject::cCompressObject(
    const char szCompression[]
    , const char szCompressFor[]
    , const ps::lib::sql::ind_t& nCompressForInd
)
	: szCompression_(szCompression)
	, szCompressFor_(szCompressFor)
	, nCompressForInd_(nCompressForInd)
{
    using ps::lib::iCompBinary;
    using ps::lib::iCompTernary;
    if (iCompBinary(strcmp, szCompression, "ENABLED"))
    {
        oss_.str("");
        oss_ << std::string("compress ");
        if (nCompressForInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            if (iCompBinary(strcmp, szCompressFor, "FOR ALL OPERATIONS"))
            {
                oss_ << std::string("for all operations");
            }
            else if (iCompBinary(strcmp, szCompressFor, "OLTP"))
            {
                oss_ << std::string("for oltp");
            }
            else if (iCompTernary(strncmp, szCompressFor, "QUERY", 5))
            {
                oss_ << std::string("for query");
            }
            else if (iCompTernary(strncmp, szCompressFor, "ARCHIVE", 7))
            {
                oss_ << std::string("for archive");
            }
            else
            {
                // Nothing to do.
            } 
            oss_ << std::string(" ");
        }
        else if (iCompBinary(strcmp, szCompression, "DISABLED"))
        {
            oss_ << std::string("nocompress ");
        }
        else if (iCompBinary(strcmp, szCompression, "NONE"))
        {
			szCompression_ = "DISABLED";
        }
        else
        {
            // Nothing to do.
        }
        sClause_ = oss_.str();
    }
}

cCompressObject::~cCompressObject()
{}

std::string cCompressObject::sGetClause(void) const
{
    return sClause_;
}

const bool operator==(
    const cCompressObject& lhs
    , const cCompressObject& rhs
){
    return ps::lib::composite_equal(
        lhs.szCompression_, rhs.szCompression_
        , lhs.nCompressForInd_, rhs.nCompressForInd_
        , lhs.szCompressFor_, rhs.szCompressFor_
    );
}

const bool operator!=(
    const cCompressObject& lhs
    , const cCompressObject& rhs
){
    return ! (lhs == rhs);
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
