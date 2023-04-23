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

/**
 * @class cSafeOstream
 * @brief
 * -# Guarantee thread safety until all cascading output operator calls
 *    with __Ostream objects as lvalues are completed. <br/>
 *    By exclusively controlling competing activities
 *    of all instances sharing __Lockable.
 */

template<typename __Ostream, class __Lockable>
class cSafeOstream
{
public:
    /**
     * @brief
     * Assume that this object is built with the leftmost operation
     *   in a chainwise operator<<() call, where the __Ostream object
     *   is passed to the left side.
     * @param[in] os
     * @param[in] mtx
     * @param[in] oLeftmost
     *   Only the leftmost output operation is implemented
     *   with a functor delegated from the _Ostream class.
     */
    cSafeOstream(
        __Ostream& os
        , __Lockable& mtx
        , std::function<void(__Ostream&)> oLeftmost
    )
        : os_(os)
        , lock_(mtx)
    {
        // Writing to the __Ostream object is protected from conflicting
        // activities sharing mtx since lock_ has been activated
        // in the member initializer lists immediately before.
        oLeftmost(os_);
    }
    /**
     * @brief
     * @param[in] rhs
     * @return self
     */
    template<typename T>
    cSafeOstream& operator<<(const T& rhs)
    {
        auto func = std::bind(&__Ostream::template vPutValue<T>, &os_, std::cref(rhs));
        func(os_);
        return *this;
    }
    /**
     * @brief
     * @param[in] pf
     * @return self
     */
    cSafeOstream& operator<<(std::ostream& (*pf)(std::ostream&))
    {
        auto func = std::bind(&__Ostream::vManipulate, &os_, std::ref(pf));
        func(os_);
        return *this;
    }
private:
    /**
     * @brief
     * -# The target of __Ostream is a custom class with operator<<() overloaded.
     * -# In addition, it must have a function named vPutValue having an arbitrary
     *     concrete type as a const reference argument and a function named vManipulate
     *     whose argument type is the address of the manipulator.
     * -# Finally, it must be a friend of cSafeOstream.
     */
    __Ostream& os_;
    /**
     * @brief
     * -# std::unique_lock moves __Lockable without state change when default
     *    copy constructor or assignment operator are called.
     * -# Therefore, __Lockable is kept beyond the lifetime of the cSafeOstream
     *    copy constructor argument or the assignment operator's rvalue.
     * -# A concrete type that has lock() and unlock() on the public interface is
     *    accepted as __Lockable.
     */
    std::unique_lock<__Lockable> lock_;
};

} // lib

} // ps

