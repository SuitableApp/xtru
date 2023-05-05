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

/**
 * @class cOstream
 * @brief This provides following roles.
 * -# provides functions useful for outputting diagnostic information from a program
 *    to a sink device such as a console or a trace file in a simple style.
 * -# Assuming that it is used as an argument type of a friend cSafeOstream template,
 *    it is an expression composed of concatenated output operators to guarantee
 *    thread safety until the last operator invocation is completed.
 * -# By registering one or more cOstream instances to a friend cDistributor object,
 *    it can be aggregated into a single output operation expression
 *    to cDistributor object.
 *    Without spelling repeatedly the output operation expression to
 *    each cOstream instance.
 */
class cOstream
    : public std::ostream  
    , private boost::noncopyable
{
    /**
     * @brief
     * A type in which lock() and unlock() are declared as public interfaces.
     */
    typedef std::mutex mutex_type;
    friend class cSafeOstream<cOstream, mutex_type>;
    friend class cDistributor;
private:
    cStreamBuf oBuf_;
    bool iTagDateTimeEnabled_;
    mutex_type mtx_;
    /**
     * @brief
     *   For offering, call back from cDistributor
     */
    mutex_type* oGetMutex(void);
    /**
     * @brief
     *   For offering, call back from cSafeOstream
     */
    template<typename T>
        void vPutValue(const T& rhs);
    /**
     * @brief
     *   For offering, call back from cSafeOstream
     */
    void vManipulate(std::ostream& (*pf)(std::ostream&));
protected:
    /**
     * @brief
     */
    cOstream(std::ostream& os, cOstream* ptr);
    virtual ~cOstream();
public:
    /**
     * @brief
     */
    virtual void vAddTag(std::ostream&) =0;
    /**
     * @brief
     * The return value is not a reference.
     * The reason for copying intentionally is
     * to transfer ownership of mtx_ while lock is kept.
     */
    template<typename T>
        cSafeOstream<cOstream, mutex_type> operator<<(const T& rhs);
    /**
     * @brief
     * The return value is not a reference.
     * The reason for copying intentionally is
     * to transfer ownership of mtx_ while lock is kept.
     */
    cSafeOstream<cOstream, mutex_type> operator<<(std::ostream& (*pf)(std::ostream&));
    /**
     * @brief
     */
    bool iGetTagDateTimeEnabled() const;
    /**
     * @brief
     */
    void vSwitchTagDateTime();
};

template<typename T>
void cOstream::vPutValue(const T& rhs)
{
    *static_cast<std::ostream*>(this) << rhs;
}

template<typename T>
cSafeOstream<cOstream, cOstream::mutex_type> cOstream::operator<<(const T& rhs)
{
    cSafeOstream<cOstream, cOstream::mutex_type> os(*this, mtx_
        , std::bind(&cOstream::vPutValue<T>, this, std::cref(rhs)));
    return os;
}

} // ps::lib

} // ps

