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

namespace nsDelimiter
{

void vForDataEscaped(std::string& dst, const std::string& result)
{
    switch(result[1])
    {
    case 'n': dst.append("\n"); break;
    case 'r': dst.append("\r"); break;
    case 't': dst.append("\t"); break;
    case '0': /* Nothing to do. */; break;
    }
}

void vForCtrlEscaped(std::string& dst, const std::string& result)
{
    std::string ch;
    switch(result[1])
    {
    case 'n': ch = "0A"; break;
    case 'r': ch = "0D"; break;
    case 't': ch = "09"; break;
    case '0': /* Nothing to do. */; break;
    }
    dst.append(ch);
}

void vForDataHexdecm(std::string& dst, const std::string& result)
{
    std::stringstream ss;
    int32_t ch;
    ss << result;
    ss >> std::showbase >> std::hex >> ch;
    dst += ch;
}

void vForCtrlHexdecm(std::string& dst, const std::string& result)
{
    std::string ch(result, 2, 2);
    std::transform(ch.cbegin(), ch.cend(), ch.begin(), &toupper);
    dst .append(ch);
}

std::string sCliteralScanner(
    const std::string& src
    , boost::regex& ex
    , boost::function<void (std::string&, const std::string&)> func
){
    boost::smatch result;
    std::string dst;
    auto it = src.cbegin(), ite = src.cend();
    while (it != ite)
    {
        if (boost::regex_search(it, ite, result, ex))
        {
            if (it != result[0].first)
            {
                dst.append(it, result[0].first);
            }
            func(dst, std::string(result[0].first, result[0].second));
        }
        else
        {
            break;
        }
        it = result[0].second;
    }
    if (it != ite)
    {
        dst.append(it, ite);
    }
    return dst;
}

std::string sConvLiteral(const cDelimiter::tType& iType, const std::string& src)
{
    std::string sRet;
    boost::regex escaped("\\\\[rnt0]");         // e.g. "\r\n"
    boost::regex hexdecm("0x[[:xdigit:]]{2}");  // e.g. "0x0d0x0a"
    if (boost::regex_search(src, escaped))
    {
        if (iType == cDelimiter::iData)
            sRet = sCliteralScanner(src, escaped, &vForDataEscaped);
        else
            sRet = "X'" + sCliteralScanner(src, escaped, &vForCtrlEscaped) + "'";
    }
    else if (boost::regex_search(src, hexdecm))
    {
        if (iType == cDelimiter::iData)
            sRet = sCliteralScanner(src, hexdecm, &vForDataHexdecm);
        else
            sRet = "X'" + sCliteralScanner(src, hexdecm, &vForCtrlHexdecm) + "'";
    }
    else
    {
        sRet = iType == cDelimiter::iData ? src : "'" + src + "'";
    }
    return sRet;
}

} // nsDelimiter

cDelimiter::cDelimiter(
    const std::string& sRowSeparator
    , const std::string& sColSeparator
    , const std::string& sEnclosure1
    , const std::string& sEnclosure2
    , const bool& iTrailinNullCols
    , const int32_t& iVarDigit
    , const bool& iEmbedColumnNames
)
    : iEndTerm_(iTrailinNullCols)
    , iVarDigit_(iVarDigit)  
    , iExplicit_(sEnclosure2.size() > 0)  
    , iEmbedColumnNames_(iEmbedColumnNames)
    , fpGetLengthStr_(
        iVarDigit_
        ? static_cast<tFp>([this](const std::string& body)->std::ostringstream&
        {
            oss_.str(""); // data clear
            oss_ << std::setw(iVarDigit_) << std::setfill('0') << body.size();
            return oss_;
        })
        : static_cast<tFp>([this](const std::string&)->std::ostringstream&
        {
            return oss_;    // Nothing to do.
        })
    )
{
    // Represents for the data files;
    oItems_.push_back(tItem(
        nsDelimiter::sConvLiteral(iData, sRowSeparator)
        , nsDelimiter::sConvLiteral(iData, sColSeparator)
        , nsDelimiter::sConvLiteral(iData, sEnclosure1)
        , nsDelimiter::sConvLiteral(iData, sEnclosure2)
    ));
    // Represents for the control files;
    oItems_.push_back(tItem(
        nsDelimiter::sConvLiteral(iCtrl, sRowSeparator)
        , nsDelimiter::sConvLiteral(iCtrl, sColSeparator)
        , nsDelimiter::sConvLiteral(iCtrl, sEnclosure1)
        , nsDelimiter::sConvLiteral(iCtrl, sEnclosure2)
    ));
}

cDelimiter::cDelimiter(const cDelimiter& rhs)
    : iEndTerm_(rhs.iEndTerm_)
    , iVarDigit_(rhs.iVarDigit_)
    , iExplicit_(rhs.iExplicit_)
    , iEmbedColumnNames_(rhs.iEmbedColumnNames_)
    , oItems_(rhs.oItems_)
    , fpGetLengthStr_(rhs.fpGetLengthStr_)
{}

cDelimiter& cDelimiter::operator=(const cDelimiter& rhs)
{
    iEndTerm_ = rhs.iEndTerm_;
    iVarDigit_ = rhs.iVarDigit_;
    iExplicit_ = rhs.iExplicit_;
    iEmbedColumnNames_ = rhs.iEmbedColumnNames_;
    oItems_ = rhs.oItems_;
    fpGetLengthStr_ = rhs.fpGetLengthStr_;
    return *this;
}

std::string cDelimiter::sGetClauseEncForCtrl() const
{
    return "ENCLOSED BY " + sGetEnclosure1(iCtrl) + (iGetExplicit() ? " AND " + sGetEnclosure2(iCtrl) : "");
}
std::string cDelimiter::sGetLengthString(const std::string& body) const
{
    return fpGetLengthStr_(body).str();
}

template<>
void vAddDelims(std::string& dest, const cDelimiter& me, const std::string data, const ps::lib::sql::ind_t& ind, const bool& iSep)
{
    me.vEnCls(dest, data, ind, iSep);
}

template<>
void vAddDelims(std::string& dest, const cDelimiter& me, const char* data, const ps::lib::sql::ind_t& ind, const bool& iSep)
{
    me.vEnCls(dest, data, ind, iSep);
}

template<>
void vAddDelims(std::string& dest, const cDelimiter& me, char* data, const ps::lib::sql::ind_t& ind, const bool& iSep)
{
    me.vEnCls(dest, data, ind, iSep);
}

void vSeparateToDelimiters(
    const std::string& sCompoStr  // format: [col_separator]<iSep>[begin_string_enclosure]<iSep>[end_string_enclosure]
    , const char* iSep
    , std::string& sColSeparator 
    , std::string& sEnclosure1 
    , std::string& sEnclosure2 
    , bool& iEndTerm
    , int32_t& iVarDigit
){
    BOOST_ASSERT(std::count(sCompoStr.cbegin(), sCompoStr.cend(), *iSep) == 2);
    boost::char_separator<char> sep(iSep, "", boost::keep_empty_tokens);
    boost::tokenizer<boost::char_separator<char> > tokens(sCompoStr, sep);
    auto tok_it = tokens.begin();
    sColSeparator = *tok_it++;
    sEnclosure1 = *tok_it++;
    sEnclosure2 = *tok_it++;
    iEndTerm = false;
    iVarDigit = 0;
}

cDelimiter oMakeVarDelimiter()
{
    const ps::lib::cConfigures& conf_(ps::lib::cConfigures::get_const_instance());
    std::string sRowSeparator = conf_.as<std::string>("rowseparator");
    std::string sColSeparator;
    std::string sEnclosure1;
    std::string sEnclosure2;
    bool iEndTerm;
    int32_t iVarDigit;
    bool iEmbedColumnNames;
    if (conf_.as<std::string>("embed_column_name").size())
    {
        ps::lib::vSeparateToDelimiters(
            conf_.as<std::string>("embed_column_name"), ":" 
            , sColSeparator, sEnclosure1, sEnclosure2, iEndTerm, iVarDigit
        );
        iEmbedColumnNames = true;
    }
    else
    {
        sColSeparator = conf_.as<std::string>("terminator");
        sEnclosure1 = conf_.as<std::string>("encloser");
        sEnclosure2 = conf_.as<std::string>("encloser2");
        iEndTerm = conf_.as<bool>("endterm");
        iVarDigit = conf_.as<int32_t>("reclength");
        iEmbedColumnNames = false;
    }
    return ps::lib::cDelimiter(
        sRowSeparator, sColSeparator, sEnclosure1, sEnclosure2, iEndTerm
        , iVarDigit, iEmbedColumnNames
    );
}

} // ps::lib

} // ps

