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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @enum tWayToNext
 */
typedef enum
{ iForward = +1     ///< for 'DROP'
, iBackward = -1    ///< for 'CREATE'
} tWayToNext;

/**
 * @class cScriptMaker
 * @brief
 * This class provides following roles.
 * - 
 */
class cScriptMaker
    : private boost::noncopyable
{
private:
    using cChmod = ps::lib::system::cChmod;
    /// refers to a consistent flushing destination of various scripts.
    const boost::filesystem::path output_;
    /// open mode when creating a file.
    /**
     * @brief
     * - Scripts whose content does not change (not depending
     *   on the selected table or query) are output in batch.
     * - Output the following file:
     *   -# par0_txt_
     *   -# par1_txt_
     *   -# conn_sql_
     *   -# wait_here_
     * - The output location is defined in output_.
     */
    void vOutputScriptsRegular(
        const boost::filesystem::fstream::ios_base::openmode& mode
        , const boost::filesystem::path& make_sh
        , const boost::filesystem::path& par0_txt
        , const boost::filesystem::path& par1_txt
        , const boost::filesystem::path& conn_sql
        , const boost::filesystem::path& wait_here
    );
public:
    /**
     * @brief
     * @param[in] output
     *   A name of directory to save scripts.
     */
    cScriptMaker(const boost::filesystem::path output);
    ~cScriptMaker();
    /**
     * @brief
     *
     * @param[in,out] oCtx
     * @param[in] vGenerateScripts
     */
    template<class C, class F0>
    void vOutputScripts(
        C& oCtx
        , F0 vGenerateScripts
        , const boost::filesystem::fstream::ios_base::openmode& mode
        , const boost::filesystem::path& make_sh
        , const boost::filesystem::path& par0_txt
        , const boost::filesystem::path& par1_txt
        , const boost::filesystem::path& conn_sql
        , const boost::filesystem::path& wait_here
    ){
        // Retrieving dictionary data to describe.
        (oCtx.*vGenerateScripts)();
        // Output of invariant part.
        vOutputScriptsRegular(
            mode
            , make_sh
            , par0_txt
            , par1_txt
            , conn_sql
            , wait_here
        );
    }
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
