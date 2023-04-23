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
 * @file define.h
 * @brief Various definitions
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef DEFINE_H
#define DEFINE_H
#include <vector>
#include <set>
#include <stdint.h>
#include <map>

// Application settings
#define APP_NAME "mkcrd"
#define APP_VERSION "v1.0"
#define APP_COPYRIGHT "(c) 2023 SuitableApp"

// Retrieve the path to the executable file of the currently executing process
#define PROC_PATH "/proc/self/exe"

#define tcout std::cout
#define tcin std::cin
#define tcerr std::cerr
#define tstring std::string
#define _MAX_PATH 256
#define TEXT(x) x

namespace ps
{
namespace app
{
namespace mkcrd
{
typedef uint32_t        DWORD;
typedef unsigned short  WORD;
typedef uint8_t         BYTE;
typedef BYTE*           LPBYTE;
typedef unsigned int    UNINT32;
typedef int32_t         LONG;
typedef int64_t         LONGLONG;
typedef char*           LPTSTR;
typedef char            TCHAR;
typedef char            _TCHAR;
typedef std::basic_istringstream<TCHAR> tistringstream;
typedef std::basic_ostringstream<TCHAR> tostringstream;
typedef std::basic_ifstream<TCHAR> tifstream;
typedef std::basic_ofstream<TCHAR> tofstream;
typedef std::ostream tostream;
typedef std::ostream tistream;
typedef std::vector<tstring> tstrvector;
typedef std::set<tstring > tstrset;
typedef std::map<tstring, tstring > tstrmap;

typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

} // ps::app::mkcrd
} // ps::app
} // ps
#endif
