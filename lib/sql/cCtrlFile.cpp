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

namespace ps
{

namespace lib
{

namespace sql
{

class cCtrlFileImpl
{
public:
    /**
     * @details
     * @param[in] sFileName
     * - A name of the data file to import.
     * - This will be offered as parameter for the INFILE clause.
     * @param[in] sTagName
     * - A table name with a schema modifier. The way of example: "SCOTT"."EMP"
     * - Syntax without schema modifier is also acceptable. example: "EMP"
     * - This will be offered as parameter for the INTO TABLE clause.
     * @param[in] sPartitionName
     * - A partition name. 
     * - This can be empty string  when table is not partitioned.
     * - When length is more than 0, this will be offered as parameter
     *   for the PARTITION option in the INTO TABLE clause.
     * @param[in] iNumLongs
     * - Number of LONG or LONGRAW type column on the table.
     * - Tables containing these types need to be identified
     *   because they are constraints of UNRECOVERABLE loading.
     */
    cCtrlFileImpl(
        const boost::filesystem::path& sFileName
        , const std::string& sTagName
        , const std::string& sPartitionName
        , const int32_t& iNumLongs
    );
    ~cCtrlFileImpl();
    /**
     * @details
     * @return Expression of the control file by plane text.
     * @note
     * Refer to the following URL, to study syntax of the control file.
     * - A SQL*Loader Syntax Diagrams<br />
     *   https://docs.oracle.com/cloud/latest/db112/SUTIL/app_ldr_syntax.htm#SUTIL050
     * - Input Data and Data Files<br />
     *   https://docs.oracle.com/cloud/latest/db112/SUTIL/ldr_concepts.htm#SUTIL981
     *
     */
    std::string sGetGrammar() const;
private:
    const ps::lib::cConfigures& conf_;
    ps::lib::cDistributor& mos_;
    const int32_t iCharsetId_;
    const bool iNoBom_;
    const bool iUsualpath_;
    const bool iRecoverable_;
    const boost::filesystem::path sFileName_;
    const std::string sTagName_;
    const boost::regex regTagName_;
    const std::string sPartitionName_;
    const ps::lib::cDelimiter oDelim_;
    const int32_t iNumLongs_;
    boost::smatch oMatch_;
    cCtrlFileImpl(const cCtrlFileImpl&) =delete;
    cCtrlFileImpl& operator=(const cCtrlFileImpl&) =delete;
};

cCtrlFileImpl::cCtrlFileImpl(
    const boost::filesystem::path& sFileName
    , const std::string& sTagName
    , const std::string& sPartitionName
    , const int32_t& iNumLongs
)
    : conf_(ps::lib::cConfigures::get_const_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , iCharsetId_(conf_.as<int32_t>("charsetid"))
    , iNoBom_(conf_.as<bool>("nobyteordermark"))
    , iUsualpath_(conf_.as<bool>("usualpath"))
    , iRecoverable_ (conf_.as<bool>("recoverable"))
    , sFileName_(sFileName)
    , sTagName_(sTagName)
    , regTagName_(R"(((?<schema>\w+)\.)?(?<table>\w+))")
    , sPartitionName_(sPartitionName)
    , oDelim_(ps::lib::oMakeVarDelimiter())
    , iNumLongs_(iNumLongs)
{
    BOOST_ASSERT(!sFileName.empty());
    BOOST_ASSERT(!sTagName.empty());
    BOOST_ASSERT(iNumLongs >= 0);
    ASSERT_OR_RAISE(boost::regex_match(sTagName, oMatch_, regTagName_)
        , std::runtime_error
        , boost::format(R"(Lexical error in sTagName. "%s" does not match for regex:"%s".)")
          % sTagName % regTagName_.str()
    );
}

cCtrlFileImpl::~cCtrlFileImpl()
{}

std::string cCtrlFileImpl::sGetGrammar(void) const
{
    std::ostringstream oss;
    std::ostringstream row_sep; // Method for separating each rows.
    const auto iRecLen = oDelim_.iGetVarDigit();
    if (iRecLen)
    {
        // to import the "Variable Record Format"
        row_sep << "VAR " << iRecLen;
    }
    else
    {
        // to import the "Stream Record Format"
        row_sep << "STR " << oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iCtrl);
    }
    // An option to use an efficient way to specify and load partitions.
    std::ostringstream part_clause;
    if (sPartitionName_.size())
    {
        part_clause << boost::format(R"( PARTITION("%s"))") % sPartitionName_;
    }
    std::ostringstream charset_clause;
    // Correspond to the charset name of AL32UTF8 and UTF8 respectively.
    if (iCharsetId_ == 873 || iCharsetId_ == 871)
    {
        charset_clause
            << " CHARACTERSET AL32UTF8 BYTEORDERMARK"
            << (iNoBom_ ? " NOCHECK" : " CHECK")
        ;
    }
    oss << boost::format("%sLOAD%s")
        % (iUsualpath_ || iRecoverable_ || iNumLongs_ ? "" : "UNRECOVERABLE ")
        % charset_clause.str()
        << std::endl
        ;
    oss << boost::format(R"(INFILE %s "%s")")
        % sFileName_
        % row_sep.str()
        << std::endl
        ;
    if (oMatch_["schema"].length() > 0)
    {
        oss << boost::format(R"(INTO TABLE "%s"."%s")")
            % oMatch_["schema"] % oMatch_["table"];
    }
    else
    {
        oss << boost::format(R"(INTO TABLE "%s")")
            % oMatch_["table"];
    }
    oss << part_clause.str() << std::endl;
    oss << boost::format("TRUNCATE REENABLE FIELDS TERMINATED BY %s")
        % oDelim_.sGetColSeparator(ps::lib::cDelimiter::iCtrl)
        << std::endl
        ;
    if (!oDelim_.iGetEndTerm())
    {
        oss << boost::format("TRAILING NULLCOLS")
            << std::endl
            ;
    }
    return oss.str();
}

cCtrlFile::cCtrlFile(
    const boost::filesystem::path& sFileName
    , const std::string& sTagName
    , const std::string& sPartitionName
    , const int32_t& iNumLongs
)
    : oImpl_(new cCtrlFileImpl(sFileName, sTagName, sPartitionName, iNumLongs))
{}

cCtrlFile::~cCtrlFile()
{}

std::string cCtrlFile::sGetGrammar(void) const
{
    return oImpl_->sGetGrammar();
}

} // ps::lib::sql

} // ps::lib

} // ps

