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

namespace mpx
{

class cAppLocalImpl;

/**
 * @class cAppLocal
 * @brief The central processing of @e MPX is defined.
 */
class cAppLocal
    : public ps::app::mpx::cFeature
{
private:
    std::unique_ptr<cAppLocalImpl> oImpl_;
    /**
     * @brief
     * The following processes are performed in order.
     * -# Changing the current directory. This refers to the startup
     *  (mpx::cStartValues::sCmdFile_) parameter.
     * -# Check and read the script file. This refers to the scr_make_sh
     *  (mpx::cStartValues::sCmdFile_) parameter.
     * -# Analyze the script file. Divide it into the first token (command)
     *  of the statement and the following part (argument list).
     *  If a place folder is found, it will expand to those immediate values.
     *  It stores the result in an array.
     * -# Prepare a character string to pass to the standard input of the
     *  child process. This refers to the csi (mpx::cStartValues::sCsi_) parameter.
     *  If the beginning of the string is the '@', it will do a special action.
     *  That is, it operates the mkcrd utility under specific conditions
     *  and uses the received character string.
     * -# Prepare files for receiving the standard output and error of
     *  the child process and the message sent to them.
     *  They refer to cso (mpx::cStartValues::sCso_)
     *  and cse (mpx::cStartValues::sCse_) parameter.
     * -# It executes sequentially using the data of the array prepared in the
     *  pre-processing. The parallel process target child process generates
     *  a thread and executes asynchronously with the specified degree of
     *  parallelism as the upper limit.
     *  parallelism (mpx::cStartValues::iParallelism_) is referenced
     *  as the upper limit value.
     * -# Wait until all asynchronous processing is completed.
     *  Then report the result.
     */
    void vRun();
public:
    cAppLocal();
};

} // ps::app::mpx

} // ps::app

} // ps

