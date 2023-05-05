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

#include <pslib.h>

namespace ps
{
 
namespace lib
{
void cDistributor::cMtxArray::vAddItem(cOstream& os)
{
    locks_.push_back(os.oGetMutex());
}

void cDistributor::cMtxArray::lock()
{
    for (auto it : locks_)
    {
        it->lock();
    }
}

void cDistributor::cMtxArray::unlock()
{
    for (auto it : locks_)
    {
        it->unlock();
    }
}

void cDistributor::vManipulate(std::ostream& (*pf)(std::ostream&))
{
    BOOST_ASSERT(vct_.size() > 0);
    for (auto it: vct_)
    {
        pf(*static_cast<std::ostream*>(it));
    }
}

cDistributor::cDistributor()
{}

cDistributor::~cDistributor()
{}

cDistributor& cDistributor::operator()(cOstream& os)
{
    vct_.push_back(&os);
    mtx_.vAddItem(os);
    return *this;
}

cSafeOstream<cDistributor, cDistributor::mutex_type> cDistributor::operator<<(std::ostream& (*pf)(std::ostream&))
{
    cSafeOstream<cDistributor, cDistributor::mutex_type> os(*this, mtx_
        , std::bind(&cDistributor::vManipulate, this, std::ref(pf)));
    return os;
}

} // ps::lib

} // ps

