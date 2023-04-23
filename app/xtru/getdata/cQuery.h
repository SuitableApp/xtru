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

namespace getdata
{

class cQueryImpl;

/**
 * @class cQuery
 * @brief 
 * This class provides following roles.
 * -# Read user-defined queries from the values of two parameters,<br/>
 *    **queryvar** and **queryfix** defined in the conf file on the SA_HOME diirectory.<br/>
 *    There are three ways of specifying the reading method.
 *  -#  Specify a directory name where 0 or more query files are saved.
 *  -#  Specify a name of one query file.
 *  -#  Specify a query as it is (it must be terminated with a semicolon at the end of the statement)
 * -# If the query contains place folder(s), you must define a numeric value or a string value to bind to it. <br/>
 *    This mapping must be described in advance to the file of the name indicated by the value of <br/>
 *    the **filebind** parameter in :key=value expression.
 * -# Send the query to the server, format the result set received instead, and output it as a data file.<br/>
 *    The control file is generated based on the parameter descriptor received at the same time. <br/>
 *    The storage output of these files is determined by the parameter **output**.
 */
class cQuery
    : public ps::app::xtru::cFeature
{
private:
    std::unique_ptr<cQueryImpl> oImpl_;
    virtual void vRun();
public:
    cQuery();
};

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps

