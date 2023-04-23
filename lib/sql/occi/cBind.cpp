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

namespace occi
{

namespace /* anonymous */
{

typedef ps::lib::cMap<std::string, oracle::occi::Type> tTypeMap;
const tTypeMap oTypeMap =
{    {typeid(std::string).name(),        oracle::occi::OCCI_SQLT_CHR}
,    {typeid(char*).name(),              oracle::occi::OCCI_SQLT_STR}
,    {typeid(int32_t).name(),            oracle::occi::OCCIINT}
,    {typeid(uint32_t).name(),           oracle::occi::OCCIUNSIGNED_INT}
,    {typeid(int32_t).name(),            oracle::occi::OCCIINT}
,    {typeid(uint32_t).name(),           oracle::occi::OCCIUNSIGNED_INT}
,    {typeid(int64_t).name(),            oracle::occi::OCCIINT}
,    {typeid(uint64_t).name(),           oracle::occi::OCCIUNSIGNED_INT}
,    {typeid(float).name(),              oracle::occi::OCCIBFLOAT}
,    {typeid(double).name(),             oracle::occi::OCCIBDOUBLE}
};

} /* anonymous */

cBind::tBind::tBind(
    uint32_t pos
    , void *valuep
    , int32_t value_sz
    , const std::string& tnm
    , int16_t *indp
    , uint16_t *alenp
    , uint16_t *rcodep
    , uint32_t pvskip
    , uint32_t indskip
    , uint32_t alskip
    , uint32_t rcskip
    , const std::string& sVal
)
    : pos_(pos), valuep_(valuep), value_sz_(value_sz)
    , tnm_(tnm), indp_(indp), alenp_(alenp), rcodep_(rcodep)
    , pvskip_(pvskip), indskip_(indskip), alskip_(alskip), rcskip_(rcskip)
    , sVal_(sVal)
{
    BOOST_ASSERT(pos_);
    BOOST_ASSERT(valuep_);
    BOOST_ASSERT(value_sz_);
    BOOST_ASSERT(tnm_.size());
}

cBind::tBind::tBind(const tBind& rhs)
    : pos_(rhs.pos_), valuep_(rhs.valuep_), value_sz_(rhs.value_sz_)
    , tnm_(rhs.tnm_), indp_(rhs.indp_), alenp_(rhs.alenp_), rcodep_(rhs.rcodep_)
    , pvskip_(rhs.pvskip_), indskip_(rhs.indskip_), alskip_(rhs.alskip_), rcskip_(rhs.rcskip_)
    , sVal_(rhs.sVal_)
{}

cBind::tBind& cBind::tBind::operator=(const tBind& rhs)
{
    pos_ = rhs.pos_; valuep_ = rhs.valuep_; value_sz_ = rhs.value_sz_;
    tnm_ = rhs.tnm_; indp_ = rhs.indp_; alenp_ = rhs.alenp_; rcodep_ = rhs.rcodep_;
    pvskip_ = rhs.pvskip_; indskip_ = rhs.indskip_; alskip_ = rhs.alskip_; rcskip_ = rhs.rcskip_;
    sVal_ = rhs.sVal_;
    return *this;
}

cBind::cBind()
{}

cBind::cBind(const cBind& rhs)
    : oMap_(rhs.oMap_)
{}

cBind& cBind::operator=(const cBind& rhs)
{
    oMap_ = rhs.oMap_;
    return *this;
}

bool cBind::find(const tType::key_type& phn) const
{
    ASSERT_OR_RAISE(
        !oMap_.empty()
        , std::runtime_error, "FAILED: oMap_ is empty."
    );
    return oMap_.find(phn) != oMap_.end();
}

std::string cBind::sGetTypeIdName(const tType::key_type& phn) const
{
    ASSERT_OR_RAISE(
        !oMap_.empty()
        , std::runtime_error, "FAILED: oMap_ is empty."
    );
    return oMap_.at(phn).tnm_;
}

std::string cBind::sAttachTo(oracle::occi::Statement *stmt, const std::string& phn, const int32_t param_pos) const
{
    BOOST_ASSERT(stmt);
    ASSERT_OR_RAISE(
        !oMap_.empty()
        , std::runtime_error, "FAILED: oMap_ is empty."
    );
    const auto& val = oMap_.at(phn);
    stmt->setDataBuffer(
        param_pos // This parameter originate from 1.
        , val.valuep_
        , oTypeMap.at(val.tnm_)
        , val.value_sz_
        , val.alenp_
        , val.indp_
        , val.rcodep_
    );
    return val.sVal_;
}

ps::lib::str_vct cBind::sPrintItems() const
{
    ps::lib::str_vct rtn;
    for (const auto oItem: oMap_)
    {
        rtn.push_back(boost::io::str(
            boost::format("(typ=%s)'%s'='%s'")
            % oItem.second.tnm_
            % oItem.first
            % oItem.second.sVal_
        ));
    }
    return rtn;
}

namespace nsParser
{

using boost::spirit::eol;
using boost::spirit::qi::char_;
using boost::spirit::qi::grammar;
using boost::spirit::qi::rule;
using boost::spirit::qi::parse;
using boost::spirit::ascii::blank;

typedef ps::lib::cMap<std::string, std::string> tCont;

template<typename Iterator>
struct cBindGrammar
    : public grammar<Iterator, tCont()>
{
    cBind *oImpl_;
    std::string phn_;
    cBindGrammar(cBind *oImpl, int32_t& iNewLine_)
        : cBindGrammar::base_type(query)
        , oImpl_(oImpl)
    {
        /// Semantic actions.
        auto when_next = [&]{ ++iNewLine_; };
        auto when_key = [&](const std::string& v_){ phn_ = v_; };
        auto when_num = [&](const std::string& v_)
        {
            if (v_.find_first_of(".") != std::string::npos)
            {
                oImpl_->vStoreItem(
                    phn_, boost::lexical_cast<double>(v_)
                    , [](const double& v)->const double *{ return &v; }
                );
            }
            else
            {
                oImpl_->vStoreItem(
                    phn_, boost::lexical_cast<int64_t>(v_)
                    , [](const int64_t& v)->const int64_t *{ return &v; }
                );
            }
        };
        auto when_str = [&](const std::string& v_)
        {
            oImpl_->vStoreItem(
                phn_, v_
                , [](const std::string& v)->const std::string &{ return v; }
            );
        };
        /// Non-terminal symbols
        query =
            pair >> *(pair_separator[when_next] >> pair);
        pair =
            ( key                [when_key]
              | voidline | comment
          ) >> -('=' >>
            ( num_value          [when_num]
            | (
                  double_quoted
                | bracket_quoted
                | brace_quoted
                | parenthes_quoted
                | inequal_quoted
            )                    [when_str]
            )) >> -voidline >> -comment;
        key =
            char_(':') >> char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");
        /// Terminal symbols
        num_value =
            -char_("-+") >> char_("1-9") >> *char_("0-9") >>
            -(char_('.') >> *char_("0-9"));
        double_quoted =
            '\"' >> +(char_ - '\"') >> '\"';
        bracket_quoted =
            '[' >> +(char_ - ']') >> ']';
        brace_quoted =
            '{' >> +(char_ - '}') >> '}';
        parenthes_quoted =
            '(' >> +(char_ - ')') >> ')';
        inequal_quoted =
            '<' >> +(char_ - '>') >> '>';
        /// Non-semantic
        comment = char_("#") >> *(char_ - eol);
        voidline = *(blank - eol);
        pair_separator = *blank >> eol | char_(',') >> *blank;
    }
    /// Rules
    rule<Iterator, tCont()> query;
    rule<Iterator, std::pair<std::string, std::string>()> pair;
    rule<Iterator, std::string()> key, num_value
        , double_quoted, bracket_quoted, brace_quoted, parenthes_quoted, inequal_quoted
        , voidline, comment, pair_separator;
};

} //nsParser

void cBind::vImportParameters(const boost::filesystem::path& sFile)
{
    BOOST_ASSERT(!sFile.empty());
    std::string str;
    if (boost::filesystem::exists(sFile))
    {
        ASSERT_OR_RAISE(
             boost::filesystem::is_regular(sFile)
            , std::runtime_error
            , boost::format("File: %s is not exist or a regular file.") % sFile.string()
        );
        boost::filesystem::ifstream ifs(sFile);
        // Reading all image data.
        str = std::string(
            std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(/*EOF*/)
        );
    }
    else
    {
        str = sFile.leaf().string();
    }
    auto it = str.cbegin(), end = str.cend();
    // Extructing key=value pairs from iterator.
    nsParser::tCont map;
    int32_t iLine = 1;
    nsParser::cBindGrammar<std::string::const_iterator> oBindGrammar(this, iLine);
    // Parsing
    const auto ret = nsParser::parse(it, end, oBindGrammar, map);
    ASSERT_OR_RAISE(
        ret
        , std::runtime_error
        , boost::format("Faild to parse with %s.") % sFile.leaf()
    );
    ASSERT_OR_RAISE(
        it == end
        , std::runtime_error
        , boost::format("Encounterd syntax error to around for [%.20s...] at line-%d in %s.")
            % std::string(it, end) % iLine % sFile.leaf()
    );
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
