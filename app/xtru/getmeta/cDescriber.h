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

namespace app
{

namespace xtru
{

namespace getmeta
{

class cDescriber
    : public ps::lib::sql::cFetchable
    , private ps::lib::sql::occi::cStmt
{
protected:
    ps::lib::cStat& stat_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
private:
    const std::string tag_;
    int64_t iTotalBytes_;
    uint32_t iTotalRows_;
    const bool iSkipExecute_;
    void vShowStatValue() const;
public:
    cDescriber(
        ps::lib::sql::occi::cSvc& oSvc
        , const uint32_t& iBulkSize
        , const std::string& szInStmt
        , const std::string& tag
        , ps::lib::sql::occi::cBind* oIBind
        , const bool& iSkipExecute
    );
    cDescriber(
        ps::lib::sql::occi::cSvc& oSvc
        , const uint32_t& iBulkSize
        , const std::string& szInStmt
        , const std::string& tag
        , ps::lib::sql::occi::cBind* oIBind
    );
    virtual ~cDescriber();
    virtual void vExecuteAndFetch();
    virtual void vPreRepeatAction();
    virtual void vNotFoundAction();
    virtual void vFeedbackAction();
    virtual void vFinalizeAction();
protected:
    void vAddOutputBytes(const int64_t& iOutputBytes);
    void vAddOutputRows(const int32_t& iOutputRows);
    ps::lib::sql::occi::cDefine& oGetDefine();
    /**
     * @brief
     *   Replace "%s" embedded in SQL with the elements
     *   of the string vector in order.
     *
     * @param [in] opts
     *   stores values for %s place-holder in sql_.
     */
    void vConvPlaceHolder(
        const ps::lib::str_vct& opts
    );
private:
    cDescriber(const cDescriber&) =delete;
    cDescriber& operator=(const cDescriber&) =delete;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
