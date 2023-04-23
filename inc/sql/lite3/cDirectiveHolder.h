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

class cDirectiveHolder
    : public ps::lib::sql::cFetchable
{
private:
    typedef std::function<void()> tCbk;
    tCbk vPostBulkCbk_;
    tCbk vPreRepeatCbk_;
    tCbk vPostRepeatCbk_;
    tCbk vNotFoundCbk_;
    tCbk vFinalizeCbk_;
public:
    cDirectiveHolder(
        tCbk vPostBulkCbk // mandatory
        , tCbk vPreRepeatCbk = boost::value_initialized<tCbk>()
        , tCbk vPostRepeatCbk = boost::value_initialized<tCbk>()
        , tCbk vNotFoundCbk = boost::value_initialized<tCbk>()
        , tCbk vFinalizeCbk = boost::value_initialized<tCbk>()
    )
        : vPostBulkCbk_(vPostBulkCbk)
        , vPreRepeatCbk_(vPreRepeatCbk) 
        , vPostRepeatCbk_(vPostRepeatCbk)
        , vNotFoundCbk_(vNotFoundCbk)
        , vFinalizeCbk_(vFinalizeCbk)
    {}
    virtual ~cDirectiveHolder()
    {}
    virtual void vExecuteAndFetch()
    {}
    virtual void vPreRepeatAction()
    {
        if (vPreRepeatCbk_)
        {
            vPreRepeatCbk_();
        }
    }
    virtual void vPostRepeatAction()
    {
        if (vPostRepeatCbk_)
        {
            vPostRepeatCbk_();
        }
    }
    virtual void vNotFoundAction()
    {
        if (vNotFoundCbk_)
        {
            vNotFoundCbk_();
        }
    }
    virtual void vPreBulkAction(const uint32_t& iBulkSize)
    {}
    virtual void vPostBulkAction(const uint32_t& iNumIter)
    {
        vPostBulkCbk_();
    }
    virtual void vFeedbackAction()
    {}
    virtual void vFinalizeAction()
    {
        if (vFinalizeCbk_)
        {
            vFinalizeCbk_();
        }
    }
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

