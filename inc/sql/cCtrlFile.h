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

namespace sql
{

class cCtrlFileImpl;

/**
 * @class cCtrlFile
 * @brief
 * This class provides following roles.
 * -# Providing control file for driving SQL*Loader.
 */
class cCtrlFile
{
public:
    /**
     * @copydoc ps::lib::sql::cCtrlFileImpl::cCtrlFileImpl
     */
    cCtrlFile(
        const boost::filesystem::path& sFileName
        , const std::string& sTagName
        , const std::string& sPartitionName
        , const int32_t& iNumLongs
    );
    ~cCtrlFile();
    /**
     * @copydoc ps::lib::sql::cCtrlFileImpl::sGetGrammar
     */
    std::string sGetGrammar(void) const;
private:
    std::unique_ptr<cCtrlFileImpl> oImpl_;
};

} // ps::lib::sql

} // ps::lib

} // ps

