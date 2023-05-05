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

namespace demo
{

class cAllSource
    : public ps::lib::sql::occi::cUnloader
{
private:
    struct tAttributes;
    static const char szStmt_[];
    static const uint32_t iBulkSize_;
    ps::lib::sql::occi::cBind oBind_;
    ps::lib::sql::occi::cDefine& oDefine_;
    const size_t iSkip_;
    ps::lib::cVector<tAttributes *> bulk_;
    ps::lib::cVector<uint32_t> iters_;
public:
    cAllSource(const std::string& sOwner, ps::lib::sql::occi::cSvc&);
private:
    virtual ~cAllSource();
    virtual void vPreRepeatAction();
    virtual void vPostRepeatAction();
    virtual void vNotFoundAction() {}
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    virtual void vPostBulkAction(const uint32_t& iNumIter);
};

} // ps::demo

} // ps

