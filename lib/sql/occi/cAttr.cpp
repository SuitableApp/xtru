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

#include "cAttrImpl.h"
#include "cAttrImplString.h"
#include "cAttrImplNumber.h"
#include "cAttrImplIeee754.h"
#include "cAttrImplDate.h"
#include "cAttrImplTimestamp.h"
#include "cAttrImplInterval.h"
#include "cAttrImplLob.h"
#include "cAttrImplRaw.h"
#include "cAttrImplRowid.h"
#include "cAttrImplBfile.h"

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cAttr::~cAttr()
{}

cAttr * cAttr::oMakeInstance(
    const std::string& tag
    , ps::lib::sql::occi::cOciStmt& oOciStmt
    , const uint32_t& pos
    , const oracle::occi::MetaData& meta
    , const uint32_t& iBulkSize
){
    BOOST_ASSERT(pos);
    BOOST_ASSERT(iBulkSize);
    cAttr *oAttr = 0;
    const ps::lib::cConfigures& conf_ = ps::lib::cConfigures::get_const_instance();
    oracle::occi::Type dType = (oracle::occi::Type) meta.getInt(oracle::occi::MetaData::ATTR_DATA_TYPE); 
    std::string sName = meta.getString(oracle::occi::MetaData::ATTR_NAME);
    switch (dType)
    // SQLT_* are defined in ocidfn.h and occiComon.h.
    {
    case oracle::occi::OCCI_SQLT_AFC:
    case oracle::occi::OCCI_SQLT_CHR:
        oAttr = new nsReprVar::cString(oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_NUM:
        {
            const int32_t iNumDigits = 15;
            /* To decide retrieved by one of string or OCINumber.
               CPU consumption when using string is lower than using OCINumber.
               This effect is significant when numerical precision is low. */
            int32_t dPrecision = meta.getInt(oracle::occi::MetaData::ATTR_PRECISION);
            int32_t dScale = meta.getInt(oracle::occi::MetaData::ATTR_SCALE);
            if (dPrecision <= iNumDigits && dScale >= 0 && dScale <= dPrecision){
                oAttr = new nsReprVar::cFixedNumber(oOciStmt, pos, dType, sName, meta, iBulkSize);
            } else {
                oAttr = new nsReprVar::cOtherNumber(oOciStmt, pos, dType, sName, meta, iBulkSize);
            }
        }
        break;
    case oracle::occi::OCCIIBDOUBLE: // BINARY_DOUBLE
        oAttr = new nsReprVar::cIeee754<double, oracle::occi::OCCIBDOUBLE, 17, 24>
            (oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCIIBFLOAT:  // BINARY_FLOAT
        oAttr = new nsReprVar::cIeee754<float, oracle::occi::OCCIBFLOAT, 8, 15>
            (oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_DAT:
        oAttr = new nsReprVar::cDate(oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_LNG:  // LONG
    case oracle::occi::OCCI_SQLT_CLOB: // CLOB, NCLOB
        oAttr = new nsReprVar::cLob<nsLob::tChr, oracle::occi::OCCI_SQLT_LNG>
            (oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_LBI:  // LONG RAW
    case oracle::occi::OCCI_SQLT_BLOB: // BLOB
        oAttr = new nsReprVar::cLob<nsLob::tRaw, oracle::occi::OCCI_SQLT_LBI>
            (oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_TIMESTAMP:
        oAttr = new nsReprVar::cTimestamp(oOciStmt, pos, dType, sName, meta, iBulkSize
            , conf_.as<std::string>("timestamp_mask"), "TIMESTAMP");
        break;
    case oracle::occi::OCCI_SQLT_TIMESTAMP_LTZ:
        oAttr = new nsReprVar::cTimestamp(oOciStmt, pos, dType, sName, meta, iBulkSize
            , conf_.as<std::string>("timestamp_mask"), "TIMESTAMP WITH LOCAL TIME ZONE");
        break;
    case oracle::occi::OCCI_SQLT_TIMESTAMP_TZ:
        oAttr = new nsReprVar::cTimestamp(oOciStmt, pos, dType, sName, meta, iBulkSize
            , conf_.as<std::string>("timestamp_tz_mask"), "TIMESTAMP WITH TIME ZONE");
        break;
    case oracle::occi::OCCI_SQLT_INTERVAL_DS:
        oAttr = new nsReprVar::cInterval<11>
            (oOciStmt, pos, dType, sName, meta, iBulkSize, "INTERVAL DAY TO SECOND");
        break;
    case oracle::occi::OCCI_SQLT_INTERVAL_YM:
        oAttr = new nsReprVar::cInterval<4>
            (oOciStmt, pos, dType, sName, meta, iBulkSize, "INTERVAL YEAR TO MONTH");
        break;
    case oracle::occi::OCCI_SQLT_BIN:
        oAttr = new nsReprVar::cRaw(oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_RDD:
        oAttr = new nsReprVar::cRowid(oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_FILE: // =SQLT_FILE =SQLT_BFILEE =114
        oAttr = new nsReprVar::cBfile(oOciStmt, pos, dType, sName, meta, iBulkSize);
        break;
    case oracle::occi::OCCI_SQLT_NTY: // =SQLT_NTY =108
        // Not ready.
        break;
    case oracle::occi::OCCI_SQLT_REF: // =SQLT_REF =110
        // Not ready.
        break;
    default :
        break;
    }
    ASSERT_OR_RAISE(0 != oAttr, std::runtime_error, boost::format
        ("%s %s;%s: oracle::occi::Type dType=%d has not supported.")
            % sClass(ps::lib::E) % tag % sName % dType);
    return oAttr;
}

void vCheckCompatibility(const cAttr::tContainer& lhs, const cAttr::tContainer& rhs)
{
    ASSERT_OR_RAISE(lhs.size() == rhs.size(), std::runtime_error
            , boost::format("%s Incompatible, number of items is not match.")
                % sClass(ps::lib::E));
    for (auto i = 0LU; i < lhs.size(); ++i)
    {
        ASSERT_OR_RAISE(
            lhs[i].sGetFieldType() == rhs[i].sGetFieldType()
            , std::runtime_error
            , boost::format(
                "%s Incompatible, type are not match."
                " Left field name = %s[%s]"
                " Right field name = %s[%s]"
            )
            % sClass(ps::lib::E)
            % lhs[i].sGetFieldName()
            % lhs[i].sGetFieldType()
            % rhs[i].sGetFieldName()
            % rhs[i].sGetFieldType()
        );
    }
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

