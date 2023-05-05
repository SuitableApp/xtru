/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Generate Credential File(mkcrd).
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
/**
 * @file nsCompo.h
 * @brief Common header for library class
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <memory>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "../define.h"
#include "cApplication.h"
#include "cArgUtils.h"
#include "cArgStyleParser.h"
#include "cBaseException.h"
#include "cEnvStrings.h"
#include "cStatCounter.h"
#include "cSwitchUtils.h"
#include "cVectorScope.h"
#include "nsContainerHelper.h"
