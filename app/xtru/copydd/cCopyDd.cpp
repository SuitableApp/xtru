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

namespace copydd
{

void cCopyDd::vFillTokensIntoVctInKeySpecFile(
    ps::lib::str_vct& vct
    , const std::string& key
){
    if (conf_.is_value(key) && conf_.length(key))
    {
        ps::lib::str_vct tmp;
        std::string line;
        boost::tokenizer< ps::lib::tSep > tokens_(line, rule_);
        boost::filesystem::path base = conf_.as<std::string>(key);
        boost::filesystem::path path(base.parent_path().empty() ? conf_.sGetPsHome() / conf_.as<std::string>("input") / base : base);
        ASSERT_OR_RAISE(boost::filesystem::exists(path), std::runtime_error, path.string() + " does not exists.");
        boost::filesystem::ifstream ifs(path);
        while (getline(ifs, line))
        {
            const std::string::size_type pos = line.find_first_of('#'); // '#' for a comment.
            if (pos != std::string::npos) line.resize(pos);
            if (line.empty()) continue;
            tokens_.assign(line, rule_);
            std::copy_if(tokens_.begin(), tokens_.end()
                , std::back_inserter(tmp)
                , [](const std::string& s){ return !s.empty(); }
            );
        }
        std::string header = boost::str(boost::format("Keyword (%s):") % key);
        trc_ << header << tmp << boost::format(" broughted by %s") % path << std::endl;
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(vct));
    }
}

cCopyDd::cCopyDd()
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , rule_("", " \t,", "\"")
    , oSvc_(nullptr)
    , oDb_(nullptr)
{}

cCopyDd::~cCopyDd()
{}

void cCopyDd::vAssociate(
    ps::lib::sql::occi::cSvc* oSvc
    , ps::lib::sql::lite3::cSqliteDb* oDb
){
    oSvc_ = oSvc;
    oDb_ = oDb;
}

void cCopyDd::vCreateRepo()
{
    BOOST_ASSERT(oDb_ != nullptr);
    vInitializeRepo(
        "ALL_USERS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllUsers::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllUsers::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_TABLES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTables::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTables::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_INDEXES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllIndexes::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllIndexes::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_TRIGGERS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTriggers::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTriggers::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_MVIEWS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllMviews::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllMviews::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_EXTERNAL_TABLES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllExternalTables::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllExternalTables::szDelStmt);}
    );
    vInitializeRepo(
        "COLUMN_OPTIONALITIES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cColumnOptionalities::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cColumnOptionalities::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_CONSTRAINTS"
        , [this](){return oDb_->iExecSql({
                ps::app::xtru::copydd::cAllConstraints::szCreStmt
                , ps::app::xtru::copydd::cAllConstraints::szIdxStmt
            });}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllConstraints::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_TAB_COLUMNS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabColumns::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabColumns::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_CONS_COLUMNS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllConsColumns::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllConsColumns::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_OBJECTS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllObjects::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllObjects::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_TAB_PARTITIONS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabPartitions::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabPartitions::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_PART_TABLES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllPartTables::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllPartTables::szDelStmt);}
    );
    vInitializeRepo(
        "ROWID_RANGES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cRowidRanges::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cRowidRanges::szDelStmt);}
    );
    vInitializeRepo(
        "TARGET_TABLES"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cTargetTables::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cTargetTables::szDelStmt);}
    );
    vInitializeRepo(
        "REFERENCE_GRAPH_TAB"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cReferenceGraphTab::szCreStmt);}
        , [](){return SQLITE_OK; /* means nothing todo. */ }
    );
    vInitializeRepo(
        "EFFECTIVE_CONS"
        , [this](){return oDb_->iExecSql({
                ps::app::xtru::copydd::cEffectiveCons::szCreStmt
                , ps::app::xtru::copydd::cEffectiveCons::szIdx1Stmt
                , ps::app::xtru::copydd::cEffectiveCons::szIdx2Stmt
            });}
        , [](){return SQLITE_OK; /* means nothing todo. */ }
    );
    vInitializeRepo(
        "REFERENCE_GRAPH_OBJ"
        , [this](){return oDb_->iExecSql({
                ps::app::xtru::copydd::cReferenceGraphObj::szCreStmt
                , ps::app::xtru::copydd::cReferenceGraphObj::szIdxStmt
            });}
        , [](){return SQLITE_OK; /* means nothing todo. */ }
    );
    vInitializeRepo(
        "USER_OBJECTS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cUserObjects::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cUserObjects::szDelStmt);}
    );
    vInitializeRepo(
        "ALL_TAB_PRIVS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabPrivs::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cAllTabPrivs::szDelStmt);}
    );
    vInitializeRepo(
        "SESSION_PRIVS"
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cSessionPrivs::szCreStmt);}
        , [this](){return oDb_->iExecSql(ps::app::xtru::copydd::cSessionPrivs::szDelStmt);}
    );

    mos_ << std::string("Repository was created successfully.") << std::endl;
}

class cRefresher
{
private:
    ps::lib::sql::occi::cSvc* oSvc_;
    ps::lib::sql::lite3::cSqliteDb* oDb_;
    const ps::lib::str_vct& oOwners_;
public:
    cRefresher(
        ps::lib::sql::occi::cSvc* oSvc
        , ps::lib::sql::lite3::cSqliteDb* oDb
        , const ps::lib::str_vct& oOwners
    )
        : oSvc_(oSvc), oDb_(oDb), oOwners_(oOwners)
    {}
    template <typename T>
    void operator()(T&) 
    {
        std::unique_ptr<ps::lib::sql::cFetchable> t(new typename T::element_type(*oSvc_, *oDb_, oOwners_));
        t->vExecuteAndFetch();
    }
};

ps::lib::str_vct cCopyDd::oConvEffectiveOwnerName(const ps::lib::str_vct& oWildOwners)
{
    BOOST_ASSERT(oSvc_ != nullptr);
    BOOST_ASSERT(oDb_ != nullptr);
    ps::lib::sql::lite3::cTransactional txn(*oDb_);
    
    // table: ALL_USERS
    std::unique_ptr<ps::lib::sql::cFetchable> oAllUsers(new cAllUsers(*oSvc_, *oDb_));
    oAllUsers->vExecuteAndFetch();
    oAllUsers.reset();

    // table: TARGET_OWNERS
    // Elements in oWildOwners is allowed to contain wild-card characters which examples for '%' or '_'.
    // And they may not necessarily exist in the database.
    cTargetOwners oTargetOwners(*oDb_, oWildOwners);
    oTargetOwners.vRun();
    // But elements in oWildOwners would be replaced into the actual schema name in the database.
    ps::lib::str_vct oOwners = oTargetOwners.oGetOwners();        /// Owners who has target tables.
    trc_ << std::string("Keyword (src_user): ") << oWildOwners << std::endl;
    trc_ << std::string("Actual users: ") << oOwners << std::endl;
    return oOwners;
}

void cCopyDd::vRefresGetDataRepo(const ps::lib::str_vct& oOwners)
{
    BOOST_ASSERT(oSvc_ != nullptr);
    BOOST_ASSERT(oDb_ != nullptr);
    ps::lib::sql::lite3::cTransactional txn(*oDb_);

    //
    // 'tSeqList' contains some classes with a same named virtual function.
    // (they are inherited an common abstruct class.)
    //
    typedef boost::mpl::transform<
        boost::mpl::list
        < cAllTables            // table: ALL_TABLES
        , cAllIndexes           // table: ALL_INDEXES
        , cAllTriggers          // table: ALL_TRIGGERS
        , cAllMviews            // table: ALL_MVIEWS
        , cAllExternalTables    // table: ALL_EXTERNAL_TABLES
        , cColumnOptionalities  // table: COLUMN_OPTIONALITIES
        , cAllConstraints       // table: ALL_CONSTRAINTS
        , cAllTabColumns        // table: ALL_TAB_COLUMNS
        , cAllConsColumns       // table: ALL_CONS_COLUMNS
        , cAllObjects           // table: ALL_OBJECTS
        , cAllTabPartitions     // table: ALL_TAB_PARTITIONS
        , cAllPartTables        // table: ALL_PART_TABLES
        , cRowidRanges          // table: ROWID_RANGES
        , cUserObjects          // table: USER_OBJECTS
        , cAllTabPrivs          // table: ALL_TAB_PRIVS
        , cSessionPrivs         // table: SESSION_PRIVS
        >
        , std::unique_ptr<boost::mpl::_1>
    >::type tSeqList;
    // 'cRefresher' is a functor. 
    // operator()() in cRefresher invokes vExecuteAndFetch() which is the virtual function 
    // of each items of typelist.
    cRefresher oRefresher(oSvc_, oDb_, oOwners);
    boost::mpl::for_each<tSeqList>(oRefresher);

    // Target tables name.
    ps::lib::str_vct oListTable_      /// Target tables for outputting .
                   , oListFixed_      /// Fixed length formatting targets (of selected above).
                   , oListExcpt_      /// Excluding targets (of selected above).
    ;
    boost::tokenizer< ps::lib::tSep > tokens_(conf_.as<std::string>("listtable"), rule_);
    oListTable_.assign(tokens_.begin(), tokens_.end());
    if (oListTable_.empty()) oListTable_.push_back("%");

    tokens_.assign(conf_.as<std::string>("listfixed"), rule_);
    oListFixed_.assign(tokens_.begin(), tokens_.end());

    tokens_.assign(conf_.as<std::string>("listexcpt"), rule_);
    oListExcpt_.assign(tokens_.begin(), tokens_.end());

    trc_ << std::string("Keyword (listtable):") << oListTable_ << std::endl;
    trc_ << std::string("Keyword (listfixed):") << oListFixed_ << std::endl;
    trc_ << std::string("Keyword (listexcpt):") << oListExcpt_ << std::endl;

    vFillTokensIntoVctInKeySpecFile(oListTable_, "filetable");
    vFillTokensIntoVctInKeySpecFile(oListFixed_, "filefixed");
    vFillTokensIntoVctInKeySpecFile(oListExcpt_, "fileexcpt");

    trc_ << std::string("total (*table):") << oListTable_ << std::string(" to select target tables.") << std::endl;
    trc_ << std::string("total (*fixed):") << oListFixed_ << std::string(" to select fixed-length targets.") << std::endl;
    trc_ << std::string("total (*excpt):") << oListExcpt_ << std::string(" to select exclusionary targets.") << std::endl;

    // table: TARGET_TABLES
    {
        cTargetTables oTargetTables(*oDb_, oListTable_, oListFixed_, oListExcpt_);
        oTableList_ = oTargetTables.oRmWhereNumRows(*oSvc_, conf_.as<int32_t>("num_rows"));
        if (oTableList_.size() == 0)
        {
            mos_ << std::string(
                "WARNING: No target table names. "
                "Check the value of the parameter listtable, listexcpt, filetable and fileexcpt."
            ) << std::endl;
        }
    }
    // table: EFFECTIVE_CONS
    {
        cReposRefresher<cEffectiveCons> oRCons(*oDb_, cEffectiveCons::B);
        oRCons.vRefresh();
    }

    // finish
    mos_ << std::string("Repository was refreshed successfully.") << std::endl;
}

const ps::app::xtru::cTableList& cCopyDd::oGetTableList(void) const
{
    return oTableList_;
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

