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

#define TEXT_LEN   4000+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllSource::tAttributes
 * See to study about [ALL_SOURCE]
 * (https://docs.oracle.com/database/121/REFRN/GUID-FD480C9E-F8DB-40DF-A04D-65765C45EA2F.htm#REFRN20249 "ALL_SOURCE")
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllSource::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szName[OBJECT_NAME_LEN];          ///< Name of object
	char szType[OBJECT_TYPE_LEN];          ///< Granted privilege
    uint16_t iTypeLen;                     ///< length of szType.
	int32_t iLine;                         ///< Index of the object type
	char szText[TEXT_LEN];                 ///< Name of the user who is granted privileges.
	uint16_t iTextSize;                    ///< Wheather WITH GRANT OPTION is given (YES or NO)
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iTypeLen(::strlen(std::get<2>(oKey)))
        , iLine(0)
        , iTextSize(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szName, 0, sizeof(szName));
        ::memset(szType, 0, sizeof(szType));
        ::memset(szText, 0, sizeof(szText));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szName, std::get<1>(oKey));
        ::strcpy(szType, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szName, rhs.szName
            , szType, rhs.szType
            , iLine, rhs.iLine
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szName, rhs.szName
            , szType, rhs.szType
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllSource& oCtx) const
    {
        // A LF is included at the end of each szText
        os << std::string(szText);
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllSource::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szName='%s', szType='%s', iLine=%d)"
        R"(, szText='%s', iTextSize=%u)"
    ) % szOwner % szName % szType % iLine
        % szText % iTextSize
    ;
    return oss.str();
}

/**
 * @struct cAllSource::cRetriever
 */
class cAllSource::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSource::oList_.
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

const uint32_t cAllSource::cRetriever::iBulkSize_ = 10;

cAllSource::tAttributes cAllSource::cRetriever::rTable_[cAllSource::cRetriever::iBulkSize_];

cAllSource::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllSource
        , "Reading stored programs", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szType, SQLT_STR, NULL, &rTable_->iTypeLen, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iLine, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szText, SQLT_STR, NULL, &rTable_->iTextSize, NULL, iSkip_);
}

void cAllSource::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllSource::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllSource::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllSource::szAllSource[] = {
"select owner, name, type, line, text "
"from all_source "
"where owner in %s "
};

cAllSource::cAllSource()
{
    // nothing to do
}

cAllSource::~cAllSource()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllSource::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllSource::vPrintAllItems(
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
        , &cAllSource::tAttributes::iKeyMatched
        , &cAllSource::tAttributes::vPrinter
        , &cAllSource::vPrintPreRows
        , &cAllSource::vPrintPostRows
        , &cAllSource::vPrintNotFound
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

void cAllSource::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os << std::string("create or replace ");
}

void cAllSource::vPrintPostRows(std::ostream& os, const tAttributes& rKey) const
{
    /*
     * Treat the end of the extracted source code.
     * Make sure the source end is "\n/\n"
     */
    std::string sWork = 
        boost::algorithm::replace_all_copy(oss_.str(), "\r\n", "\n");
    const std::string chars_to_check("\r\n/");
    auto rit = sWork.rbegin();
    while (chars_to_check.find(*rit) != std::string::npos)
    {
        ++rit;
    }
    sWork.erase(rit.base(), sWork.end());
    std::string sType;
    // Make the alphabet spelling lowercase.
    std::transform(&rKey.szType[0], &rKey.szType[rKey.iTypeLen], std::back_inserter(sType), ::tolower);
    os << sWork << std::endl << '/' << std::endl  // sequence of "\n/\n"
        << boost::format("show errors %s %s;")
            % sType % ps::lib::sMakeEnclosedName(rKey.szName)
        << std::endl;
}

void cAllSource::vPrintNotFound(std::ostream& os, const tAttributes&) const
{
    os << std::string("-- No PL/SQL source codes.") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
