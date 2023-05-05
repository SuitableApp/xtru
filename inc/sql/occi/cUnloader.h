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
 * @class cUnloader
 * @brief
 * This class provides following roles.
 * -# provides common interface and data for the classes that is derived from this class,
 * -# and default implementation.
 */
class cUnloader
    : public ps::lib::sql::cFetchable
    , private boost::noncopyable
{
private:
    ps::lib::cStat& stat_;       ///< to gather and store statistic counters for the diagnosis of performance.
    const ps::lib::cRtn& rtn_;   ///< to transmit a status value that is used to diagnosis when occuring the fault.
    const ps::lib::cConfigures& conf_; ///< to store values which was used at starting up.
    ps::lib::cConsole& cout_;    ///< output to the console.
    ps::lib::cTracer& trc_;      ///< output to the tracing file.
    ps::lib::cDistributor& mos_; ///< to make it possible to aggregate one console and one trace to one stream.
    typedef ps::lib::cSpinLock<int64_t, std::micro> spinlock_t;
    spinlock_t spin_;            ///< to protect the I/O integrity from the multiple access.
    int32_t iTls_;               ///< is used to count number of TLSs.
    /// @brief A smart pointer pointing to an int32_t type that is used to pick up an element
    ///   from the @ref oCont_.
    /// @note It is used as a subscript for maintaining / referencing the context of thread
    ///   by assigning ordinal numbers of constants unique to each thread.
    boost::thread_specific_ptr<int32_t> oTls_;
    std::atomic<int64_t> iTotalBytes_; ///< accumulates total written bytes of amount.
    std::atomic<uint32_t> iTotalRows_;
    std::string fbase_;          ///< A base name (exclude an extention) of data (and control) file.
    /**
     * @brief
     * An identifier for the output of diagnosis. It is based on table name or query file name.
     */
    const std::string tag_;
    const int32_t iNumLongs_;
    boost::filesystem::path sLastOpendFilenme_;
    std::string sPartitionName_;
    /**
     * @brief
     * @struct tValue
     */
    struct tValue
    {
        std::unique_ptr<ps::lib::sql::occi::cStmt> oStmt_;
        uint32_t iNumRows_;
        ps::lib::str_vct oRowBuf_;
        std::future<uint32_t> oFuture_;
        std::unique_ptr<std::thread> oThr_;
        std::thread::id iTid_;
        tValue(
            ps::lib::sql::occi::cStmt* oStmt
            , const uint32_t& iBulkSize
        )
            : oStmt_(oStmt)
            , iNumRows_(0U)
            , oRowBuf_(iBulkSize, ps::lib::str_vct::value_type())
            , oThr_(nullptr)
        {}
    };
    /**
     * @brief
     * - Array of structured type data needed to execute element SQL.
     * - Every array element is associated with SQL-Select.
     */
    ps::lib::cVector<tValue> oCont_;
    /**
     * @brief
     * @return a string which is stored column names list
     *   which is separated with delimiters.
     * @note
     * - Which delimiters are applied depends
     *   on the @ref oDelim_ of the constructor argument.
     * - That string can be used for first line of datafile.
     */
    std::string oGetColNamesList() const;
    /**
     * @brief
     * @return a string which is stored column names list
     *   for a part of contents on the control-file of SQL*Loader.
     */
    std::string sGetFieldsListForCtrl() const;
    /**
     * @brief
     * writes the summary of the statistics
     * to the console and the trace-file.
     */
    void vShowStatValue() const;
protected:
    const ps::lib::cDelimiter oDelim_;
    const uint32_t iBulkSize_;
    std::unique_ptr<ps::lib::nsStreamLocator::cStreamSupplier> oStreamSup_;
    std::string sDataFileDir_;   ///< Reserved for the future extension.
    /// @brief A handole of the controle file for the SQL*Loader
    std::unique_ptr<std::ostream> st_ctrl_;
    /// @brief A handole of the data file for the SQL*Loader
    std::unique_ptr<std::ostream> st_data_;
    /**
     * @brief
     */
     ps::lib::sql::occi::cDefine& oGetDefine()
     {
         BOOST_ASSERT(oCont_.size());
         return oCont_[0].oStmt_->oGetDefine();
     }
    /**
     * @brief
     * - In order to be able to verify the throughput of the unload, this object accumulates
     *   the total number of bytes written to the stream.
     * @param[in] iOutputBytes
     *   Number of bytes of difference newly generated after previous call.
     */
    void vAddOutputBytes(const int64_t& iOutputBytes);
    void vAddOutputRows(const int32_t& iOutputRows);
    /**
     * @brief
     * - makes and dispatches to the ostream a row represented by column name list.
     */
    void vPutColumnNamesToDataFile();
    /**
     * @brief
     */
    void vClearBuffer();
    /**
     * @brief
     * # RAW data(s) on the OCI array interface (It is attached by calling
     *     ps::lib::sql::occi::vDefineArrayOfStruct() function) are converted to
     *     ps::lib::str_vct (oRowBuf) by this function.<br/>
     *   the method of reading the OCI array data appropriate to the data type of <br/>
     *   each column is dynamically selected at runtime, <br/>
     *   becaouse ps::lib::sql::occi::cAttr::vConvertStringVct() is a virtual function,
     * # Invoke this function to fill the oRowBuf_ of tValue with OCI array data
     *   before calling vPutRowsToDataFile().
     * @param[in] iCol
     *   indicates position of the column. Originated zero.
     * @param[in] iNumIter
     *   takes a value of range which is between 1 and iBulkSize.
     * @param[in] iSep
     *   false indicates a last clolumn in the select list.
     */
    void vSetRowBuf(
        const int32_t& iCol
        , const uint32_t& iNumIter
        , const bool& iSep
    );
    /**
     * @brief
     * - Assemble one bulk rows based on formatting and dispatch to ostream.
     * - With thread safe, multiple accesses to ostream are serialized.
     * @param[in] iNumIter
     */
    void vPutRowsToDataFile(const uint32_t& iNumIter);
    /**
     * @brief
     * - generates a control file used for SQL*Loader.
     */
    void vPutGrammerToCtrlFile();
    /**
     * @brief
     * @return a number of the columns as integer.
     */
    const int32_t iGetNumCols() const
    {
        BOOST_ASSERT(oCont_.size());
        return oCont_[0].oStmt_->oGetAttrs().size();
    }
public:
    static const int32_t NO_LONG_COLUMN; ///< >0: "Table has not any LONG/LONG RAW columns."
    /**
     * @brief
     *
     * @param[in,out] oSvc
     *   Contains a connection to at least one established
     *   Oracle instance.
     *
     * @param[in] oStreamSup
     *
     * @param[in] iBulkSize
     *
     * @param[in] sql
     *   stores one or more SQL statement(s).
     * - If there are multi statements in it,
     *   they are separated by single semi-colon each other.
     * - In addition, each element SQL will be assigned
     *   threads and processed in parallel.
     *
     * @param[in] tag Supposing to pass either:
     * -# tag_name
     * -# or tag_name;table_name
     * -# or tag_name;schema_name.table_name
     * - Paticularly, patterm (1) is regarded as (2).
     * - tag_name is used to label in diagnostic information.
     * - schema_name and table_name are used to label outputting.
     *
     * @param[in] iNumLongs Number of LONG/LONG RAW columns.
     * - NOTE: SQL*Loader rejects UNRECOVERABLE data loading
     *   for tables containing LONG/LONG RAW types
     *
     * @param[in] oBind
     *   Contains zero or more pair(s) consisting of
     *   a combination of a place folder and a value
     *   is included.
     *   If the places folder that appears in sql
     *   is undefined here, an exception will be thrown.
     *
     * @par Refer
     * @par Modify
     */
    cUnloader(
        ps::lib::sql::occi::cSvc& oSvc
        , ps::lib::nsStreamLocator::cStreamSupplier* oStreamSup
        , const uint32_t& iBulkSize
        , const std::string& sql
        , const std::string& tag
        , const int32_t& iNumLongs
        , ps::lib::sql::occi::cBind* oBind
    );
    /**
     * @brief
     *   Construction without any bind variables
     */
    cUnloader(
        ps::lib::sql::occi::cSvc& oSvc
        , ps::lib::nsStreamLocator::cStreamSupplier* oStreamSup
        , const uint32_t& iBulkSize
        , const std::string& sql
        , const std::string& tag
        , const int32_t& iNumLongs
    );
    /**
     * @brief
     */
    virtual ~cUnloader();
    /**
     * @brief
     * - Provides a standard implementation of processing for
     *   the data returned by executing SQL-Select.
     * - It is also possible to override this standard implementation
     *   in an inherited class.
     * <p>
     * - When called up, the following processing is performed in order.
     *  -# Opens two ostreams referenced by @ref st_ctrl_ and @ref st_data_.<br/>
     *    They indicate where control file and data file are stored, respectively.
     *  -# Executes a SQL-Select statement saved as tValue::oStmt_ in @ref oCont_.<br/>
     *    This operation is ran in the current thread.
     *  -# Reserves delayed fetching using std::packaged_task.<br/>
     *    On this time,
     *    <a href="http://en.cppreference.com/w/cpp/thread/packaged_task">std::packaged_task</a>
     *    hooks ps::lib::sql::occi::cStmt::iFetch().<br/>
     *    And as context, oStmt_ is passed to the argument of the constructor.<br/>
     *    The hooked function is asynchronously handled by another thread on background.
     *  -# About all elements of @ref oCont_, Repeats the process from No.2.
     *  -# Joins all queued tasks to the current thread. When asynchronous processing is completed,<br/>
     *    the number of rows read is returned to tValue::iNumRows_.
     *  -# About how many number of bytes and rows written, It is reported to the tracing file.
     *  -# Finally, Closes two ostreams.
     * </p>
     * @note
     */
    virtual void vExecuteAndFetch();
    /**
     * @brief
     *   It is executed only once before the record reading starts.
     */
    virtual void vPreRepeatAction();
    /**
     * @brief
     *   It is executed only once after all records are read.<br/>
     *   Mutually exclusive execution with vNotFoundAction().
     */
    virtual void vPostRepeatAction();
    /**
     * @brief
     *   It is executed only once if no record can be read.<br/>
     *   Mutually exclusive execution with vPostRepeatAction().
     */
    virtual void vNotFoundAction();
    /**
     * @brief
     *   It is repeatedly executed when the total number of record readings is divisible
     *   by the calculation result of feedback * bulk_size (they are defined in xtru.conf).<br/>
     *   This comparison operation is executed by ps::lib::sql::occi::cStmt::iFetch().
     */
    virtual void vFeedbackAction();
    /**
     * @brief
     *   It is executed only once regardless of whether or not a record is read.
     */
    virtual void vFinalizeAction();
    /**
     * @brief
     *   It is repeatedly executed before reading is completed in 1 bulk unit.
     */
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /**
     * @brief
     *   It is repeatedly executed after reading is completed in 1 bulk unit.
     */
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /**
     * @brief
     * @param[in] opts
     *   stores values for %s place-holder in ps::lib::cVector<tValue>::oStmt_ .
     */
    virtual void vConvPlaceHolder(const ps::lib::str_vct& opts);
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

