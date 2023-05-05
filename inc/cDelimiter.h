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


class cDelimiter
{
public:
    enum tType {iData, iCtrl, iNumTypes};
private:
    bool iEndTerm_;
    int32_t iVarDigit_;      ///< Shows in bytes precision of the data length field.
    bool iExplicit_;
    bool iEmbedColumnNames_; ///< When it is true, first row of a datafile is column names list 
                             ///< whose each names are separated by a comma.
    std::ostringstream oss_;
    class tItem
    {
        friend class cDelimiter;
    private:
        std::string sRowSeparator_;
        std::string sColSeparator_;
        std::string sEnclosure1_;
        std::string sEnclosure2_;
    public:
        tItem(
            const std::string& sRowSeparator
            , const std::string& sColSeparator
            , const std::string& sEnclosure1
            , const std::string& sEnclosure2
        )
            : sRowSeparator_(sRowSeparator)
            , sColSeparator_(sColSeparator)  
            , sEnclosure1_(sEnclosure1)
            , sEnclosure2_(sEnclosure2)
        {}
    };
    ps::lib::cVector<tItem> oItems_;
    typedef std::function<std::ostringstream&(const std::string&)> tFp;
    tFp fpGetLengthStr_;
    const std::string& sGetEnclosure1(const tType& iType) const { return oItems_[iType].sEnclosure1_; }
    const std::string& sGetEnclosure2(const tType& iType) const { return oItems_[iType].sEnclosure2_; }
public:
    cDelimiter(
        const std::string& sRowSeparator ="\\n"
        , const std::string& sColSeparator =","
        , const std::string& sEnclosure1 ="\""
        , const std::string& sEnclosure2 ="\""
        , const bool& iEndTerm =false
        , const int32_t& iVarDigit =10
        , const bool& iEmbedColumnNames =false
    );
    cDelimiter(const cDelimiter& rhs);
    cDelimiter& operator=(const cDelimiter& rhs);
    const std::string& sGetRowSeparator(const tType& iType) const { return oItems_[iType].sRowSeparator_; }
    const std::string& sGetColSeparator(const tType& iType) const { return oItems_[iType].sColSeparator_; }
    const std::string& sGetLastSeparator(const tType& iType) const
    {
        const static std::string sNullStr = "";
        return iGetEndTerm() ? sGetColSeparator(iType) : sNullStr;
    }
    bool iGetEndTerm() const { return iEndTerm_; }
    int32_t iGetVarDigit() const { return iVarDigit_; }
    bool iGetExplicit() const { return iExplicit_; }
    bool iDoesEmbedColumnNames() const { return iEmbedColumnNames_; }
    std::string sGetClauseEncForCtrl() const;
    std::string sGetLengthString(const std::string& body) const;
    void vEnCls(std::string& dest, const std::string& data, const ps::lib::sql::ind_t& ind, const bool& iSep) const
    {
        dest += sGetEnclosure1(iData);
        if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL) dest += data;
        dest += iGetExplicit() ? sGetEnclosure2(iData) : sGetEnclosure1(iData);
        if (iSep) dest += sGetColSeparator(iData);
    }
    template<typename U>
    void vUnCls(std::string& dest, const U& data, const ps::lib::sql::ind_t& ind, const bool& iSep) const
    {
        if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL) dest += boost::lexical_cast<std::string>(data);
        if (iSep) dest += sGetColSeparator(iData);
    }
    void vUnCls(std::string& dest, const char* data, const uint32_t len, const ps::lib::sql::ind_t& ind, const bool& iSep) const
    {
        if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL) dest.append(data, data + len);
        if (iSep) dest += sGetColSeparator(iData);
    }
};

template<typename U>
void vAddDelims(std::string& dest, const cDelimiter& me, const U data, const ps::lib::sql::ind_t& ind, const bool& iSep)
{
    me.vUnCls(dest, data, ind, iSep);
}

template<>                                          // Don't change to reference.
void vAddDelims(std::string& dest, const cDelimiter& me, const std::string data, const ps::lib::sql::ind_t& ind, const bool& iSep);

template<>
void vAddDelims(std::string& dest, const cDelimiter& me, const char* data, const ps::lib::sql::ind_t& ind, const bool& iSep);

template<>
void vAddDelims(std::string& dest, const cDelimiter& me, char* data, const ps::lib::sql::ind_t& ind, const bool& iSep);

void vSeparateToDelimiters(
    const std::string& sCompoStr  // format: [col_separator]<iSep>[begin_string_enclosure]<iSep>[end_string_enclosure]
    , const char* iSep
    , std::string& sColSeparator 
    , std::string& sEnclosure1 
    , std::string& sEnclosure2 
    , bool& iEndTerm
    , int32_t& iVarDigit
);

// makes a set of delimiters for the valiable length format. 
cDelimiter oMakeVarDelimiter();

} // ps::lib

} // ps

