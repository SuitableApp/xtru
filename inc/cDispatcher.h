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

template<class T>
using tManipOf = void (T::*)(void);

template<class T>
using tSequence = boost::ptr_deque<T>;

/**
 * @class cDispatcher
 * @brief This provides following roles.
 * -# The entity of the task and the function for manipulating it are held.
 * -# In order to optimize the resources to be used,
 *    it destructs itself when the task is completed.
 *
 * @tparam T
 * - A type that defines a task or its abstract data type.
 * - For T, a member function of signature expressed by tManipOf<T> must be declared.
 *
 */ 
template<class T>
class cDispatcher
{
private:
    typedef typename tSequence<T>::auto_type cSmartPtr;
    /**
     * - It points to the entity of the task and acts 'smartly'.
     * - Therefore, when an instance of cDispatcher disappears,
     *   its entity is also automatically destructed.
     */
    cSmartPtr oTask_;
    /**
     * - It is introduced from the argument of ps::lib::vSynchronize.
     * - It is used to call back procedures declared in T type.
     */
    tManipOf<T> vCbk_;
public:
    /**
     * @param [in] oTask
     * - In order to move ownership of pointee, pass a value as non-const.
     * @param[in] vCbk
     * - Must be a menber function of T.
     */
    cDispatcher(
        cSmartPtr oTask
        , tManipOf<T> vCbk
    )
        : oTask_(boost::ptr_container::move(oTask))
        , vCbk_(vCbk)
    {}
    /**
     * @details
     * - It is intended to be executed in a thread different
     *   from the thread when constructed.
     * - Procedure to do the following things in order:
     *   -# Calling cDispatcher::vCbk_.
     *   -# Destructing self.
     *   -# Recycling of used thread.
     *
     * @param [in] oThr
     * - The thread associated with the task.
     * - When entering this function is joinable state.
     *   The state is reversed when the task is completed.
     * @param [in,out] thrp
     * - It is a pool for recycling oThr and also serves as a synchronization object.
     * - When oThr is pushed to this, trying to pop the same pool,
     *   the blocking main thread is released.
     * @param [in,out] ep
     * - Non nullptr: An exception occurred in a member function was detected.
     */
    void vRun(
        std::thread* oThr
        , ps::lib::cPool<std::thread>& thrp
        , std::exception_ptr& ep
        , std::mutex& mtx
    ){
        try
        {
            // Call member function declared T type.
            (oTask_.get()->*vCbk_)();
        }
        catch (...)
        {
            // To throw an exception that occurred in this thread to the main thread.
            // Set a pointer to that exception.
            std::lock_guard<std::mutex> lock(mtx);
            // It would be sufficient if we could only distinguish one exception
            // which is caught earliest.
            if (ep == nullptr)
            {
                ep = std::current_exception();
            }
        }
        // It is intended to implicitly call T::~T().
        delete this;
        if (oThr->joinable())
        {
            oThr->detach();
        }
        // Prepare for recycling when the thread object is detached.
        // Simultaneously call ps::lib::cPool::oPop() to break
        // if there is a blocking thread.
        thrp.vPush(oThr);
    }
private:
    cDispatcher(const cDispatcher&);
    cDispatcher& operator=(const cDispatcher&);
};

/**
 * @brief
 * - This function retrieves tasks one by one in front of a sequence 'oTasks'.
 *   and asynchronouslly executes them.
 * - The parallel degree not exceeding the instructed iConcurrency is kept.
 * - The sequence of 'oTasks'' are consumed until it is empty.
 *
 * @param[in] iConcurrency
 * - A positive integer as upper limit of the number of tasks executed asynchronously.
 * @param[in,out] oTasks
 * - A sequence whose elements are smart pointers to instance of T type.
 * - This function blocks until the number of elements is zero.
 * @param[in,out] ep
 * - It is for propagating exceptions that occurred while executing
 *   an asynchronous task.
 * @param[in] vCbk
 * - Must be a pointer to menber function of T. 
 */
template<class T>
void vSynchronize(
    const int32_t& iConcurrency
    , tSequence<T>& oTasks
    , tManipOf<T> vCbk 
){
    ASSERT_OR_RAISE(iConcurrency > 0, std::runtime_error
        , boost::format(
          "iConcurrency must give an integer greater than or equal to 1. "
          "Actually %d is given."
        ) % iConcurrency
    );
    ps::lib::cPool<std::thread> thrp_(iConcurrency);
    BOOST_SCOPE_EXIT_TPL(&thrp_)
    {
        /* The contents in this block will always
         * be executed before exiting this function.
         * Even if an unhandled exception is thrown.
         */
        thrp_.vSynchronize();
    } BOOST_SCOPE_EXIT_END
    std::exception_ptr ep = nullptr;
    std::mutex mtx; // to protect ep.
    auto& rtn_(ps::lib::cRtn::get_mutable_instance());
    while (!oTasks.empty() && rtn_.iCotinue())
    {
        // This thread will be blocked by oPop() if thrp_ is empty.
        auto oThr = thrp_.oPop();
        *oThr = std::thread(
            &cDispatcher<T>::vRun
            // The item is consumed one by one in order from the head of the sequence.
            , new cDispatcher<T>(oTasks.pop_front(), vCbk)
            , oThr, std::ref(thrp_), std::ref(ep), std::ref(mtx)
        );
        {
            // Terminate early by canceling subsequent tasks without re-throwing.
            std::lock_guard<std::mutex> lock(mtx);
            if (ep)
            {
                rtn_.vOrValue(EXIT_FAILURE);
            }
        }
    }
    if (ep)
    {
        std::rethrow_exception(ep);
    }
}

} // ps::lib

} // ps

