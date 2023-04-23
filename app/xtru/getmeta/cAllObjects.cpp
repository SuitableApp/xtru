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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/*
 * @struct tAttributes
 */

cAllObjects::tAttributes::tAttributes(const tKeyTuple& oKey)
    : nObjectTypeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    , nSubObjectNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
{
    ::memset(szOwner, 0, sizeof(szOwner));
    ::memset(szObjectType, 0, sizeof(szObjectType));
    ::memset(szObjectName, 0, sizeof(szObjectName));
    ::memset(szSubObjectName, 0, sizeof(szSubObjectName));
    ::strcpy(szOwner, std::get<0>(oKey));
    ::strcpy(szObjectType, std::get<1>(oKey));
}

cAllObjects::tAttributes::tAttributes()
    : tAttributes(std::make_tuple("", ""))
{}

bool cAllObjects::tAttributes::operator<(const tAttributes& rhs) const
{
    return ps::lib::composite_less(
        szOwner, rhs.szOwner
        , szObjectType, rhs.szObjectType
        , szObjectName, rhs.szObjectName
        , szSubObjectName, rhs.szObjectName
    );
}

bool cAllObjects::tAttributes::iKeyMatched(const tAttributes& rhs) const
{
    return ps::lib::composite_equal(
        szOwner, rhs.szOwner
        , szObjectType, rhs.szObjectType
    );
}

std::string cAllObjects::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szObjectType='%s'(%d))"
        R"(, szObjectName='%s', szSubObjectName='%s'(%d))"
    )   % szOwner % szObjectType % nObjectTypeInd
        % szObjectName % szSubObjectName % nSubObjectNameInd
    ;
    return oss.str();
}

/**
 * @struct cAllObjects::cRetriever
 */
class cAllObjects::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllObjects::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllObjects::cRetriever::iBulkSize_ = 100;

cAllObjects::tAttributes cAllObjects::cRetriever::rTable_[cAllObjects::cRetriever::iBulkSize_];

cAllObjects::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllObjects
        , "Reading objects", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectType, SQLT_STR, &rTable_->nObjectTypeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubObjectName, SQLT_STR, &rTable_->nSubObjectNameInd, NULL, NULL, iSkip_);
}

void cAllObjects::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllObjects::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllObjects::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllObjects::szAllObjects[] = {
"select owner "
", object_type"
", object_name"
", subobject_name "
"from all_objects "
"where owner in %s "
"and generated = 'N' "
"and object_type in "
"('CLUSTER' "
",'FUNCTION'"
",'MATERIALIZED VIEW'"
",'PACKAGE'"
",'PACKAGE BODY'"
",'PROCEDURE'"
",'SEQUENCE'"
",'SYNONYM'"
",'TABLE'"
",'TRIGGER'"
",'TYPE'"
",'TYPE BODY'"
",'VIEW'"
") "
};

/*
information to choice value.
decode(o.type#
, 0, 'NEXT OBJECT', 1, 'INDEX', 2, 'TABLE', 3, 'CLUSTER',
4, 'VIEW', 5, 'SYNONYM', 6, 'SEQUENCE',
7, 'PROCEDURE', 8, 'FUNCTION', 9, 'PACKAGE',
11, 'PACKAGE BODY', 12, 'TRIGGER',
13, 'TYPE', 14, 'TYPE BODY',
19, 'TABLE PARTITION', 20, 'INDEX PARTITION', 21, 'LOB',
22, 'LIBRARY', 23, 'DIRECTORY', 24, 'QUEUE',
28, 'JAVA SOURCE', 29, 'JAVA CLASS', 30, 'JAVA RESOURCE',
32, 'INDEXTYPE', 33, 'OPERATOR',
34, 'TABLE SUBPARTITION', 35, 'INDEX SUBPARTITION',
40, 'LOB PARTITION', 41, 'LOB SUBPARTITION',
42, NVL((SELECT 'REWRITE EQUIVALENCE' FROM sum$ s
  WHERE s.obj#=o.obj# and bitand(s.xpflags, 8388608) = 8388608),
  'MATERIALIZED VIEW'),
43, 'DIMENSION',
44, 'CONTEXT', 46, 'RULE SET', 47, 'RESOURCE PLAN',
48, 'CONSUMER GROUP',
51, 'SUBSCRIPTION', 52, 'LOCATION',
55, 'XML SCHEMA', 56, 'JAVA DATA',
57, 'EDITION', 59, 'RULE',
60, 'CAPTURE', 61, 'APPLY',
62, 'EVALUATION CONTEXT',
66, 'JOB', 67, 'PROGRAM', 68, 'JOB CLASS', 69, 'WINDOW',
72, 'SCHEDULER GROUP', 74, 'SCHEDULE', 79, 'CHAIN',
81, 'FILE GROUP', 82, 'MINING MODEL', 87, 'ASSEMBLY',
90, 'CREDENTIAL', 92, 'CUBE DIMENSION', 93, 'CUBE',
94, 'MEASURE FOLDER', 95, 'CUBE BUILD PROCESS',
100, 'FILE WATCHER', 101, 'DESTINATION',
114, 'SQL TRANSLATION PROFILE',
115, 'UNIFIED AUDIT POLICY',
'UNDEFINED')
 */

cAllObjects::cAllObjects()
{
    // nothing to do
}

cAllObjects::~cAllObjects()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllObjects::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

