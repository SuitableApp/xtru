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

class cTracer
    : public cOstream
    , public boost::serialization::singleton< cTracer >
{
    friend class boost::serialization::singleton< cTracer >;
private:
    boost::filesystem::path file_;       ///< Name of trace file.
    boost::filesystem::ofstream ofs_;    ///< a trace file.
    cTracer();  // will call by ctor of "singleton_wrapper<cTracer >".
    ~cTracer();
public:
    virtual void vAddTag(std::ostream& os);
    /// Leading a charactor '+' can specify append mode if fn is already exists. 
    cTracer& oRedirectTo(const boost::filesystem::path& fn);
    boost::filesystem::path sGetName() const;
};

} // ps::lib

} // ps

