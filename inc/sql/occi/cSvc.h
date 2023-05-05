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

namespace lib
{

namespace sql
{

namespace occi
{

/**
 * @class cSvc
 * @brief 
 *
 */
class cSvc
{
public:
    /**
     * @enum tTypeOfPool
     * @brief Determines which kind of instance does oMakeSvc() 
     *   makes and retunrs.
     */
    enum tTypeOfPool
    {
        iDedicated  ///< makes @ref cSvcDedicatedPool 
        , iShared   ///< makes @ref cSvcPooling
        , iNormal   ///< makes @ref cSvcNormal
    };
    /**
     * @typedef tPtr
     * @brief Determines which kind of instance does oMakeSvc() 
     */
    typedef std::unique_ptr<cSvc> tPtr;
    /**
     * @param[in] iTypeOfPool
     *   determins what kind of derived class instance is made.<br/>
     *   Pass one of the following.
     * - @ref iDedicated   makes cSvcDedicatedPool.<br/>
     *   <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#i1120739">
     *     oracle::occi::Environment::createEnvironment</a>
     *   and  <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#CHEEGFAI">
     *     oracle::occi::Environment::createConnection</a>
     *   are internally called iNumConn times.
     * - @ref iShared      makes cSvcPooling.<br/>
     *   <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#i1120739">
     *     oracle::occi::Environment::createEnvironment</a>
     *   and  <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#i1090287">
     *     oracle::occi::Environment::createConnectionPool</a>
     *   are internally called at once.
     * - @ref iNormal      makes cSvcNormal.<br/>
     *   <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#i1120739">
     *     oracle::occi::Environment::createEnvironment</a>
     *   and  <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference014.htm#CHEEGFAI">
     *     oracle::occi::Environment::createConnection</a>
     *   are internally called at once.
     * @param[in] user
     * - Pass a user-id which is one of the database accounts.
     * @param[in] passwd
     * - Pass an entity to prove qualification of the user.
     * @param[in] db
     * - Pass an Oracle Net Service name.
     * @param[in] iNumConn
     * - Maximaum number of connections kept in the pool.<br/>
     * - When either @ref iDedicated or @ref iShared are passed to iTypeOfPoo, this is referenced.
     * - Pass an integer greater than or equal to zero.
     * @param[in] iMinConn
     * - At least this number of connections is activated.<br/>
     * - When @ref iShared is just passed to iTypeOfPoo, this is referenced.
     * - Pass an integer greater than or equal to iNumConn.
     * @return
     */
    static cSvc * oMakeSvc(
        const tTypeOfPool& iTypeOfPool
        , const std::string& user
        , const std::string& passwd
        , const std::string& db
        , const uint32_t iNumConn = 0
        , const uint32_t iMinConn = 0
    );
    virtual ~cSvc()
    {}
    virtual oracle::occi::Connection* oGetCon() =0;
    virtual void vRelCon(oracle::occi::Connection* conn) =0;
    virtual std::string sGetServerVersion() =0;
    virtual void vShowServerVersion() =0;
    virtual void vAddSessionParam(
        const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
        , const std::string& sKey
        , const std::string& sValue
    ) =0;
    virtual void vModSessionParam(
        const std::string& sKey
        , const std::string& sValue
    ) =0;
    virtual void vDelSessionParam(
        const std::string& sKey
    ) =0;
    virtual std::string sGetSchemaName() const =0;
    virtual std::string sGetNetServiceName() const =0;
    virtual const int32_t iCompDbVersion(const std::string& str, const int32_t len) const =0;
    static uint32_t iGetDbBlockSize();
};

/**
 * @struct cConnDeleter
 * @brief A functor to release a pointer to 
 * <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference010.htm#i1118650">oracle::occi::Connection</a>.
 */
struct cConnDeleter
{
    typedef oracle::occi::Connection* pointer;
    typedef ps::lib::sql::occi::cSvc& type_of_context; ///< ps::lib::sql::occi::cSvc is abstruct.
    type_of_context ctx_;
    cConnDeleter(type_of_context ctx) : ctx_(ctx)
    {}
    void operator()(pointer ptr)
    {
        ctx_.vRelCon(ptr);
    }
};

/**
 * @struct cStmtDeleter
 * @brief A functor to terminate a pointer to 
 * <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference030.htm#i1079052">oracle::occi::Statement</a>.
 */
struct cStmtDeleter
{
    typedef oracle::occi::Statement* pointer;
    typedef std::unique_ptr<oracle::occi::Connection, ps::lib::sql::occi::cConnDeleter> type_of_context;
    type_of_context& ctx_;
    cStmtDeleter(type_of_context& ctx) : ctx_(ctx)
    {}
    void operator()(pointer ptr)
    {
        if (ptr) ctx_->terminateStatement(ptr);
    }
};

/**
 * @struct cRsDeleter
 * @brief A functor to close a pointer to 
 * <a href="https://docs.oracle.com/cd/E57425_01/121/LNCPP/reference027.htm#i1079050">oracle::occi::ResultSet</a>.
 */
struct cRsDeleter
{
    typedef oracle::occi::ResultSet* pointer;
    typedef std::unique_ptr<oracle::occi::Statement, ps::lib::sql::occi::cStmtDeleter> type_of_context;
    type_of_context& ctx_;
    cRsDeleter(type_of_context& ctx) : ctx_(ctx)
    {}
    void operator()(pointer ptr)
    {
        if (ptr) ctx_->closeResultSet(ptr);
    }
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

