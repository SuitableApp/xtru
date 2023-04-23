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

#pragma once

#define COMPRES_LEN    8+1
#define COMPFOR_LEN   18+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cCompressObject
 */

class cCompressObject
{
private:
    std::string szCompression_;
	std::string szCompressFor_;
	ps::lib::sql::ind_t nCompressForInd_;
    std::string sClause_;
public:
    //// @Param [in] szCompression
    //// @Param [in] szCompressFor
    //// @Param [in] nCompressForInd
    cCompressObject(
        const char szCompression[]
        , const char szCompressFor[]
        , const ps::lib::sql::ind_t& nCompressForInd
    );
    ~cCompressObject();
	std::string sGetClause(void) const;
	friend const bool operator==(
        const cCompressObject& lhs
        , const cCompressObject& rhs
    );
	friend const bool operator!=(
        const cCompressObject& lhs
        , const cCompressObject& rhs
    );
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
