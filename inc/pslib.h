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

#ifndef PSLIB_H
#define PSLIB_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <stack>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <fstream>
#include <iterator>
#include <random>

/*
#include <regex>
 * Adopt in "boost::regex" as a regular expression library instead of "std::regex".
 * Because std::regex was not supported fully until g++-4.9.
 */
namespace spf = std::placeholders;
#include <condition_variable>
#include <future>
#include <memory>
#include <ostream>
#include <thread>

#define BOOST_ENABLE_ASSERT_HANDLER // Must be placed before assert.hpp.
#include <boost/assert.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/exception/all.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/process.hpp>
#include <boost/ptr_container/ptr_container.hpp>
#include <boost/regex.hpp>
#include <boost/utility.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/scope_exit.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

#include <occi.h>                  // Programing interfaces for Oracle client
#include <sqlite3ext.h>            // Programing interfaces for Sqlite3
//#include <google/profiler.h>       // for the diagnosis.


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// ps::lib
#include "types.h"
#include "cEnclosure.h"
#include "nsPsCommon.h"
#if __GNUC__ >= 4
#include "cBackTrace.h"
#endif
#include "cLocale.h"
#include "cEnv.h"
#include "cRtn.h"
#include "cStat.h"
#include "cStreamBuf.h"
#include "cSafeOstream.h"
#include "cOstream.h"
#include "cDistributor.h"
#include "cTracer.h"
#include "cConsole.h"
#include "cConfigures.h"
#include "nsEffector.h"
#include "cSemaphore.h"
#include "cPool.h"
#include "cSignal.h"
#include "sql/nsSql.h"
#include "cDelimiter.h"
#include "cIntervalTimer.h"
#include "sql/cCtrlFile.h"
#include "cDispatcher.h"
#include "nsStreamLocator/nsStreamLocator.h"
#include "nsStreamLocator/cStreamSupplier.h"
#include "nsStreamLocator/cStreamLocator.h"
#include "nsStreamLocator/cLocalProcess.h"
#include "nsStreamLocator/cNamedPipe.h"
#include "nsStreamLocator/cAsyncRedirector.h"
#include "nsStreamLocator/cFileSystem.h"
// ps::lib::sql
#include "sql/cFetchable.h"
// ps::lib::sql::occi
#include "sql/occi/nsPsOcci.h"
#include "sql/occi/cOciErr.h"
#include "sql/occi/cOciStmt.h"
#include "sql/occi/cPieceVct.h"
#include "sql/occi/cBind.h"
#include "sql/occi/cDefine.h"
#include "sql/occi/cFailover.h"
#include "sql/occi/cAllocator.h"
#include "sql/occi/cSvc.h"
#include "sql/occi/cSetCurrentSchema.h"
#include "sql/occi/cAttr.h"
#include "sql/occi/cStmt.h"
#include "sql/occi/cMetaData.h"
#include "sql/occi/cUnloader.h"
// ps::lib::sql::lite3
#include "sql/lite3/cSqliteDb.h"
#include "sql/lite3/cAttr.h"
#include "sql/lite3/cBind.h"
#include "sql/lite3/cDefine.h"
#include "sql/lite3/cSqliteStmt.h"
#include "sql/lite3/cDirectiveHolder.h"
#include "sql/lite3/nsHelper.h"
// ps::lib::sql
#include "sql/cTransporter.h"
// ps::lib::sql::formatter
#include "sql/formatter/nsFormatter.h"
#include "sql/formatter/cAnnotator.h"
#include "sql/formatter/cDirector.h"
#include "sql/formatter/oracle/cGrammar.h"
// ps::lib::system
#include "system/nsSystem.h"
#include "system/cChmod.h"

#endif /* PSLIB_H */
