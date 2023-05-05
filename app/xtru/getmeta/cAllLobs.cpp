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

#define CACHE_TYPE_LEN           (3+1)
#define LOGGING_TYPE_LEN         (4+1)
#define INROW_TYPE_LEN           (3+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllLobs::tAttributes
 * See to study about [ALL_LOBS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-EA28B5D0-3C72-4D7A-9151-AD96A132FDE5.htm#REFRN20115)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllLobs::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the object containing the LOB
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the object containing the LOB
	char szColumnName[COLUMN_NAME_LEN];    ///< Name of the LOB column or attribute
	char szSegmentName[OBJECT_NAME_LEN];   ///< Name of the LOB segment
	char szIndexName[OBJECT_NAME_LEN];     ///< Name of the LOB index
    /// -  as a unit of allocation or manipulation
	ub4 iChunk;                            ///< Size (in bytes) of the LOB chunk
	ub4 iPctversion;                       ///< Maximum percentage of the LOB space used for versioning
    ps::lib::sql::ind_t nPctversionInd;    ///< ind_t::VAL_IS_NULL: value is NULL
    /// - YES - LOB data is placed in the buffer cache
    /// - NO - LOB data either is not brought into the buffer cache or is brought
    ///        into the buffer cache and placed at the least recently used end of the LRU list
    /// - CACHEREADS - LOB data is brought into the buffer cache only during 
    //         read operations but not during write operations
	char szCache[CACHE_TYPE_LEN];          ///< Indicates whether and how the LOB data is to be cached in the buffer cache:
	char szLogging[LOGGING_TYPE_LEN];      ///< Indicates whether or not changes to the LOB are logged:
	char szInRow[INROW_TYPE_LEN];          ///< Indicates whether some LOBs are stored inline with the base row (YES) or not (NO).
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iChunk(0)
        , iPctversion(0)
        , nPctversionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::memset(szSegmentName, 0, sizeof(szSegmentName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szCache, 0, sizeof(szCache));
        ::memset(szLogging, 0, sizeof(szLogging));
        ::memset(szInRow, 0, sizeof(szInRow));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szColumnName, rhs.szColumnName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllLobs& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllLobs::tAttributes::vPrinter(std::ostream& os, const size_t&, cAllLobs& oCtx) const
{
    using ps::lib::iCompBinary;
    const std::string sPctversion =
        nPctversionInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
        ? (boost::format("pctversion %d ") % iPctversion).str()
        : std::string("")
        ;
    os << boost::format("lob (%s) store as (")
            % ps::lib::sMakeEnclosedName(szColumnName)
        << std::endl
        << boost::format("  %s storage in row chunk %d %s%s %s ")
            % std::string(iCompBinary(strcmp, szInRow,"YES") ? "enable" : "disable")
            % iChunk
            % sPctversion
            % std::string(iCompBinary(strcmp, szCache,"YES") ? "cache" : "nocache")
            % std::string(
                iCompBinary(strcmp, szCache,"NO")
                ? (
                    iCompBinary(strcmp, szLogging,"YES")
                    ? "logging" : "nologging"
                )
                : ""
            )
        << oPrintItems(oCtx.oUserSegments_, std::make_tuple(szOwner, szSegmentName, "LOBSEGMENT", ""), true)
        << boost::format(") /* segment_name = %s index_name = %s */")
            % szSegmentName % szIndexName
        << std::endl
        ;
}

std::string cAllLobs::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szColumnName='%s', szSegmentName='%s')"
        R"(, szIndexName='%s', iChunk=%u, iPctversion=%u(%d))"
        R"(, szCache='%s', szLogging='%s', szInRow='%s')"
    ) % szOwner % szTableName % szColumnName % szSegmentName
        % szIndexName % iChunk % iPctversion % nPctversionInd
        % szCache % szLogging % szInRow
    ;
    return oss.str();
}

/**
 * @struct cAllLobs::cRetriever
 */
class cAllLobs::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllLobs::oList_.
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

const uint32_t cAllLobs::cRetriever::iBulkSize_ = 100;

cAllLobs::tAttributes cAllLobs::cRetriever::rTable_[cAllLobs::cRetriever::iBulkSize_];

cAllLobs::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllLobs
        , "Reading lob properties", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSegmentName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iChunk, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctversion, SQLT_UIN, &rTable_->nPctversionInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCache, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogging, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInRow, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllLobs::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllLobs::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllLobs::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllLobs::szAllLobs[] = {
"select owner "
", table_name "
", column_name "
", segment_name "
", index_name "
", chunk "
", pctversion "
", ltrim(cache) as cache "
", logging "
", in_row "
"from all_lobs "
"where owner in %s "
};

cAllLobs::cAllLobs(
    cUserSegments& oUserSegments
)
    : oUserSegments_(oUserSegments)
{
    // nothing to do
}

cAllLobs::~cAllLobs()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllLobs::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllLobs::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllLobs::tAttributes::iKeyMatched
        , &cAllLobs::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
