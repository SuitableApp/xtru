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

namespace ps
{

namespace lib
{

namespace sql
{

namespace lite3
{

class cBind
{
private:
    cSqliteStmt& oStmt_;
    cAttr::tContainer oArr_;

    cBind& operator=(const cBind& rhs);
public:
    explicit cBind(cSqliteStmt& oStmt);
    cBind(const cBind& rhs);
    template<typename T>
    void vAddItem(
        const T& data
        , const cAttr::tLite3Type& iType
        , const ps::lib::sql::ind_t* iInd  // ture: Data present.
        , const size_t& iDSkip
        , const size_t& iISkip
    ){
        const int32_t pos = oArr_.size() + 1;
        oArr_.push_back(
            cAttr::oMakeInstance(
                (boost::format("%s_%d") % typeid(T).name() % pos).str()
                , oStmt_, pos, sizeof(T), iType
                , (void*) &data, iInd, iDSkip, iISkip
            )
        );
    }
    void vAddItem(
        const char* data
        , const cAttr::tLite3Type& iType
        , const ps::lib::sql::ind_t* iInd  // ture: Data present.
        , const size_t& iDSkip
        , const size_t& iISkip
    ){
        const int32_t pos = oArr_.size() + 1;
        oArr_.push_back(
            cAttr::oMakeInstance(
                (boost::format("str_%d") % pos).str()
                , oStmt_, pos, -1, iType
                , (void*) data, iInd, iDSkip, iISkip
            )
        );
    }
    void vAddItem(
        char* data
        , const cAttr::tLite3Type& iType
        , const ps::lib::sql::ind_t* iInd  // ture: Data present.
        , const size_t& iDSkip
        , const size_t& iISkip
    ){
        const int32_t pos = oArr_.size() + 1;
        oArr_.push_back(
            cAttr::oMakeInstance(
                (boost::format("str_%d") % pos).str()
                , oStmt_, pos, -1, iType
                , data, iInd, iDSkip, iISkip
            )
        );
    }
    void vAttachTo(const size_t& i);
    /**
     * @brief
     */
    size_t iGetSize(void) const
    {
        return oArr_.size();
    }
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps



