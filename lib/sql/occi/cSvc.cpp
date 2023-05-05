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

namespace occi
{

/**
 * @class cSvcImpl
 * @brief Common data structure for the connection pool object.
 */

class cSvcImpl
    : private boost::noncopyable
{
protected:
    const ps::lib::cConfigures& conf_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    std::string user_;
    std::string passwd_;
    std::string db_;
    /**
     * cFailover is a class that provides objects to prepare for
     * accidental instance failures or process failures occurring on the server side.
     * Even if process succeed in TAF, the connection will never be fully restored. <br/>
     * That is, contexts such as locale and format mask are cleared.<br/>
     * cFailover is responsible for building session parameters  that
     * it wishes to be restored * beforehand and enabling reinitialization
     * of the connection after successful TAF.<br/>
     * To prepare to be able to reinitilize session after TAF.
     */
    ps::lib::sql::occi::cFailover oFailover_;
    /** Utility class for low-level memory allocation for use by OCI.
     * The role of this class is to aim for efficient operation
     * by supplying pooled memory resources.
     */
    std::unique_ptr<ps::lib::sql::occi::cAllocator> alc_;
    std::string db_version;  ///< formed 9999999999
    std::string cl_version;  ///< formed 9999999999 (But it has not implemented yet)
    cSvcImpl(
        const std::string& user
        , const std::string& passwd
        , const std::string& db
    )
        : conf_(ps::lib::cConfigures::get_const_instance())
        , cout_(ps::lib::cConsole::get_mutable_instance())
        , trc_(ps::lib::cTracer::get_mutable_instance())
        , mos_(ps::lib::cDistributor::get_mutable_instance())
        , user_(user)
        , passwd_(passwd)
        , db_(db)
        , alc_(new ps::lib::sql::occi::cAllocator)
    {
        mos_ << boost::format("Attempting to connect to %s as user %s")
            % (db_.empty() ? "[LOCAL_INSTANCE]" : db_)
            % user_ << std::endl;
        // Setting session parameters.
        if (conf_.is_value("date_mask"))
        {
            oFailover_.oAddParam(
                ps::lib::sql::occi::cFailover::tStmtType::STR_PARAM
                , "NLS_DATE_FORMAT", conf_.as<std::string>("date_mask")
            );
        }
        if (conf_.is_value("timestamp_mask"))
        {
            oFailover_.oAddParam(
                ps::lib::sql::occi::cFailover::tStmtType::STR_PARAM
                , "NLS_TIMESTAMP_FORMAT", conf_.as<std::string>("timestamp_mask")
            );
        }
        if (conf_.is_value("timestamp_tz_mask"))
        {
            oFailover_.oAddParam(
                ps::lib::sql::occi::cFailover::tStmtType::STR_PARAM
                , "NLS_TIMESTAMP_TZ_FORMAT", conf_.as<std::string>("timestamp_tz_mask")
            );
        }
        if (conf_.is_value("events_10046"))
        {
            oFailover_.oAddParam(
                ps::lib::sql::occi::cFailover::tStmtType::EVENTS
                , "10046", conf_.as<std::string>("events_10046")
            );
        }
    }
    ~cSvcImpl()
    {
        mos_ << std::string("Released environmental resources.") << std::endl;
    }
    void vSetDbVersion(const std::string& str)
    {
        /**
         * show database version string.
         */
        boost::regex re(R"((\d+)\.(\d+)\.(\d+)\.(\d+)\.(\d+))");  // matches "11.2.0.1.0"
        static int32_t subm[] = {1,2,3,4,5};
        boost::sregex_token_iterator it(str.cbegin(), str.cend(), re, subm);
        boost::sregex_token_iterator ite;
        ps::lib::cVector<int32_t> seg;
        while (it != ite)
        {
            seg.push_back(boost::lexical_cast<int32_t>(it->str()));
            ++it;
        }
        db_version = (
            boost::format("%02d%02d%02d%02d%02d")
                % seg[0] % seg[1] % seg[2] % seg[3] % seg[4]
        ).str();
    }
    void vShowBannerAndVersStr(const std::string& str)
    {
        // Example of str:
        // Oracle Database 11g Enterprise Edition Release 11.2.0.1.0 - 64bit Production
        // With the Partitioning, OLAP, Data Mining and Real Application Testing options
        /**
         * show database banner. New-line character will be removed in the str.
         */
        std::string sNewStr = str;
        std::replace(sNewStr.begin(), sNewStr.end(), '\n', ' ');
        mos_ << sNewStr << std::endl;
        vSetDbVersion(str);
        trc_ << boost::format("db_version is %s") % db_version << std::endl;
    }
    const int32_t iCompDbVersion(const std::string& str, const int32_t len) const
    {
        // ex.) pair of str and len:
        // "1002", 4 .... Compare wheather version is Oracle 10g Release 2 or not.
        // "1201", 4 .... Compare wheather version is Oracle 12g Release 1 or not.
        // str has 10 numeric characters. But usually it is not used after the fifth character.
        ASSERT_OR_RAISE(str.size() >= static_cast<size_t>(len), std::runtime_error, "len is too large.");
        return db_version.compare(0, len, str);
    }
private:
};

/*
 * @class cSvcDedicatedPool
 * Dedicated connection pool service
 */

class cSvcDedicatedPool
    : private cSvcImpl
    , public cSvc
    , private ps::lib::cSemaphore
{
public:
    cSvcDedicatedPool(
        const std::string& user
        , const std::string& passwd
        , const std::string& db
        , const uint32_t iNumConn
    );
private:
    std::unique_ptr<ps::lib::sql::occi::cAllocator[]> alc_;
    typedef ps::lib::cVector<ps::lib::sql::occi::tOcciEnv> tOcciEnvVect;
    tOcciEnvVect env_vct_;
    tOcciEnvVect oBuildEnv(uint32_t iNumConn)
    {
        const auto mode_(oracle::occi::Environment::Mode(
            oracle::occi::Environment::THREADED_UNMUTEXED
            | oracle::occi::Environment::OBJECT
        ));
        tOcciEnvVect env;
        for (uint32_t i = 0; i < iNumConn; i++)
        {
            env.push_back(tOcciEnvVect::value_type(
                oracle::occi::Environment::createEnvironment(
                    mode_, &alc_[i], maloc, raloc, mfree
                )
            ));
        }
        return env;
    }
    typedef ps::lib::cVector<oracle::occi::Connection*> tOcciConnVect;
    tOcciConnVect conn_;
    tOcciConnVect oBuildConn(uint32_t iNumConn){
        tOcciConnVect conn;
        for (uint32_t i = 0; i < iNumConn; i++)
        {
            // Pushed item that is a pointer to oracle::occi::Connection does not have to
            // release explicitly by oracle::occi::Environment::terminateConnection()
            // at destroy. Because these are released recursively
            // by invokation oracle::occi::Environment::terminateEnvironment().
            try
            {
                conn.push_back(env_vct_[i]->createConnection(user_, passwd_, db_));
            }
            catch (const oracle::occi::SQLException& ex)
            {
                RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
            }
        }
        return conn;
    }
    virtual ~cSvcDedicatedPool();
    virtual oracle::occi::Connection* oGetCon();
    virtual void vRelCon(oracle::occi::Connection* conn);
    virtual std::string sGetServerVersion();
    virtual void vShowServerVersion();
    virtual void vAddSessionParam(
        const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
        , const std::string& sKey
        , const std::string& sValue
    );
    virtual void vModSessionParam(
        const std::string& sKey
        , const std::string& sValue
    );
    virtual void vDelSessionParam(
        const std::string& sKey
    );
    virtual std::string sGetSchemaName() const
    {
        return user_;
    }
    virtual std::string sGetNetServiceName() const
    {
        return db_;
    }
    virtual const int32_t iCompDbVersion(const std::string& str, const int32_t len) const
    {
        return cSvcImpl::iCompDbVersion(str, len);
    }
    struct cDeleter
    {
        void operator()(oracle::occi::Connection*)
        {
        /*
         * Nothing to do.
         * Because the Connection object is released implicitly and recursively
         * when the Environment object it depends on is released.
         */
        }
    };
    ps::lib::cPool<oracle::occi::Connection, cDeleter> pool_;
};

cSvcDedicatedPool::cSvcDedicatedPool(
    const std::string& user
    , const std::string& passwd
    , const std::string& db
    , const uint32_t iNumConn
)
    : cSvcImpl(user, passwd, db)
    , cSemaphore(iNumConn)
    , alc_(new ps::lib::sql::occi::cAllocator[iNumConn])
    , env_vct_(oBuildEnv(iNumConn))
    , conn_(oBuildConn(iNumConn))
    , pool_(conn_)
{
    trc_ << std::string("cSvcDedicatedPool is invoked.") << std::endl;
}

cSvcDedicatedPool::~cSvcDedicatedPool()
{}

oracle::occi::Connection* cSvcDedicatedPool::oGetCon()
{
    vWait();
    oracle::occi::Connection* conn_ = pool_.oPop();
    oFailover_.vCheckout(conn_);
    return conn_;
}

void cSvcDedicatedPool::vRelCon(oracle::occi::Connection* conn)
{
    oFailover_.vCheckin(conn);
    pool_.vPush(conn);
    vNotify();
}

std::string cSvcDedicatedPool::sGetServerVersion()
{
    try
    {
        std::shared_ptr<oracle::occi::Connection> conn_(
            oGetCon(), [&](oracle::occi::Connection* conn){vRelCon(conn);}
        );
        vSetDbVersion(conn_->getServerVersion());
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
    return db_version;
}

void cSvcDedicatedPool::vShowServerVersion()
{
    try
    {
        std::shared_ptr<oracle::occi::Connection> conn_ (
            oGetCon()
            , [&](oracle::occi::Connection* conn){vRelCon(conn);}
        );
        vShowBannerAndVersStr(conn_->getServerVersion());
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
}

void cSvcDedicatedPool::vAddSessionParam(
    const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
    , const std::string& sKey
    , const std::string& sValue
){
    oFailover_.oAddParam(iStmtType, sKey, sValue);
}

void cSvcDedicatedPool::vModSessionParam(
    const std::string& sKey
    , const std::string& sValue
){
    oFailover_.vModParam(sKey, sValue);
}

void cSvcDedicatedPool::vDelSessionParam(
    const std::string& sKey
){
    oFailover_.vDelParam(sKey);
}

/*
 * Shared connection pool service
 */
class cSvcPooling
    : private cSvcImpl
    , public cSvc
    , private ps::lib::cSemaphore
{
public:
    cSvcPooling(
        const std::string& user
        , const std::string& passwd
        , const std::string& db
        , const uint32_t iNumConn
        , const uint32_t iMinConn
    );
private:
    virtual ~cSvcPooling();
    virtual oracle::occi::Connection* oGetCon();
    virtual void vRelCon(oracle::occi::Connection* conn);
    virtual std::string sGetServerVersion();
    virtual void vShowServerVersion();
    virtual void vAddSessionParam(
        const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
        , const std::string& sKey
        , const std::string& sValue
    );
    virtual void vModSessionParam(
        const std::string& sKey
        , const std::string& sValue
    );
    virtual void vDelSessionParam(
        const std::string& sKey
    );
    virtual std::string sGetSchemaName() const
    {
        return user_;
    }
    virtual std::string sGetNetServiceName() const
    {
        return db_;
    }
    virtual const int32_t iCompDbVersion(const std::string& str, const int32_t len) const
    {
        return cSvcImpl::iCompDbVersion(str, len);
    }
    tOcciEnv env_;           /// is implemented by smart pointer class.
    uint32_t iNumConn_;
    uint32_t iMinConn_;
    std::shared_ptr<oracle::occi::ConnectionPool> pool_;
};

cSvcPooling::cSvcPooling(
    const std::string& user
    , const std::string& passwd
    , const std::string& db
    , const uint32_t iNumConn
    , const uint32_t iMinConn
)
    : cSvcImpl(user, passwd, db)
    , cSemaphore(iNumConn)
    , env_(oracle::occi::Environment::createEnvironment(
        oracle::occi::Environment::Mode(
            oracle::occi::Environment::THREADED_MUTEXED
            | oracle::occi::Environment::OBJECT
        )
        , alc_.get(), maloc, raloc, mfree
    ))
    , iNumConn_(iNumConn), iMinConn_(iMinConn)
{
    trc_ << std::string("cSvcPooling is invoked.") << std::endl;
    try
    {
        pool_.reset(
            env_->createConnectionPool(user_, passwd_, db_, iMinConn_, iNumConn_, iNumConn_ - iMinConn_)
            , [&](oracle::occi::ConnectionPool* pool)
            {
                BOOST_ASSERT(env_);
                BOOST_ASSERT(pool);
                env_->terminateConnectionPool(pool);
            }
        );
        trc_ << std::string("Created an instance of cSvcPooling .") << std::endl;
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
    pool_->setTimeOut(120); // represents in second.
                           // Probation time until it is purged.
}

cSvcPooling::~cSvcPooling()
{}

oracle::occi::Connection* cSvcPooling::oGetCon()
{
    vWait();
    oracle::occi::Connection* conn_ = pool_->createConnection(user_, passwd_);
    oFailover_.vCheckout(conn_);
    return conn_;
}

void cSvcPooling::vRelCon(oracle::occi::Connection* conn)
{
    oFailover_.vCheckin(conn);
    pool_->terminateConnection(conn);
    vNotify();
}

std::string cSvcPooling::sGetServerVersion()
{
    try
    {
        std::shared_ptr<oracle::occi::Connection> conn_(
            oGetCon(), [&](oracle::occi::Connection* conn){vRelCon(conn);}
        );
        vSetDbVersion(conn_->getServerVersion());
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
    return db_version;
}

void cSvcPooling::vShowServerVersion()
{
    try
    {
        std::shared_ptr<oracle::occi::Connection> conn_ (
            oGetCon()
            , [&](oracle::occi::Connection* conn){vRelCon(conn);}
        );
        vShowBannerAndVersStr(conn_->getServerVersion());
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
}

void cSvcPooling::vAddSessionParam(
    const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
    , const std::string& sKey
    , const std::string& sValue
){
    oFailover_.oAddParam(iStmtType, sKey, sValue);
}

void cSvcPooling::vModSessionParam(
    const std::string& sKey
    , const std::string& sValue
){
    oFailover_.vModParam(sKey, sValue);
}

void cSvcPooling::vDelSessionParam(
    const std::string& sKey
){
    oFailover_.vDelParam(sKey);
}

/*
 * Normal type service
 */
class cSvcNormal
    : private cSvcImpl
    , public cSvc
{
public:
    cSvcNormal(
        const std::string& user
        , const std::string& passwd
        , const std::string& db
    );
private:
    virtual ~cSvcNormal();
    virtual oracle::occi::Connection* oGetCon();
    virtual void vRelCon(oracle::occi::Connection* conn);
    virtual std::string sGetServerVersion();
    virtual void vShowServerVersion();
    virtual void vAddSessionParam(
        const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
        , const std::string& sKey
        , const std::string& sValue
    );
    virtual void vModSessionParam(
        const std::string& sKey
        , const std::string& sValue
    );
    virtual void vDelSessionParam(
        const std::string& sKey
    );
    virtual std::string sGetSchemaName() const
    {
        return user_;
    }
    virtual std::string sGetNetServiceName() const
    {
        return db_;
    }
    virtual const int32_t iCompDbVersion(const std::string& str, const int32_t len) const
    {
        return cSvcImpl::iCompDbVersion(str, len);
    }
    tOcciEnv env_;           /// is implemented by smart pointer class.
    std::shared_ptr<oracle::occi::Connection> conn_;
};

cSvcNormal::cSvcNormal(
    const std::string& user
    , const std::string& passwd
    , const std::string& db
)
    : cSvcImpl(user, passwd, db)
    , env_(oracle::occi::Environment::createEnvironment(
        oracle::occi::Environment::Mode(
            oracle::occi::Environment::THREADED_MUTEXED
            | oracle::occi::Environment::OBJECT
        )
    ))
{
    trc_ << std::string("cSvcNormal is invoked.") << std::endl;
    try
    {
        conn_.reset(
            env_->createConnection(user_, passwd_, db_)
            , [&](oracle::occi::Connection* conn)
            {
                env_->terminateConnection(conn);
            }
        );
        trc_ << std::string("Created an instance of cSvcNormal.") << std::endl;
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
    }
}

cSvcNormal::~cSvcNormal()
{}

oracle::occi::Connection* cSvcNormal::oGetCon()
{
    oFailover_.vCheckout(conn_.get());
    return conn_.get();
}

void cSvcNormal::vRelCon(oracle::occi::Connection* conn)
{
    oFailover_.vCheckin(conn);
}

std::string cSvcNormal::sGetServerVersion()
{
    vSetDbVersion(conn_->getServerVersion());
    return db_version;
}

void cSvcNormal::vShowServerVersion()
{
    vShowBannerAndVersStr(conn_->getServerVersion());
}

void cSvcNormal::vAddSessionParam(
    const ps::lib::sql::occi::cFailover::tStmtType& iStmtType
    , const std::string& sKey
    , const std::string& sValue
){
    oFailover_.oAddParam(iStmtType, sKey, sValue);
}

void cSvcNormal::vModSessionParam(
    const std::string& sKey
    , const std::string& sValue
){
    oFailover_.vModParam(sKey, sValue);
}

void cSvcNormal::vDelSessionParam(
    const std::string& sKey
){
    oFailover_.vDelParam(sKey);
}

// -------------- End of the implementation --------------

cSvc * cSvc::oMakeSvc(
    const tTypeOfPool& iTypeOfPool
    , const std::string& user
    , const std::string& passwd
    , const std::string& db
    , const uint32_t iNumConn
    , const uint32_t iMinConn
){
    BOOST_ASSERT(iNumConn >= 0);
    BOOST_ASSERT(iNumConn >= iMinConn);
    cSvc *oSvc = nullptr;
    switch (iTypeOfPool)
    {
    case iDedicated:
        try
        {
            oSvc = new cSvcDedicatedPool(user, passwd, db, iNumConn);
        }
        catch (const oracle::occi::SQLException& ex)
        {
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
        break;
    case iShared:
        try
        {
            oSvc = new cSvcPooling(user, passwd, db, iNumConn, iMinConn);
        }
        catch (const oracle::occi::SQLException& ex)
        {
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
        break;
    case iNormal:
        try
        {
            oSvc = new cSvcNormal(user, passwd, db);
        }
        catch (const oracle::occi::SQLException& ex)
        {
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
        break;
    default:
        break;
    }
    ASSERT_OR_RAISE(nullptr != oSvc, std::runtime_error, boost::format
        ("ps::lib::sql::occi::cSvc::tTypeOfPool=%d has not supported.") % iTypeOfPool);
    return oSvc;
}

uint32_t cSvc::iGetDbBlockSize()
{
    static uint32_t iDbBlockSize = 8192;
    return iDbBlockSize;
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

