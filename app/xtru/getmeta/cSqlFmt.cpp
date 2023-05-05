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
#include "cSqlFmt.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

std::ostream& operator<<(std::ostream& os, cSqlFmt oMe)
{
    return oMe.oInvokeCbk(os);
}

std::ostream& cSqlFmt::oInvokeCbk(std::ostream& os)
try
{
    typedef boost::spirit::line_pos_iterator<std::string::const_iterator> pos_iterator_t;
    using ps::lib::sql::formatter::oracle::cGrammar;
    using ps::lib::sql::formatter::vParse;
    using ps::lib::sql::formatter::cDirector;
    const pos_iterator_t first_(sStmt_.begin())
                       , last_(sStmt_.end());
    pos_iterator_t       iter_(first_);
    cGrammar<pos_iterator_t> oParsar(first_);
    vParse(oParsar, first_, last_, iter_);
    const auto oDirective = oParsar.oGetDirective();
    const cDirector dire("  " /* spacer for indent */ );
    return os << dire.oConvert(
        first_.base(), last_.base()
        , oDirective.begin(), oDirective.end()
    );
}
catch (const std::runtime_error& e)
{
    ps::lib::cDistributor& mos_(ps::lib::cDistributor::get_mutable_instance());
    mos_ << e.what() << std::endl;
    return os << sStmt_;
}

cSqlFmt::cSqlFmt(const std::string& sStmt)
    : sStmt_(sStmt)
{}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
