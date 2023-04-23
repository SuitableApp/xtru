/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Generate Credential File(mkcrd).
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
/**
 * @file cStatCounter.cpp
 * @brief OS statistics counter
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include "nsCompo.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

struct timeval cSimpleStats::tLiFreq_;

/**
 * @details
 */
cSimpleStats::cSimpleStats()
    : iCount_(0)
{
    static tSingleton s;        // Scott Meyers Singleton
    fLiTotalTick_ = 0.0;
}

/**
 * @details
 */
const struct timeval cSimpleStats::tLiGetPerfCounter() const
{
    struct timeval tLiCurTick;
    gettimeofday(&tLiCurTick, nullptr);
    return tLiCurTick;
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
