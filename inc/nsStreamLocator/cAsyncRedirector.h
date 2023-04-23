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
namespace nsStreamLocator
{

class cAsyncRedirectorImpl;

/**
 * @class cAsyncRedirector
 * @brief
 * Forwarding a stream of data from a file descriptor given by vReadAsync
 * to a std::ostream given by CTOR until encountered EOF of the descriptor.
 * @note
 * Make to be a critical section between vReadAsync and vWait,
 * if you need to protect the ostream,
 * because of vReadAsync is not thread safe.
 */
class cAsyncRedirector
{
public:
    /**
     * @brief
     *  A std::basic_stream for forwarding destination.
     */
    cAsyncRedirector();
    /**
     * @brief
     */
    ~cAsyncRedirector();
    /**
     * @brief
     *   After calling this, control will be returned soon,
     *   since forwarding has done in background.
     * @param[in] iSrc
     *   A file descriptor for forwarding source.
     */
    void vReadAsync(const int& iSrc);
    /**
     * @brief
     *   waits to encounter EOF of the descriptor gvien by vReadAsync.
     */
    void vWait();
    /**
     * @brief
     */
    const std::string& sGetMsg() const;
private:
    std::unique_ptr<cAsyncRedirectorImpl, void(*)(cAsyncRedirectorImpl *)> oImpl_;
};

} // nsStreamLocator

} // lib

} // ps
