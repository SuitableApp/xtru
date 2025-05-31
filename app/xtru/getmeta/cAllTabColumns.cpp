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

constexpr size_t MINIMUM_DTYPE_LENGTH = 15;

constexpr size_t DATA_TYPE_LEN = 106+1; // Length of name of Oracle external data type.
constexpr size_t CHAR_USED_LEN = 1+1;
constexpr size_t NULLABLE_LEN = 1+1;
constexpr size_t VIRTUAL_COLUMN_LEN = 3+1;
constexpr size_t DATA_DEFAULT_LEN = 1024+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllTabColumns::tAttributes
 * See to study about [ALL_TAB_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-7BF4F8C7-1917-4532-B048-C044E17A1754.htm#REFRN23243)
 * or [ALL_TAB_COLS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-85036F42-140A-406B-BE11-0AC49A00DBA3.htm#REFRN20276)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTabColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];        ///< Name of owner.
	char szTableName[OBJECT_NAME_LEN];    ///< Name of table.
	ub4 iColumnId;                        ///< ID of the column in the table.
	char szColumnName[OBJECT_NAME_LEN];   ///< Name of column in the table.
	char szDataType[DATA_TYPE_LEN];       ///< DataType on created.
	ub4 iDataLength;                      ///< Length of data.
	char szCharUsed[CHAR_USED_LEN];       ///< Type of charactor semantics. Identified by 'B' or 'C'.
	ps::lib::sql::ind_t nCharUsedInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iDataPrecision;                   ///< Accuracy (number of significant digits)
	ps::lib::sql::ind_t nDataPrecisionInd;///< ind_t::VAL_IS_NULL: value is NULL
	sb4 iDataScale;                       ///< Number of object type.
	ps::lib::sql::ind_t nDataScaleInd;    ///< ind_t::VAL_IS_NULL: value is NULL
	char szNullable[NULLABLE_LEN];        ///< 'Y' means nullable.
	char szVirtualColumn[VIRTUAL_COLUMN_LEN];///< "YES" means that this column is virtual.
	ps::lib::sql::ind_t nVirtualColumnInd;///< ind_t::VAL_IS_NULL: value is NULL
    /// To suppress NOT NULL constraint definition when the primary key has this column.
	ps::lib::sql::ind_t nPkColNameInd;
    /// To suppress data type clause when the foreign key has this column.
	ps::lib::sql::ind_t nFkColNameInd;
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnId(0)
        , iDataLength(0)
        , nCharUsedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iDataPrecision(0)
        , nDataPrecisionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iDataScale(0)
        , nDataScaleInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nVirtualColumnInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nPkColNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nFkColNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::memset(szDataType, 0, sizeof(szDataType));
        ::memset(szCharUsed, 0, sizeof(szCharUsed));
        ::memset(szNullable, 0, sizeof(szNullable));
        ::memset(szVirtualColumn, 0, sizeof(szVirtualColumn));
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
            , iColumnId, rhs.iColumnId
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
    /**
     * @brief
     * - retrieves Oracle external datatype name.
     * - See to study about [Data Types](https://docs.oracle.com/database/121/SQLRF/sql_elements001.htm#SQLRF0021)
     *   (Oracle Database Online Documentation 12c Release 1 (12.1): Database SQL Language Reference).
     */
    std::string sGetDataTypeClause(cAllTabColumns& oCtx) const;
    /**
     * @brief
     */
    std::string sGetNotNullConsClause(void) const
    {
        return (ps::lib::iCompBinary(strcmp, szNullable, "N")
            && nPkColNameInd == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? "not null " : ""
        ;
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintTabColumn(std::ostream& os, const size_t& i, cAllTabColumns& oCtx) const
    {
        // Virtual column is supported in 10g Release 1 or later.
        const auto sVirtualColumnClause = (
            nVirtualColumnInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
            && ps::lib::iCompBinary(strcmp, szVirtualColumn, "YES")
        )
            ? oCtx.oDataDefault_.sGetVirtualColumnClause(
                std::make_tuple(szOwner, szTableName, szColumnName)
            ) : std::string("")
        ;
        os << boost::format("%c %s%s %s %s%s")
            % (i == 0 ? '(' : ',')
            % sColumnIdCooment(iColumnId)
            % ps::lib::sMakeEnclosedName(szColumnName, MINIMUM_COLUMN_LENGTH)
            % sGetDataTypeClause(oCtx)
            % sVirtualColumnClause
            % sGetNotNullConsClause()
            << std::endl
        ;
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintViewColumn(std::ostream& os, const size_t& i, cAllTabColumns& oCtx) const
    {
        os << boost::format("%c %s%s")
            % (i == 0 ? '(' : ',')
            % sColumnIdCooment(iColumnId)
            // It is unnecessary to align with space character.
            % ps::lib::sMakeEnclosedName(szColumnName)
            << std::endl
        ;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllTabColumns::tAttributes::sGetDataTypeClause(cAllTabColumns& oCtx) const
{
    using ps::lib::iCascadeBinaryOr;
    using ps::lib::iCompBinary;
    using ps::lib::iCascadeTernaryOr;
    oss_.str("");
    std::string ret;
    if (! oCtx.iDependent_ || nFkColNameInd == ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        bool iCaps = true;
        if (iCascadeBinaryOr(
            strcmp, szDataType
            , "CHAR"
            , "VARCHAR2"
        ))
        {
            oss_ << boost::format("%s(%d%s)")% szDataType % iDataLength
                % (iCompBinary(strcmp, szCharUsed, "C") ? " char" : "");
        }
        else if (iCascadeBinaryOr(
            strcmp, szDataType
            , "NCHAR"
            , "NVARCHAR2"
            , "RAW"
        ))
        {
            oss_ << boost::format("%s(%d)") % szDataType % iDataLength;
        }
        else if (iCompBinary(strcmp, szDataType, "FLOAT"))
        {
            if (iDataPrecision == 126)
            {
                oss_ << szDataType;
            }
            else
            {
                oss_ << boost::format("%s(%d)") % szDataType % iDataPrecision;
            }
        }
        else if (iCompBinary(strcmp, szDataType, "NUMBER"))
        {
            if (
                nDataPrecisionInd == ps::lib::sql::ind_t::VAL_IS_NULL
                && nDataScaleInd == ps::lib::sql::ind_t::VAL_IS_NULL
            ){
                oss_ << szDataType;
            }
            else if (
                iDataScale == 0
                && nDataScaleInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
            {
                oss_ << boost::format("%s(%d)")
                    % szDataType % iDataPrecision;
            }
            else
            {
                oss_ << boost::format("%s(%d,%d)")
                    % szDataType % iDataPrecision % iDataScale;
            }
        }
        else if (iCompBinary(strcmp, szDataType, "LONG"))
        {
			if (oCtx.iLongTransit_ == 0)
            {
                oss_ << szDataType;
			}
            else if (oCtx.iLongTransit_ == 1)
            {
                oss_ << "CLOB";
			}
            else if (oCtx.iLongTransit_ == 2)
            {
                oss_ << "BFILE";
			}
        }
        else if (iCompBinary(strcmp, szDataType, "LONG RAW"))
        {
			if (oCtx.iLongTransit_ == 0)
            {
                oss_ << szDataType;
			}
            else if (oCtx.iLongTransit_ == 1)
            {
                oss_ << "BLOB";
			}
            else if (oCtx.iLongTransit_ == 2)
            {
                oss_ << "BFILE";
			}
        }
        else if (iCascadeBinaryOr(
            strcmp, szDataType   // Binary
            , "DATE"
            , "BLOB"
            , "CLOB"
            , "NCLOB"
            , "BFILE"
            , "ROWID"
            , "UNDEFINED"
            , "MLSLABEL"
        ) || iCascadeTernaryOr(
            strncmp, szDataType   // Ternary
            , "TIMESTAMP"
            , "INTERVAL"
            , "XMLTYPE"
            , "URITYPE"
            , "BINARY_FLOAT"
            , "BINARY_DOUBLE"
        ))
        {
            oss_ << szDataType;
        }
        else if (iCascadeTernaryOr(
            strncmp, szDataType   // Ternary
            , "ANYDATA"
            , "ANYTYPE"
            , "ANYDATASET"
        ))
        {
            oss_ << "SYS." << szDataType;
        }
        else
        {
            oss_ << boost::format(R"(UNSUPPORTTED("%s","%s","%s","%s"))")
               % szOwner % szTableName % szColumnName % szDataType;
            iCaps = false;
        }
        ret = oss_.str();
        if (iCaps)
        {
            // Make the alphabet spelling lowercase.
            std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
        }
    }
    return ps::lib::sGetMinimumWidth(ret, MINIMUM_DTYPE_LENGTH);
}

std::string cAllTabColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szTableName='%s', iColumnId=%u, szColumnName='%s', szDataType='%s')"
            R"(, iDataLength=%u, szCharUsed='%s'(%d), iDataPrecision=%u(%d))"
            R"(, iDataScale=%d(%d), szNullable='%s', szVirtualColumn='%s'(%d))"
            R"(, nPkColNameInd=%d, nFkColNameInd=%d)"
        ) % szOwner % szTableName % iColumnId % szColumnName % szDataType
            % iDataLength % szCharUsed % nCharUsedInd % iDataPrecision % nDataPrecisionInd
            % iDataScale % nDataScaleInd % szNullable % szVirtualColumn % nVirtualColumnInd
            % nPkColNameInd % nFkColNameInd
        ;
    return oss.str();
}

/**
 * @struct cAllTabColumns::cRetriever
 */
class cAllTabColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTabColumns::oList_.
    bool& iCloneable_;                  ///< It is used to check copiable.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    cKeyColumns& oKeyColumns_;   ///< Used to identify keys derived from other tables.
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , bool& iCloneable
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , cKeyColumns& oKeyColumns
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllTabColumns::cRetriever::iBulkSize_ = 1000;

cAllTabColumns::tAttributes cAllTabColumns::cRetriever::rTable_[cAllTabColumns::cRetriever::iBulkSize_];

cAllTabColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , bool& iCloneable
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , cKeyColumns& oKeyColumns
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTabColumns
        , "Reading column of tables", &oBind_
    )
    , oList_(oList)
    , iCloneable_(iCloneable)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , oKeyColumns_(oKeyColumns)
{
    const auto later_9iR1  = oSvc.iCompDbVersion("0900",4) >= 0; // true: equal or later.
    const auto later_10iR1 = oSvc.iCompDbVersion("1001",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
		later_9iR1 ? szNewFeauturedCharLength : szUsualCharLength
		, later_10iR1 ? szVirtualColumnEnable : szVirtualColumnDisable
		, later_10iR1 ? "all_tab_cols" : "all_tab_columns"
        , sGetSqlInList(oOwners)
		, later_10iR1 ? "t1.hidden_column = 'NO'" : "0=0"
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDataType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iDataLength, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szCharUsed, SQLT_STR, &rTable_->nCharUsedInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iDataPrecision, SQLT_UIN, &rTable_->nDataPrecisionInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iDataScale, SQLT_INT, &rTable_->nDataScaleInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szNullable, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szVirtualColumn, SQLT_STR, &rTable_->nVirtualColumnInd, nullptr, nullptr, iSkip_);
}

void cAllTabColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTabColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    std::for_each(&rTable_[0], &rTable_[iNumIter]
        , [this](tAttributes& rCur)
        {
            rCur.nPkColNameInd = oKeyColumns_.iDoesConstraintHas(
                std::make_tuple(rCur.szOwner, rCur.szTableName, rCur.szColumnName, "P")
            );
            rCur.nFkColNameInd = oKeyColumns_.iDoesConstraintHas(
                std::make_tuple(rCur.szOwner, rCur.szTableName, rCur.szColumnName, "R")
            );
        }
    );
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllTabColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iCloneable_ = true;
}

const char cAllTabColumns::szAllTabColumns[] = {
"select t1.owner "
", t1.table_name "
", t1.column_id "
", t1.column_name "
", t1.data_type "
", %s "
", t1.data_precision "
", t1.data_scale "
", t1.nullable "
", %s "
"from %s t1 "
"where t1.owner in %s "
"and %s"
};

const char cAllTabColumns::szUsualCharLength[] = { // less then Oracle 9i
"t1.data_length, 'B' as char_used"
};
const char cAllTabColumns::szNewFeauturedCharLength[] = { // more then Oracle 9i
"decode(t1.data_type "
	", 'NCHAR', t1.char_length "
	", 'NVARCHAR2', t1.char_length "
	", 'RAW', t1.data_length "
	", decode(t1.char_used, 'B', t1.data_length, t1.char_length) "
") as data_length, t1.char_used"
};
const char cAllTabColumns::szVirtualColumnDisable[] = { // less than Oracle 10g R1
	"NULL as virtual_column "
};
const char cAllTabColumns::szVirtualColumnEnable[] = { // more than Oracle 10g R1
	"t1.virtual_column "
};

cAllTabColumns::cAllTabColumns(
    const bool& iDependent
    , const int32_t& iLongTransit
    , cDataDefault& oDataDefault
)
    : iCloneable_(false)
    , iDependent_(iDependent)
    , iLongTransit_(iLongTransit)
    , oDataDefault_(oDataDefault)
{
    // nothing to do
}

cAllTabColumns::cAllTabColumns(
    const cAllTabColumns& rhs
)
    : oList_(rhs.oList_)
    , iCloneable_(rhs.iCloneable_)
    , iDependent_(rhs.iDependent_)
    , iLongTransit_(rhs.iLongTransit_)
    , oDataDefault_(rhs.oDataDefault_)
{
    ASSERT_OR_RAISE(
        rhs.iCloneable_, std::runtime_error
        , std::string("Object is not cloneable. Because SQL-select has not been run.")
    );
    // nothing to do
}

cAllTabColumns::~cAllTabColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTabColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , cKeyColumns& oKeyColumns
){
    return new cRetriever(
        oList_
        , iCloneable_
        , oSvc
        , oOwners
        , oKeyColumns
    );
}

void cAllTabColumns::vPrintAllTabColumns(
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
        , &cAllTabColumns::tAttributes::iKeyMatched
        , &cAllTabColumns::tAttributes::vPrintTabColumn
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

void cAllTabColumns::vPrintAllViewColumns(
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
        , &cAllTabColumns::tAttributes::iKeyMatched
        , &cAllTabColumns::tAttributes::vPrintViewColumn
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllTabColumns::vPrintPostRowsView
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

void cAllTabColumns::vPrintPostRowsView(std::ostream& os, const tAttributes&) const
{
    os << std::string(") ");
}

uint32_t cAllTabColumns::iGetColumnId(
    const char* szOwner
    , const char* szTableName
    , const char* szColumnName
) const
{
    const auto ite = oList_.end();
    const auto it = std::find_if(
        oList_.begin(), ite
        , [&] (const tAttributes& rCur)
        {
            return ps::lib::composite_equal(
                szOwner, rCur.szOwner
                , szTableName, rCur.szTableName
                , szColumnName, rCur.szColumnName
            );
        }
    );
    return it != ite ? it->iColumnId : 9999u;
}

const char* cAllTabColumns::szGetColumnName(
    const char* szOwner
    , const char* szTableName
    , const uint32_t iColumnId
) const
{
    const auto ite = oList_.end();
    const auto it = std::find_if(
        oList_.begin(), ite
        , [&] (const tAttributes& rCur)
        {
            return ps::lib::composite_equal(
                szOwner, rCur.szOwner
                , szTableName, rCur.szTableName
                , iColumnId, rCur.iColumnId
            );
        }
    );
    return it != ite ? it->szColumnName : "";
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

