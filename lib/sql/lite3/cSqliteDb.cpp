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
#include <sqlite3ext.h>
#include <dlfcn.h>

#define PS_SQL_LITE3_SO_NAME  "libsqlite3.so"
#define IN_MEMORY_DATABASE         ":memory:"

namespace ps 
{

namespace lib
{

namespace sql
{

namespace lite3
{

std::unique_ptr<sqlite3_api_routines> sqlite3_api (new sqlite3_api_routines);
std::shared_ptr<void> hModule_(dlopen(PS_SQL_LITE3_SO_NAME,  RTLD_LAZY), [](void* hModule){ dlclose(hModule); });

void* vDlSym(const char* symbol)
{
    void* ret = ::dlsym(hModule_.get(), symbol);
    const char *szMsg = ::dlerror();
    ASSERT_OR_RAISE(
        szMsg == NULL
        , std::runtime_error
        , boost::format("symbol:%s, error_msg:%s") % symbol % szMsg
    );
    return ret;
}

std::once_flag oOnceLoadSymbols_;

void vActivateSymbols()
{
    ASSERT_OR_RAISE(hModule_, std::runtime_error, "Failed to load a shared library.");
    sqlite3_aggregate_context = (void * (*)(sqlite3_context*,int32_t nBytes)) vDlSym("sqlite3_aggregate_context");
    sqlite3_aggregate_count = (int32_t  (*)(sqlite3_context*)) vDlSym("sqlite3_aggregate_count");
    sqlite3_bind_blob = (int32_t  (*)(sqlite3_stmt*,int32_t,const void*,int32_t n,void(*)(void*))) vDlSym("sqlite3_bind_blob");
    sqlite3_bind_double = (int32_t  (*)(sqlite3_stmt*,int32_t,double)) vDlSym("sqlite3_bind_double");
    sqlite3_bind_int = (int32_t  (*)(sqlite3_stmt*,int32_t,int32_t)) vDlSym("sqlite3_bind_int");
    sqlite3_bind_int64 = (int32_t  (*)(sqlite3_stmt*,int32_t,sqlite_int64)) vDlSym("sqlite3_bind_int64");
    sqlite3_bind_null = (int32_t  (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_bind_null");
    sqlite3_bind_parameter_count = (int32_t  (*)(sqlite3_stmt*)) vDlSym("sqlite3_bind_parameter_count");
    sqlite3_bind_parameter_index = (int32_t  (*)(sqlite3_stmt*,const char*zName)) vDlSym("sqlite3_bind_parameter_index");
    sqlite3_bind_parameter_name = (const char * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_bind_parameter_name");
    sqlite3_bind_text = (int32_t  (*)(sqlite3_stmt*,int32_t,const char*,int32_t n,void(*)(void*))) vDlSym("sqlite3_bind_text");
    sqlite3_bind_text16 = (int32_t  (*)(sqlite3_stmt*,int32_t,const void*,int32_t,void(*)(void*))) vDlSym("sqlite3_bind_text16");
    sqlite3_bind_value = (int32_t  (*)(sqlite3_stmt*,int32_t,const sqlite3_value*)) vDlSym("sqlite3_bind_value");
    sqlite3_busy_handler = (int32_t  (*)(sqlite3*,int32_t(*)(void*,int32_t),void*)) vDlSym("sqlite3_busy_handler");
    sqlite3_busy_timeout = (int32_t  (*)(sqlite3*,int32_t ms)) vDlSym("sqlite3_busy_timeout");
    sqlite3_changes = (int32_t  (*)(sqlite3*)) vDlSym("sqlite3_changes");
    sqlite3_close = (int32_t  (*)(sqlite3*)) vDlSym("sqlite3_close");
    sqlite3_collation_needed = (int32_t  (*)(sqlite3*,void*,void(*)(void*,sqlite3*,int32_t eTextRep,const char*))) vDlSym("sqlite3_collation_needed");
    sqlite3_collation_needed16 = (int32_t  (*)(sqlite3*,void*,void(*)(void*,sqlite3*,int32_t eTextRep,const void*))) vDlSym("sqlite3_collation_needed16");
    sqlite3_column_blob = (const void * (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_blob");
    sqlite3_column_bytes = (int32_t  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_bytes");
    sqlite3_column_bytes16 = (int32_t  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_bytes16");
    sqlite3_column_count = (int32_t  (*)(sqlite3_stmt*pStmt)) vDlSym("sqlite3_column_count");
//  sqlite3_column_database_name = (const char * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_database_name");  /// has not provided in Sqlite3-3.8.8
//  sqlite3_column_database_name16 = (const void * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_database_name16");  /// has not provided in Sqlite3-3.8.8
    sqlite3_column_decltype = (const char * (*)(sqlite3_stmt*,int32_t i)) vDlSym("sqlite3_column_decltype");
    sqlite3_column_decltype16 = (const void * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_decltype16");
    sqlite3_column_double = (double  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_double");
    sqlite3_column_int = (int32_t  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_int");
    sqlite3_column_int64 = (sqlite_int64  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_int64");
    sqlite3_column_name = (const char * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_name");
    sqlite3_column_name16 = (const void * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_name16");
//  sqlite3_column_origin_name = (const char * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_origin_name");  /// has not provided in Sqlite3-3.8.8
//  sqlite3_column_origin_name16 = (const void * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_origin_name16");  /// has not provided in Sqlite3-3.8.8
//  sqlite3_column_table_name = (const char * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_table_name");  /// has not provided in Sqlite3-3.8.8
//  sqlite3_column_table_name16 = (const void * (*)(sqlite3_stmt*,int32_t)) vDlSym("sqlite3_column_table_name16");  /// has not provided in Sqlite3-3.8.8
    sqlite3_column_text = (const uint8_t * (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_text");
    sqlite3_column_text16 = (const void * (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_text16");
    sqlite3_column_type = (int32_t  (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_type");
    sqlite3_column_value = (sqlite3_value* (*)(sqlite3_stmt*,int32_t iCol)) vDlSym("sqlite3_column_value");
    sqlite3_commit_hook = (void * (*)(sqlite3*,int32_t(*)(void*),void*)) vDlSym("sqlite3_commit_hook");
    sqlite3_complete = (int32_t  (*)(const char*sql)) vDlSym("sqlite3_complete");
    sqlite3_complete16 = (int32_t  (*)(const void*sql)) vDlSym("sqlite3_complete16");
    sqlite3_create_collation = (int32_t  (*)(sqlite3*,const char*,int32_t,void*,int32_t(*)(void*,int32_t,const void*,int32_t,const void*))) vDlSym("sqlite3_create_collation");
    sqlite3_create_collation16 = (int32_t  (*)(sqlite3*,const void*,int32_t,void*,int32_t(*)(void*,int32_t,const void*,int32_t,const void*))) vDlSym("sqlite3_create_collation16");
    sqlite3_create_function = (int32_t  (*)(sqlite3*,const char*,int32_t,int32_t,void*,void (*xFunc)(sqlite3_context*,int32_t,sqlite3_value**),void (*xStep)(sqlite3_context*,int32_t,sqlite3_value**),void (*xFinal)(sqlite3_context*))) vDlSym("sqlite3_create_function");
    sqlite3_create_function16 = (int32_t  (*)(sqlite3*,const void*,int32_t,int32_t,void*,void (*xFunc)(sqlite3_context*,int32_t,sqlite3_value**),void (*xStep)(sqlite3_context*,int32_t,sqlite3_value**),void (*xFinal)(sqlite3_context*))) vDlSym("sqlite3_create_function16");
    sqlite3_create_module = (int32_t (*)(sqlite3*,const char*,const sqlite3_module*,void*)) vDlSym("sqlite3_create_module");
    sqlite3_data_count = (int32_t  (*)(sqlite3_stmt*pStmt)) vDlSym("sqlite3_data_count");
    sqlite3_db_handle = (sqlite3 * (*)(sqlite3_stmt*)) vDlSym("sqlite3_db_handle");
    sqlite3_declare_vtab = (int32_t (*)(sqlite3*,const char*)) vDlSym("sqlite3_declare_vtab");
    sqlite3_enable_shared_cache = (int32_t  (*)(int32_t)) vDlSym("sqlite3_enable_shared_cache");
    sqlite3_errcode = (int32_t  (*)(sqlite3*db)) vDlSym("sqlite3_errcode");
    sqlite3_errmsg = (const char * (*)(sqlite3*)) vDlSym("sqlite3_errmsg");
    sqlite3_errmsg16 = (const void * (*)(sqlite3*)) vDlSym("sqlite3_errmsg16");
    sqlite3_exec = (int32_t  (*)(sqlite3*,const char*,sqlite3_callback,void*,char**)) vDlSym("sqlite3_exec");
    sqlite3_expired = (int32_t  (*)(sqlite3_stmt*)) vDlSym("sqlite3_expired");
    sqlite3_finalize = (int32_t  (*)(sqlite3_stmt*pStmt)) vDlSym("sqlite3_finalize");
    sqlite3_free = (void  (*)(void*)) vDlSym("sqlite3_free");
    sqlite3_free_table = (void  (*)(char**result)) vDlSym("sqlite3_free_table");
    sqlite3_get_autocommit = (int32_t  (*)(sqlite3*)) vDlSym("sqlite3_get_autocommit");
    sqlite3_get_auxdata = (void * (*)(sqlite3_context*,int32_t)) vDlSym("sqlite3_get_auxdata");
    sqlite3_get_table = (int32_t  (*)(sqlite3*,const char*,char***,int32_t*,int32_t*,char**)) vDlSym("sqlite3_get_table");
    sqlite3_global_recover = (int32_t  (*)(void)) vDlSym("sqlite3_global_recover");
    sqlite3_interrupt = (void  (*)(sqlite3*)) vDlSym("sqlite3_interrupt");
    sqlite3_last_insert_rowid = (sqlite_int64  (*)(sqlite3*)) vDlSym("sqlite3_last_insert_rowid");
    sqlite3_libversion = (const char * (*)(void)) vDlSym("sqlite3_libversion");
    sqlite3_libversion_number = (int32_t  (*)(void)) vDlSym("sqlite3_libversion_number");
    sqlite3_malloc = (void *(*)(int32_t)) vDlSym("sqlite3_malloc");
    sqlite3_mprintf = (char * (*)(const char*,...)) vDlSym("sqlite3_mprintf");
    sqlite3_open = (int32_t  (*)(const char*,sqlite3**)) vDlSym("sqlite3_open");
    sqlite3_open16 = (int32_t  (*)(const void*,sqlite3**)) vDlSym("sqlite3_open16");
    sqlite3_prepare = (int32_t  (*)(sqlite3*,const char*,int32_t,sqlite3_stmt**,const char**)) vDlSym("sqlite3_prepare");
    sqlite3_prepare16 = (int32_t  (*)(sqlite3*,const void*,int32_t,sqlite3_stmt**,const void**)) vDlSym("sqlite3_prepare16");
    sqlite3_profile = (void * (*)(sqlite3*,void(*)(void*,const char*,sqlite_uint64),void*)) vDlSym("sqlite3_profile");
    sqlite3_progress_handler = (void  (*)(sqlite3*,int32_t,int32_t(*)(void*),void*)) vDlSym("sqlite3_progress_handler");
    sqlite3_realloc = (void *(*)(void*,int32_t)) vDlSym("sqlite3_realloc");
    sqlite3_reset = (int32_t  (*)(sqlite3_stmt*pStmt)) vDlSym("sqlite3_reset");
    sqlite3_result_blob = (void  (*)(sqlite3_context*,const void*,int32_t,void(*)(void*))) vDlSym("sqlite3_result_blob");
    sqlite3_result_double = (void  (*)(sqlite3_context*,double)) vDlSym("sqlite3_result_double");
    sqlite3_result_error = (void  (*)(sqlite3_context*,const char*,int32_t)) vDlSym("sqlite3_result_error");
    sqlite3_result_error16 = (void  (*)(sqlite3_context*,const void*,int32_t)) vDlSym("sqlite3_result_error16");
    sqlite3_result_int = (void  (*)(sqlite3_context*,int32_t)) vDlSym("sqlite3_result_int");
    sqlite3_result_int64 = (void  (*)(sqlite3_context*,sqlite_int64)) vDlSym("sqlite3_result_int64");
    sqlite3_result_null = (void  (*)(sqlite3_context*)) vDlSym("sqlite3_result_null");
    sqlite3_result_text = (void  (*)(sqlite3_context*,const char*,int32_t,void(*)(void*))) vDlSym("sqlite3_result_text");
    sqlite3_result_text16 = (void  (*)(sqlite3_context*,const void*,int32_t,void(*)(void*))) vDlSym("sqlite3_result_text16");
    sqlite3_result_text16be = (void  (*)(sqlite3_context*,const void*,int32_t,void(*)(void*))) vDlSym("sqlite3_result_text16be");
    sqlite3_result_text16le = (void  (*)(sqlite3_context*,const void*,int32_t,void(*)(void*))) vDlSym("sqlite3_result_text16le");
    sqlite3_result_value = (void  (*)(sqlite3_context*,sqlite3_value*)) vDlSym("sqlite3_result_value");
    sqlite3_rollback_hook = (void * (*)(sqlite3*,void(*)(void*),void*)) vDlSym("sqlite3_rollback_hook");
    sqlite3_set_authorizer = (int32_t  (*)(sqlite3*,int32_t(*)(void*,int32_t,const char*,const char*,const char*,const char*),void*)) vDlSym("sqlite3_set_authorizer");
    sqlite3_set_auxdata = (void  (*)(sqlite3_context*,int32_t,void*,void (*)(void*))) vDlSym("sqlite3_set_auxdata");
    sqlite3_snprintf = (char * (*)(int32_t,char*,const char*,...)) vDlSym("sqlite3_snprintf");
    sqlite3_step = (int32_t  (*)(sqlite3_stmt*)) vDlSym("sqlite3_step");
    sqlite3_table_column_metadata = (int32_t  (*)(sqlite3*,const char*,const char*,const char*,char const**,char const**,int32_t*,int32_t*,int32_t*)) vDlSym("sqlite3_table_column_metadata");
    sqlite3_thread_cleanup = (void  (*)(void)) vDlSym("sqlite3_thread_cleanup");
    sqlite3_total_changes = (int32_t  (*)(sqlite3*)) vDlSym("sqlite3_total_changes");
    sqlite3_trace = (void * (*)(sqlite3*,void(*xTrace)(void*,const char*),void*)) vDlSym("sqlite3_trace");
    sqlite3_transfer_bindings = (int32_t  (*)(sqlite3_stmt*,sqlite3_stmt*)) vDlSym("sqlite3_transfer_bindings");
    sqlite3_update_hook = (void * (*)(sqlite3*,void(*)(void*,int32_t ,char const*,char const*,sqlite_int64),void*)) vDlSym("sqlite3_update_hook");
    sqlite3_user_data = (void * (*)(sqlite3_context*)) vDlSym("sqlite3_user_data");
    sqlite3_value_blob = (const void * (*)(sqlite3_value*)) vDlSym("sqlite3_value_blob");
    sqlite3_value_bytes = (int32_t  (*)(sqlite3_value*)) vDlSym("sqlite3_value_bytes");
    sqlite3_value_bytes16 = (int32_t  (*)(sqlite3_value*)) vDlSym("sqlite3_value_bytes16");
    sqlite3_value_double = (double  (*)(sqlite3_value*)) vDlSym("sqlite3_value_double");
    sqlite3_value_int = (int32_t  (*)(sqlite3_value*)) vDlSym("sqlite3_value_int");
    sqlite3_value_int64 = (sqlite_int64  (*)(sqlite3_value*)) vDlSym("sqlite3_value_int64");
    sqlite3_value_numeric_type = (int32_t  (*)(sqlite3_value*)) vDlSym("sqlite3_value_numeric_type");
    sqlite3_value_text = (const uint8_t * (*)(sqlite3_value*)) vDlSym("sqlite3_value_text");
    sqlite3_value_text16 = (const void * (*)(sqlite3_value*)) vDlSym("sqlite3_value_text16");
    sqlite3_value_text16be = (const void * (*)(sqlite3_value*)) vDlSym("sqlite3_value_text16be");
    sqlite3_value_text16le = (const void * (*)(sqlite3_value*)) vDlSym("sqlite3_value_text16le");
    sqlite3_value_type = (int32_t  (*)(sqlite3_value*)) vDlSym("sqlite3_value_type");
    sqlite3_vmprintf = (char *(*)(const char*,va_list)) vDlSym("sqlite3_vmprintf");
/* Added ??? */
    sqlite3_overload_function = (int32_t (*)(sqlite3*, const char *zFuncName, int32_t nArg)) vDlSym("sqlite3_overload_function");
/* Added by 3.3.13 */
    sqlite3_prepare_v2 = (int32_t (*)(sqlite3*,const char*,int32_t,sqlite3_stmt**,const char**)) vDlSym("sqlite3_prepare_v2");
    sqlite3_prepare16_v2 = (int32_t (*)(sqlite3*,const void*,int32_t,sqlite3_stmt**,const void**)) vDlSym("sqlite3_prepare16_v2");
    sqlite3_clear_bindings = (int32_t (*)(sqlite3_stmt*)) vDlSym("sqlite3_clear_bindings");
/* Added by 3.4.1 */
    sqlite3_create_module_v2 = (int32_t (*)(sqlite3*,const char*,const sqlite3_module*,void*,void (*xDestroy)(void *))) vDlSym("sqlite3_create_module_v2");
/* Added by 3.5.0 */
    sqlite3_bind_zeroblob = (int32_t (*)(sqlite3_stmt*,int32_t,int32_t)) vDlSym("sqlite3_bind_zeroblob");
    sqlite3_blob_bytes = (int32_t (*)(sqlite3_blob*)) vDlSym("sqlite3_blob_bytes");
    sqlite3_blob_close = (int32_t (*)(sqlite3_blob*)) vDlSym("sqlite3_blob_close");
    sqlite3_blob_open = (int32_t (*)(sqlite3*,const char*,const char*,const char*,sqlite3_int64,int32_t,sqlite3_blob**)) vDlSym("sqlite3_blob_open");
    sqlite3_blob_read = (int32_t (*)(sqlite3_blob*,void*,int32_t,int32_t)) vDlSym("sqlite3_blob_read");
    sqlite3_blob_write = (int32_t (*)(sqlite3_blob*,const void*,int32_t,int32_t)) vDlSym("sqlite3_blob_write");
    sqlite3_create_collation_v2 = (int32_t (*)(sqlite3*,const char*,int32_t,void*,int32_t(*)(void*,int32_t,const void*,int32_t,const void*),void(*)(void*))) vDlSym("sqlite3_create_collation_v2");
    sqlite3_file_control = (int32_t (*)(sqlite3*,const char*,int32_t,void*)) vDlSym("sqlite3_file_control");
    sqlite3_memory_highwater = (sqlite3_int64 (*)(int32_t)) vDlSym("sqlite3_memory_highwater");
    sqlite3_memory_used = (sqlite3_int64 (*)(void)) vDlSym("sqlite3_memory_used");
    sqlite3_mutex_alloc = (sqlite3_mutex *(*)(int32_t)) vDlSym("sqlite3_mutex_alloc");
    sqlite3_mutex_enter = (void (*)(sqlite3_mutex*)) vDlSym("sqlite3_mutex_enter");
    sqlite3_mutex_free = (void (*)(sqlite3_mutex*)) vDlSym("sqlite3_mutex_free");
    sqlite3_mutex_leave = (void (*)(sqlite3_mutex*)) vDlSym("sqlite3_mutex_leave");
    sqlite3_mutex_try = (int32_t (*)(sqlite3_mutex*)) vDlSym("sqlite3_mutex_try");
    sqlite3_open_v2 = (int32_t (*)(const char*,sqlite3**,int32_t,const char*)) vDlSym("sqlite3_open_v2");
    sqlite3_release_memory = (int32_t (*)(int32_t)) vDlSym("sqlite3_release_memory");
    sqlite3_result_error_nomem = (void (*)(sqlite3_context*)) vDlSym("sqlite3_result_error_nomem");
    sqlite3_result_error_toobig = (void (*)(sqlite3_context*)) vDlSym("sqlite3_result_error_toobig");
    sqlite3_sleep = (int32_t (*)(int32_t)) vDlSym("sqlite3_sleep");
    sqlite3_soft_heap_limit = (void (*)(int32_t)) vDlSym("sqlite3_soft_heap_limit");
    sqlite3_vfs_find = (sqlite3_vfs *(*)(const char*)) vDlSym("sqlite3_vfs_find");
    sqlite3_vfs_register = (int32_t (*)(sqlite3_vfs*,int32_t)) vDlSym("sqlite3_vfs_register");
    sqlite3_vfs_unregister = (int32_t (*)(sqlite3_vfs*)) vDlSym("sqlite3_vfs_unregister");
    sqlite3_threadsafe = (int32_t (*)(void)) vDlSym("sqlite3_threadsafe");
    sqlite3_result_zeroblob = (void (*)(sqlite3_context*,int32_t)) vDlSym("sqlite3_result_zeroblob");
    sqlite3_result_error_code = (void (*)(sqlite3_context*,int32_t)) vDlSym("sqlite3_result_error_code");
    sqlite3_test_control = (int32_t (*)(int32_t, ...)) vDlSym("sqlite3_test_control");
    sqlite3_randomness = (void (*)(int32_t,void*)) vDlSym("sqlite3_randomness");
    sqlite3_context_db_handle = (sqlite3 *(*)(sqlite3_context*)) vDlSym("sqlite3_context_db_handle");
    sqlite3_extended_result_codes = (int32_t (*)(sqlite3*,int32_t)) vDlSym("sqlite3_extended_result_codes");
    sqlite3_limit = (int32_t (*)(sqlite3*,int32_t,int32_t)) vDlSym("sqlite3_limit");
    sqlite3_next_stmt = (sqlite3_stmt *(*)(sqlite3*,sqlite3_stmt*)) vDlSym("sqlite3_next_stmt");
    sqlite3_sql = (const char *(*)(sqlite3_stmt*)) vDlSym("sqlite3_sql");
    sqlite3_status = (int32_t (*)(int32_t,int32_t*,int32_t*,int32_t)) vDlSym("sqlite3_status");
}

cSqliteDb::cSqliteDb()
    : conf_(ps::lib::cConfigures::get_const_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , db_(0), errmsg_(0)
    , fn_(conf_.as<std::string>("repos_fn"))
{
    std::call_once(
        oOnceLoadSymbols_, [this]
        {
            vActivateSymbols();
            boost::filesystem::path bfn_(fn_.size() == 0 ? IN_MEMORY_DATABASE : conf_.sGetPsHome() / fn_);
            int32_t rc = sqlite3_open_v2(bfn_.c_str(), &db_, 
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
            , 0);
            ASSERT_OR_RAISE_FNC(!rc, std::runtime_error, cCheckErr(*this));
            sVersion_ = boost::io::str(
                boost::format("Sqlite version %s activated and attached to %s")
                % sqlite3_libversion() % bfn_
            );
        }
    );
}

cSqliteDb::~cSqliteDb()
{
    if (errmsg_) sqlite3_free( errmsg_ );
    if (db_) sqlite3_close(db_);
}

std::string cSqliteDb::sShowVersion() const
{
    return sVersion_;
}

int32_t cSqliteDb::iExecSql(
    const char* sql
    , int32_t (*cbk_fn)(void*,int32_t,char**,char**)
    , void* ctx
){
    BOOST_ASSERT(sql);
    BOOST_ASSERT(strlen(sql) > 0);
    if (errmsg_)
    {
        sqlite3_free( errmsg_ );
        errmsg_ = 0;
    }
    int32_t rc = sqlite3_exec(db_, sql, cbk_fn, ctx, &errmsg_);
    if (rc) mos_ << sql << std::endl;
    return rc;
}

int32_t cSqliteDb::iExecSql(const char* sql)
{
    return cSqliteDb::iExecSql(sql, NULL, NULL);
}

int32_t cSqliteDb::iExecSql(const ps::lib::str_vct& arr)
{
    int32_t rc(0);
    for (const auto& str: arr)
    {
        rc = cSqliteDb::iExecSql(str.c_str(), NULL, NULL);
        if (rc) break;
    }
    return rc;
}

sqlite3* cSqliteDb::oGetHndl() const
{
    return db_;
}

std::string cSqliteDb::sGetFn() const
{
    return fn_;
}

cCheckErr::tErrSymMap cCheckErr::oErrMap =
{ 
    { 0, "SQLITE_OK"}
    , { 1, "SQLITE_ERROR"}
    , { 2, "SQLITE_INTERNAL"}
    , { 3, "SQLITE_PERM"}
    , { 4, "SQLITE_ABORT"}
    , { 5, "SQLITE_BUSY"}
    , { 6, "SQLITE_LOCKED"}
    , { 7, "SQLITE_NOMEM"}
    , { 8, "SQLITE_READONLY"}
    , { 9, "SQLITE_INTERRUPT"}
    , {10, "SQLITE_IOERR"}
    , {11, "SQLITE_CORRUPT"}
    , {12, "SQLITE_NOTFOUND"}
    , {13, "SQLITE_FULL"}
    , {14, "SQLITE_CANTOPEN"}
    , {15, "SQLITE_PROTOCOL"}
    , {16, "SQLITE_EMPTY"}
    , {17, "SQLITE_SCHEMA"}
    , {18, "SQLITE_TOOBIG"}
    , {19, "SQLITE_CONSTRAINT"}
    , {20, "SQLITE_MISMATCH"}
    , {21, "SQLITE_MISUSE"}
    , {22, "SQLITE_NOLFS"}
    , {23, "SQLITE_AUTH"}
    , {24, "SQLITE_FORMAT"}
    , {25, "SQLITE_RANGE"}
    , {26, "SQLITE_NOTADB"}
    , {27, "SQLITE_NOTICE"}
    , {28, "SQLITE_WARNING"}
    , {100,"SQLITE_ROW"}
    , {101,"SQLITE_DONE"}
};

const std::string cCheckErr::operator()() const
{
    sqlite3* db_ = oSqliteDb_.oGetHndl();
    auto it = oErrMap.find(sqlite3_errcode(db_));
    return boost::str(boost::format("[%s] %s (%s).")
        % ((it != oErrMap.end()) ? it->second : "Unknown Result code")
        % sqlite3_errmsg(db_) % oSqliteDb_.sGetFn()
    );
}

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

