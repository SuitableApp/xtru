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
 * @class cDistributor
 * @brief This provides following roles.
 * -# By pre-registering pointers to one or more cOstream instances,
 *    it is possible to provide a function to aggregate diagnostic information
 *    output operations to a single cDistributor object.
 *    Without repeating spelling of output operatio expressions to
 *    each cOstream instance.
 * -# Execution of an expression in which output operator whose lvalue is
 *     the cDistributor object are spelled in a chain guarantees thread safety
 *     until the last operator invocation is completed.
 *
 */
class cDistributor
    : public boost::serialization::singleton< cDistributor >
{
public:
    class cMtxArray
    {
    private:
        ps::lib::cVector<cOstream::mutex_type *> locks_;
    public:
        void vAddItem(cOstream& os);
        void lock();
        void unlock();
    };
    /**
     * @brief
     * A type in which lock() and unlock() are declared as public interfaces.
     */
    typedef cMtxArray mutex_type;
    friend class boost::serialization::singleton< cDistributor >;
    friend class cSafeOstream<cDistributor, mutex_type>;
private:
    ps::lib::cVector<cOstream *> vct_;
    cDistributor(); ///< will call by ctor of "singleton_wrapper<cDistributor>".
    ~cDistributor();
    mutex_type mtx_;
    /**
     * @brief
     *   For offering, call back from cSafeOstream
     */
    template<typename T>
        void vPutValue(const T& rhs);;
    /**
     * @brief
     *   For offering, call back from cSafeOstream
     */
    void vManipulate(std::ostream& (*pf)(std::ostream&));
public:
    /**
     * @brief
     *   For registering, new instance of cOstream.
     */
    cDistributor& operator()(cOstream& os);
    /**
     * @brief
     * The return value is not a reference.
     * The reason for copying intentionally is
     * to transfer ownership of mtx_ while lock is kept.
     */
    template <typename T>
        cSafeOstream<cDistributor, mutex_type> operator<<(const T& rhs);
    /**
     * @brief
     * The return value is not a reference.
     * The reason for copying intentionally is
     * to transfer ownership of mtx_ while lock is kept.
     */
    cSafeOstream<cDistributor, mutex_type> operator<<(std::ostream& (*pf)(std::ostream&));
};

template<typename T>
void cDistributor::vPutValue(const T& rhs)
{
    BOOST_ASSERT(vct_.size() > 0);
    for (auto it: vct_)
    {
        *static_cast<std::ostream*>(it) << rhs;
    }
}

template <typename T>
cSafeOstream<cDistributor, cDistributor::mutex_type> cDistributor::operator<<(const T& rhs)
{
    cSafeOstream<cDistributor, cDistributor::mutex_type> os(*this, mtx_
        , std::bind(&cDistributor::vPutValue<T>, this, std::cref(rhs)));
    return os;
}

} // ps::lib

} // ps

