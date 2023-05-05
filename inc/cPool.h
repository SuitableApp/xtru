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
 * @class cPool
 * - Construct a specified number of T type instances
 *   (Or receive a reference to an externally existing T type array).
 * - A pointer to an instance can be put in and out
 *   according to a client request.
 * - Synchronization between threads is controlled so that
 *   supply and demand balance is maintained.
 * - As a result it provides the function of counting semaphore + α.
 *
 * @tparam T
 * - Type of instance to be put in/out
 * - A default constructor must be defined in T
 * @tparam DelT 
 *   Functor when destructing instance
 */
template<
    class T
    , class DelT = ps::lib::cRegularDeleter<T>
>
class cPool
{
private:
    /// A sink for the messages
    ps::lib::cDistributor& mos_;
    /// Maximum concurrent number
    uint32_t iParallelism_;
    /// Exclusive control when multiple threads access simultaneously
    std::mutex mtx_;
    /// It arbitrates resuming between the main and sub thread.
    std::condition_variable evt_;
    /// stores resource elements. Content varies dynamically.
    /// The elements stored here are inactive.
    std::stack<T*> cont_;
public:
    cPool(void)
        : cPool(0)
    {}
    /**
     * @brief
     * - Constructs a number of T type instances.
     * - Assume that it is called on the main thread.
     *
     * @param [in] iParallelism
     *    specifies concurrency.
     */
    explicit cPool(const uint32_t& iParallelism)
        : mos_(ps::lib::cDistributor::get_mutable_instance())
        , iParallelism_(iParallelism)
    {
        for (uint32_t i = 0; i < iParallelism_; ++i)
        {
            vPush(new T);
        }
    }
    /**
     * @brief
     * - Make an object stored in an existing array a resource pool.
     * - Concurrency is derived from the number of array elements.
     * - Assume that it is called on the main thread.
     *
     * @param [in] oArr
     *   An reference to array of pointers of T.
     */
    cPool(const ps::lib::cVector<T*>& oArr)
        : mos_(ps::lib::cDistributor::get_mutable_instance())
        , iParallelism_(oArr.size())
    {
        boost::for_each(
            oArr
            , [this](typename ps::lib::cVector<T*>::const_reference rf)
            {
                vPush(rf);
            }
        );
    }
    ~cPool()
    {
        DelT oDel;
        for (uint32_t i = 0; i < iParallelism_; ++i)
        {
            oDel(oPop());
        }
        iParallelism_ = 0;
    }
    /**
     * @brief
     * - Attempt to retrieve one element,
     *   but wait if all have been retrieved
     * - Reentrant reserve capacity decrease  by one.
     * - Assume that it is called on the main thread.
     *
     * @return
     *   A pointer of T.
     */
    T* oPop()
    {
        std::unique_lock<std::mutex> lk(mtx_);
        evt_.wait(
            // until one or more items return to the cont_.
            lk, [this]()->bool const
            {
                return ! cont_.empty();
            }
        );
        T* oObj = cont_.top();
        cont_.pop();
        return oObj;
    }
    /**
     * @brief
     * - Return a element to prepare for reuse.
     * - Free one waiting thread.
     * - Reentrant reserve capacity increase by one.
     * - Assume that it is called on the sub thread.
     */
    void vPush(T* oObj)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        cont_.push(oObj);
        // is waked up waiting thread at wait()
        evt_.notify_all();
    }
    /**
     * @brief
     * - Wait for all elements to be returned.
     * - Assume that it is called on the main thread.
     */
    void vSynchronize()
    {
        if (cont_.size() < iParallelism_)
        {
            std::unique_lock<std::mutex> lk(mtx_);
            mos_ << std::string("Waiting...") << std::endl;
            evt_.wait(
                // until all items return to the cont_.
                lk, [this]()->bool const
                {
                    return cont_.size() == iParallelism_;
                }
            );
            mos_ << std::string("Synchronized.") << std::endl;
        }
    }
private:
    cPool(const cPool&);
    cPool& operator=(const cPool&) const;
};

} // ps::lib

} // ps

