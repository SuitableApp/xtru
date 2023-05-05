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

namespace ps
{

namespace lib
{

namespace sql
{

namespace lite3
{

/**
 * @class cAttrImpl
 *   Present low-level common processing for use by derived classes.
 */
class cAttrImpl
{
protected:
    /**
     * - Used to retrieve diagnostic information
     *   when the sqlite3 function group returns an error.
     * - See cCheckErr::operator()() to study detail.
     */
    cSqliteDb& oDb_;
    /**
     * A handle for sqlite3_bind_*() or sqlite3_column_*()
     */
    sqlite3_stmt* sqlite3_stmt_;
    /**
     * - The position of the place folder (sqlite3_bind_*) or
     *   the column of the select statement (sqlite3_column_*)
     *   is represented by an integer.
     *   -# In case of position of bind variable, it is originates from 1.
     *   -# However, in case of column position, it is originates from 0.
     */
    const int32_t pos_;
    /**
     * represents length of data when cAttr::tLite3Type is STR or RAW.
     */
    const size_t iDLength_;
private:
    /**
     * Indicates the address of the member of
     * the first array element.
     */
    void * pData_;
    /**
     * The number of bytes indicating the interval
     * to the next pData_ element.
     */
    size_t iDSkip_;
    /**
     *   Indicates the indicator variable address
     *   of the member of the first array element.
     *   If the bound column is explicitly is not null
     *   , then nullptr may be passed.
     */
    const ps::lib::sql::ind_t* iInd_;
    /**
     * The number of bytes indicating the interval
     * to the next iInd_ element.
     */
    size_t iISkip_;
protected:
    /**
     * @brief
     *
     * @param[in] oStmt
     *   An envelopes for retrieving cAttrImpl::oDb_
     *   and cAttrImpl::sqlite3_stmt_.
     * @param[in] pos
     *   @copydetails cAttrImpl::pos_
     * @param[in] iDLength
     *   @copydetails cAttrImpl::iDLength_
     * @param[in] pData
     *   @copydetails cAttrImpl::pData_
     * @param[in] iInd
     *   @copydetails cAttrImpl::iInd_
     * @param[in] iDSkip
     *   @copydetails cAttrImpl::iDSkip_
     * @param[in] iISkip
     *   @copydetails cAttrImpl::iISkip_
     */
    cAttrImpl(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : oDb_(oStmt.oGetDb())
        , sqlite3_stmt_(oStmt.oGetHndl())
        , pos_(pos)
        , iDLength_(iDLength)
        , pData_(pData)
        , iDSkip_(iDSkip)
        , iInd_(iInd)
        , iISkip_(iISkip)
    {}
    ~cAttrImpl() =default;
    ps::lib::sql::ind_t iGetInd(const size_t& i) const
    {
        return iInd_
            ? (ps::lib::sql::ind_t)((char *) iInd_)[i * iISkip_]
            : ps::lib::sql::ind_t::VAL_IS_NOTNULL
        ;
    }
    void vSetInd(const size_t& i, const ps::lib::sql::ind_t& n)
    {
        if (iInd_)
        {
            *((ps::lib::sql::ind_t *)&((char *) iInd_)[i * iISkip_]) = n;
        }
    }
    template<typename U>
    const U& oGetVal(const size_t& i) const
    {
        return *((const U *)&((const char *) pData_)[i * iDSkip_]);
    }
    template<typename U>
    void vSetVal(const size_t& i, const U& v)
    {
        *((U *)&((const char *) pData_)[i * iDSkip_]) = v;
    }
    template<typename U>
    const U* pGetPtr(const size_t& i) const
    {
        return &((const U *) pData_)[i * iDSkip_];
    }
    template<typename U>
    void vSetPtr(const size_t& i, const U* v, const size_t l)
    {
        U *dst = &((U *) pData_)[i * iDSkip_];
        ::memcpy(dst, v, l);
        dst[l] = '\0';
    }
private:
    cAttrImpl(const cAttrImpl&) =delete;
    cAttrImpl& operator=(const cAttrImpl&) =delete;
};

class cAttrImplInt32
    : public cAttr
    , private cAttrImpl
{
private:
public:
    /**
     * @brief
     * @copydoc cAttrImpl::cAttrImpl()
     */
    cAttrImplInt32(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : cAttrImpl(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip)
    {}
    /**
     * @brief
     * @copydoc cAttr::vBind()
     */
    virtual void vBind(const size_t& i) const
    {
        int32_t rc = (iGetInd(i) == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? sqlite3_bind_null(sqlite3_stmt_, pos_)
            : sqlite3_bind_int(sqlite3_stmt_, pos_, oGetVal<int32_t>(i));
        ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(oDb_));
    }
    /**
     * @brief
     * @copydoc cAttr::vPick()
     */
    virtual void vPick(const size_t& i)
    {
        if (sqlite3_column_type(sqlite3_stmt_, pos_) != SQLITE_NULL)
        {
            vSetVal(i, sqlite3_column_int(sqlite3_stmt_, pos_));
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NOTNULL);
        }
        else
        {
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NULL);
        }
    }
    virtual ~cAttrImplInt32()
    {}
};

class cAttrImplInt64
    : public cAttr
    , private cAttrImpl
{
private:
public:
    /**
     * @brief
     * @copydoc cAttrImpl::cAttrImpl()
     */
    cAttrImplInt64(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : cAttrImpl(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip)
    {}
    /**
     * @brief
     * @copydoc cAttr::vBind()
     */
    virtual void vBind(const size_t& i) const
    {
        int32_t rc = (iGetInd(i) == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? sqlite3_bind_null(sqlite3_stmt_, pos_)
            : sqlite3_bind_int64(sqlite3_stmt_, pos_, oGetVal<sqlite3_int64>(i));
        ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(oDb_));
    }
    /**
     * @brief
     * @copydoc cAttr::vPick()
     */
    virtual void vPick(const size_t& i)
    {
        if (sqlite3_column_type(sqlite3_stmt_, pos_) != SQLITE_NULL)
        {
            vSetVal(i, sqlite3_column_int64(sqlite3_stmt_, pos_));
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NOTNULL);
        }
        else
        {
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NULL);
        }
    }
    virtual ~cAttrImplInt64()
    {}
};

class cAttrImplFlt
    : public cAttr
    , private cAttrImpl
{
private:
public:
    /**
     * @brief
     * @copydoc cAttrImpl::cAttrImpl()
     */
    cAttrImplFlt(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : cAttrImpl(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip)
    {}
    /**
     * @brief
     * @copydoc cAttr::vBind()
     */
    virtual void vBind(const size_t& i) const
    {
        int32_t rc = (iGetInd(i) == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? sqlite3_bind_null(sqlite3_stmt_, pos_)
            : sqlite3_bind_double(sqlite3_stmt_, pos_, oGetVal<double>(i));
        ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(oDb_));
    }
    /**
     * @brief
     * @copydoc cAttr::vPick()
     */
    virtual void vPick(const size_t& i)
    {
        if (sqlite3_column_type(sqlite3_stmt_, pos_) != SQLITE_NULL)
        {
            vSetVal(i, sqlite3_column_double(sqlite3_stmt_, pos_));
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NOTNULL);
        }
        else
        {
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NULL);
        }
    }
    virtual ~cAttrImplFlt()
    {}
};

class cAttrImplStr
    : public cAttr
    , private cAttrImpl
{
private:
public:
    /**
     * @brief
     * @copydoc cAttrImpl::cAttrImpl()
     */
    cAttrImplStr(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : cAttrImpl(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip)
    {}
    /**
     * @brief
     * @copydoc cAttr::vBind()
     */
    virtual void vBind(const size_t& i) const
    {
        int32_t rc = (iGetInd(i) == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? sqlite3_bind_null(sqlite3_stmt_, pos_)
            : sqlite3_bind_text(sqlite3_stmt_, pos_, pGetPtr<char>(i), iDLength_, SQLITE_STATIC);
        ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(oDb_));
    }
    /**
     * @brief
     * @copydoc cAttr::vPick()
     */
    virtual void vPick(const size_t& i)
    {
        if (sqlite3_column_type(sqlite3_stmt_, pos_) != SQLITE_NULL)
        {
            const size_t iActual = sqlite3_column_bytes(sqlite3_stmt_, pos_);
            ASSERT_OR_RAISE_FNC(iActual < iDLength_, std::runtime_error, cCheckErr(oDb_));
            vSetPtr(i, sqlite3_column_text(sqlite3_stmt_, pos_), iActual);
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NOTNULL);
        }
        else
        {
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NULL);
        }
    }
    virtual ~cAttrImplStr()
    {}
};

class cAttrImplRaw
    : public cAttr
    , private cAttrImpl
{
private:
public:
    /**
     * @brief
     * @copydoc cAttrImpl::cAttrImpl()
     */
    cAttrImplRaw(
        cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , void * pData
        , const ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    )
        : cAttrImpl(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip)
    {}
    /**
     * @brief
     * @copydoc cAttr::vBind()
     */
    virtual void vBind(const size_t& i) const
    {
        int32_t rc = (iGetInd(i) == ps::lib::sql::ind_t::VAL_IS_NULL)
            ? sqlite3_bind_null(sqlite3_stmt_, pos_)
            : sqlite3_bind_blob(sqlite3_stmt_, pos_, pGetPtr<uint8_t>(i), iDLength_, SQLITE_STATIC);
        ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(oDb_));
    }
    /**
     * @brief
     * @copydoc cAttr::vPick()
     */
    virtual void vPick(const size_t& i)
    {
        if (sqlite3_column_type(sqlite3_stmt_, pos_) != SQLITE_NULL)
        {
            const size_t iActual = sqlite3_column_bytes(sqlite3_stmt_, pos_);
            ASSERT_OR_RAISE_FNC(iActual <= iDLength_, std::runtime_error, cCheckErr(oDb_));
            vSetPtr(i, (uint8_t *) sqlite3_column_blob(sqlite3_stmt_, pos_), iActual);
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NOTNULL);
        }
        else
        {
            vSetInd(i, ps::lib::sql::ind_t::VAL_IS_NULL);
        }
    }
    virtual ~cAttrImplRaw()
    {}
};

cAttr::~cAttr()
{}

cAttr * cAttr::oMakeInstance(
    const std::string& tag
    , cSqliteStmt& oStmt
    , const int32_t& pos
    , const int32_t& iDLength
    , const tLite3Type& iType
    , void * pData
    , const ps::lib::sql::ind_t* iInd
    , const size_t& iDSkip
    , const size_t& iISkip
){
    BOOST_ASSERT(pos >= 0);
    BOOST_ASSERT(pData);
    BOOST_ASSERT(0 != iDLength);
    BOOST_ASSERT(iDSkip >= 0);
    BOOST_ASSERT(iDLength >= 0 || iType == cAttr::tLite3Type::STR);
    BOOST_ASSERT(iType != cAttr::tLite3Type::INT32 || iDLength == sizeof(int32_t));
    BOOST_ASSERT(iType != cAttr::tLite3Type::INT64 || iDLength == sizeof(int64_t));
    cAttr *oAttr = nullptr;
    switch (iType)
    {
    case INT32:
        BOOST_ASSERT(iDLength == sizeof(int32_t));
        oAttr = new cAttrImplInt32(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip);
        break;
    case INT64:
        BOOST_ASSERT(iDLength == sizeof(sqlite3_int64));
        oAttr = new cAttrImplInt64(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip);
        break;
    case DOUBLE:
        BOOST_ASSERT(iDLength == sizeof(double));
        oAttr = new cAttrImplFlt(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip);
        break;
    case STR:
        oAttr = new cAttrImplStr(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip);
        break;
    case RAW:
        BOOST_ASSERT(iDLength >= 0);
        oAttr = new cAttrImplRaw(oStmt, pos, iDLength, pData, iInd, iDSkip, iISkip);
        break;
    }
    ASSERT_OR_RAISE(nullptr != oAttr, std::runtime_error, boost::format
        ("%s;%d: iType=%d has not supported.") % tag % pos % iType);
    return oAttr;
}

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

