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

#define NO_SCHEMA_CHAR                    "\"$#" ///< Characters that can not be used as part of the schema name.
#define OBJECT_NAME_LEN                  (120+1) ///< Length of schema-object name.
#define OBJECT_TYPE_LEN                  (120+1) ///< Length of schema-object type.
#define DATETIME_STR_LEN                  (19+1) ///< Length of 'YYYY-MM-DD HH:MI:SS'.
#define DATETIME_FMT_STR "YYYY-MM-DD HH24:MI:SS" ///< Date-time format string of standard.
#define TIMESTAMP_STR_LEN                 (26+1) ///< Length of YYYY-MM-DD HH:MI:SS TZH:TZM'.
#define TIMESTAMP_FMT_STR "YYYY-MM-DD HH24:MI:SS TZH:TZM" ///< Timestamp format string of standard.
#define MAX_DBALIAS_LEN                  (128+1)
#define MAX_CONNECT_STRING_LEN (OBJECT_NAME_LEN * 2) + MAX_DBALIAS_LEN + 2
#define BOM_OF_UTF16BE                 "\xfe\xff"
#define BOM_OF_UTF8                "\xef\xbb\xbf"
#define BOM_LENGTH_OF_UTF16  (sizeof(BOM_OF_UTF16BE) - 1)
#define BOM_LENGTH_OF_UTF8   (sizeof(BOM_OF_UTF8) - 1)
#define OCI_UTF16BEID                (ub2) 2000   ///< 1000 == OCI_UTF16ID
#define NUM_DIGITS_VARCHARC                  10


#define DEBUG_PRINT(fn, ln) {\
    auto& __output = ps::lib::cConsole::get_mutable_instance(); \
    __output << boost::format("%s-%d: %s") % fn % ln % sDebugPrint() << std::endl; \
}

namespace ps
{

namespace lib
{

/**
 * @brief
 * Screen display of software name, version, license terms.
 */
std::ostream& oNotice(std::ostream& out);

/**
 * @brief
 * Sleep for a specified number of seconds.
 */
void vSleepSecs(const int32_t& iSecs);

/**
 * @brief
 * - Throws exception E if expr is false. msg is a message string given to E.
 * - Write the call stack information to the trace file using the object cBackTrace.
 *
 * @param[in] expr
 *   Linguistic expression of Boolean expressions
 * @param[in] en
 *   An Exception name thrown.
 * @param[in] fn
 *   Function name of exception occurrence place.
 * @param[in] fl
 *   Name of the source code where the exception occurred.
 * @param[in] ln
 *   Line number of exception occurrence place.
 * @param[in] msg
 *   message
 */
template<class E>
void vRaiseWithMsg(
    const char* expr
    , const char* en
    , const char* fn
    , const char* fl
    , const int32_t& ln
    , const std::string& msg
){
    std::ostringstream oss;
    oss << msg << boost::format(
        " %s in %s (%s-%d): Assertion '%s' failed."
    ) % en % fn % fl % ln % expr;
#ifdef PS_UTL_CBACKTRACE_H
    cBackTrace bt(oss);
#endif
    throw E(oss.str());
}

/**
 * @brief
 * - Throws exception E if expr is false. msg is a message string given to E.
 * - Write the call stack information to the trace file using the object cBackTrace.
 *
 * @param[in] expr
 *   Linguistic expression of Boolean expressions
 * @param[in] en
 *   An Exception name thrown.
 * @param[in] fn
 *   Function name of exception occurrence place.
 * @param[in] fl
 *   Name of the source code where the exception occurred.
 * @param[in] ln
 *   Line number of exception occurrence place.
 * @param[in] fmt
 *   An expression containing a boost::format function call that generates a message.
 */
template<class E>
void vRaiseWithMsg(
    const char* expr
    , const char* en
    , const char* fn
    , const char* fl
    , const int32_t& ln
    , const boost::format& fmt
){
    vRaiseWithMsg<E>(expr, en, fn, fl, ln, fmt.str());
}

#define ASSERT_OR_RAISE(cond, ex, msg) \
    {if (!(cond)) ps::lib::vRaiseWithMsg<ex>(#cond, #ex, __PRETTY_FUNCTION__, __FILE__, __LINE__, msg);}

#define ASSERT_OR_RAISE_FNC(cond, ex, func) \
    {if (!(cond)) ps::lib::vRaiseWithMsg<ex>(#cond, #ex, __PRETTY_FUNCTION__, __FILE__, __LINE__, func());}

/**
 * @brief
 * - Throws exception E. msg is a message string given to E.
 * - Write the call stack information to the trace file using the object cBackTrace.
 *
 * @param[in] en
 *   An Exception name thrown.
 * @param[in] fn
 *   Function name of exception occurrence place.
 * @param[in] fl
 *   Name of the source code where the exception occurred.
 * @param[in] ln
 *   Line number of exception occurrence place.
 * @param[in] msg
 *   message
 */
template<class E>
void vRaiseWithMsg(
    const char* en
    , const char* fn
    , const char* fl
    , const int32_t& ln
    , const std::string& msg
){
    std::ostringstream oss;
    oss << msg << boost::format(" %s in %s (%s-%d)") % en % fn % fl % ln;
#ifdef PS_UTL_CBACKTRACE_H
    cBackTrace bt(oss);
#endif
    throw E(oss.str());
}

/**
 * @brief
 * - Throws exception E. msg is a message string given to E.
 * - Write the call stack information to the trace file using the object cBackTrace.
 *
 * @param[in] en
 *   An Exception name thrown.
 * @param[in] fn
 *   Function name of exception occurrence place.
 * @param[in] fl
 *   Name of the source code where the exception occurred.
 * @param[in] ln
 *   Line number of exception occurrence place.
 * @param[in] fmt
 *   An expression containing a boost::format function call that generates a message.
 */
template<class E>
void vRaiseWithMsg(
    const char* en
    , const char* fn
    , const char* fl
    , const int32_t& ln
    , const boost::format& fmt
){
    vRaiseWithMsg<E>(en, fn, fl, ln, fmt.str());
}

#define RAISE_EX_CONVERT(ex, msg) \
    {ps::lib::vRaiseWithMsg<ex>(#ex, __PRETTY_FUNCTION__, __FILE__, __LINE__, msg);}

/**
 * @brief
 * - Wait for one letter of 'Y', 'y', 'N' and 'n'.
 * - Continue urging input by displaying a prompt until the correct one character is input.
 *
 * @param[in,out] os
 *   Destination for prompting to re-enter.
 */
extern int32_t vConfirmation(std::ostream& os);
/**
 * @brief
 * searches sPath for a home directory that uses to save the configuration
 * for application or data.<br/>
 * - if sPath is, nothing is done and returns false.
 * - else if sPath is not, You will be prompted to make it.<br/>
 * if you agree to make a home directory, actually it will be made.
 * - else nothing is done and it will be returned a value of true.
 */
extern bool iCheckHomeOrMakeIt(const boost::filesystem::path& sPath);
/**
 * @brief
 * - A configuration file that should be placed in
 *   sPath+"/"+sPkgName+".conf" will be searched.
 * - If it is not, a tarfile (including it) that should be placed in
 *   sPkgDataDir+"/"+sPkgName+".tar" will be expand to sPath.<br/>
 * - This returns a value of true when tarfile is not found, and
 *   configration file is not found.
 * - In the latter case, it requires inplicitly to rerun the application.
 *   To reflect the contents of the expanded conf file to the behavior of the application.
 *
 * @param[in] sPath
 *   stores application home dhirectory.
 * @param[in] sPkgDataDir
 * @param[in] sPkgName
 */
extern bool iCheckConfOrMakeIt(
    const boost::filesystem::path& sPath
    , const boost::filesystem::path& sPreFixDir
    , const boost::filesystem::path& sPkgDataDir
    , const std::string& sPkgName
);

/**
 * @brief
 */
template<typename Ch>
struct equal_char_ignorecase
{
    bool operator()(Ch x, Ch y) const
    {
        std::locale loc;
        return std::tolower(x,loc) == std::tolower(y,loc);
    }
};

/**
 * @brief
 *   Compare two strings with ignore case.
 *
 * @par Example way to be used:
 * @code
   std::string a = "ABC", b = "abc";
   if (ps::lib::stricmp(a, b))
   {
     std::cout << "Both a and b are same." << std::endl;
   }
   @endcode
 *
 * @param [in] x
 * @param [in] y
 * @return
 *    true: Both x and y are same except for case differences.
 */
template<typename Str>
bool stricmp(const Str& x, const Str& y)
{
  return x.size() == y.size()
      && std::equal(
          x.cbegin(), x.cend(), y.cbegin()
          , equal_char_ignorecase<typename Str::traits_type::char_type>()
      );
}

/**
 * @brief
 * @param [in] ptn
 *   containing wild-card characters like as '*' or '?'.
 * @param [in] str
 * - a target string to compare.
 * - Even if '*' or '?' are included, it is not recognized as a wild card.
 * @return
 *   returns true when matched.
 */
extern bool wild_strmatch(
    const std::basic_string<char>& ptn
    , const std::basic_string<char>& str
);

/**
 * @brief
 */
template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U> p)
{
    os << p.first << ':' << p.second;
    return os;
}

/**
 * @brief
 */
template<class Cont>
std::ostream& oTransForm(
    std::ostream& os
    , const Cont& cont
    , const cEnclosure& oEnclosure
){
    int32_t i = 0;
    os << oEnclosure.aGetOuter().front();
    for (const auto& item : cont)
    {
        os << boost::format(R"(%s%s%s%s)")
            % (i++ ? "," : "")
            % oEnclosure.aGetQuate().front()
            % item
            % oEnclosure.aGetQuate().back()
            ;
    }
    os << oEnclosure.aGetOuter().back();
    return os;
}

/**
 * @brief
 */
template <class T>
class cVector : private std::vector<T, std::allocator<T> >
{
    cEnclosure oEnclosure_;
public:
    using typename std::vector<T>::value_type;
    using typename std::vector<T>::const_iterator;
    using typename std::vector<T>::const_reference;
    using typename std::vector<T>::reference;
    using typename std::vector<T>::iterator;
    using typename std::vector<T>::size_type;
    using std::vector<T>::vector;
    using std::vector<T>::size;
    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    using std::vector<T>::push_back;
    using std::vector<T>::emplace_back;
    using std::vector<T>::back;
    using std::vector<T>::at;
    using std::vector<T>::insert;
    using std::vector<T>::erase;
    using std::vector<T>::empty;
    using std::vector<T>::assign;
    using std::vector<T>::operator[];
    friend std::ostream& operator<<(std::ostream& lhs, const cVector& rhs)
    {
        return oTransForm(lhs, rhs, rhs.oEnclosure_);
    }
    cVector& oChange(const cEnclosure& oEnclosure)
    {
        oEnclosure_ = oEnclosure;
        return *this;
    }
};

typedef cVector<std::string> str_vct;

/**
 * @brief
 */
template <class T>
class cList : private std::list<T, std::allocator<T> >
{
    cEnclosure oEnclosure_;
public:
    using typename std::list<T>::const_iterator;
    using typename std::list<T>::iterator;
    using typename std::list<T>::value_type;
    using std::list<T>::list;
    using std::list<T>::begin;
    using std::list<T>::end;
    using std::list<T>::cbegin;
    using std::list<T>::cend;
    using std::list<T>::size;
    using std::list<T>::push_back;
    using std::list<T>::sort;
    using std::list<T>::insert;
    using std::list<T>::erase;
    using std::list<T>::reverse;
    friend std::ostream& operator<<(std::ostream& lhs, const cList& rhs)
    {
        return oTransForm(lhs, rhs, rhs.oEnclosure_);
    }
    cList& oChange(const cEnclosure& oEnclosure)
    {
        oEnclosure_ = oEnclosure;
        return *this;
    }
    cList& operator-=(const cList& oMatched)
    {
        cList oTemp;
        this->swap(oTemp);
        std::set_difference(
            oTemp.cbegin(), oTemp.cend()
            , oMatched.begin(), oMatched.end()
            , std::back_inserter(*this)
        );
        return *this;
    }
};

/**
 * @brief
 */
template <class Key, class T
    , class Cmp = std::less<Key>
    , class A = std::allocator<
        std::pair<const Key,T>
    >
>
class cMap : private std::map<Key, T, Cmp, A>
{
    cEnclosure oEnclosure_;
public:
    using std::map<Key, T, Cmp, A>::map;
    using std::map<Key, T, Cmp, A>::at;
    using std::map<Key, T, Cmp, A>::size;
    using std::map<Key, T, Cmp, A>::begin;
    using std::map<Key, T, Cmp, A>::end;
    using std::map<Key, T, Cmp, A>::cbegin;
    using std::map<Key, T, Cmp, A>::cend;
    using std::map<Key, T, Cmp, A>::insert;
    using std::map<Key, T, Cmp, A>::find;
    using std::map<Key, T, Cmp, A>::erase;
    using std::map<Key, T, Cmp, A>::operator[];
    using std::map<Key, T, Cmp, A>::empty;
    using std::map<Key, T, Cmp, A>::emplace;
    using typename std::map<Key, T, Cmp, A>::key_type;
    using typename std::map<Key, T, Cmp, A>::value_type;
    using typename std::map<Key, T, Cmp, A>::size_type;
    using typename std::map<Key, T, Cmp, A>::const_iterator;
    using typename std::map<Key, T, Cmp, A>::iterator;
    friend std::ostream& operator<<(std::ostream& lhs, const cMap& rhs)
    {
        return oTransForm(lhs, rhs, rhs.oEnclosure_);
    }
    cMap& oChange(const cEnclosure& oEnclosure)
    {
        oEnclosure_ = oEnclosure;
        return *this;
    }
};

/**
 * @brief
 */
template <class Key
    , class Cmp = std::less<Key>
    , class A = std::allocator<Key>
>
class cSet : private std::set<Key, Cmp, A>
{
    cEnclosure oEnclosure_;
public:
    using typename std::set<Key, Cmp, A>::const_reference;
    using typename std::set<Key, Cmp, A>::reference;
    using std::set<Key, Cmp, A>::set;
    using std::set<Key, Cmp, A>::find;
    using std::set<Key, Cmp, A>::begin;
    using std::set<Key, Cmp, A>::end;
    using std::set<Key, Cmp, A>::cbegin;
    using std::set<Key, Cmp, A>::cend;
    using std::set<Key, Cmp, A>::insert;
    using std::set<Key, Cmp, A>::size;
    using std::set<Key, Cmp, A>::erase;
    using std::set<Key, Cmp, A>::clear;
    using typename std::set<Key, Cmp, A>::key_type;
    friend std::ostream& operator<<(std::ostream& lhs, const cSet& rhs)
    {
        return oTransForm(lhs, rhs, rhs.oEnclosure_);
    }
    cSet& oChange(const cEnclosure& oEnclosure)
    {
        oEnclosure_ = oEnclosure;
        return *this;
    }
};

typedef cSet<std::string> str_set;

/**
 * @brief
 */
bool composite_less(const char* lhs, const char* rhs);

/**
 * @brief
 */
template<typename T>
bool composite_less(const T& lhs, const T& rhs)
{
    if (lhs < rhs) return true;
    if (lhs > rhs) return false;
    return false;
}

/**
 * @brief
 */
template<typename T, typename... Args>
bool composite_less(const T& lhs, const T& rhs, Args&... args)
{
    if (lhs < rhs) return true;
    if (lhs > rhs) return false;
    return composite_less(args...);
}

/**
 * @brief
 */
template<typename... Args>
bool composite_less(const char* lhs, const char* rhs, Args&... args)
{
    const auto i = ::strcmp(lhs, rhs);
    if (i) return i < 0;
    return composite_less(args...);
}

/**
 * @brief
 */
template<typename T>
bool composite_equal(const T& lhs, const T& rhs)
{
    return lhs == rhs;
}

/**
 * @brief
 */
template<typename T, typename... Args>
bool composite_equal(const T& lhs, const T& rhs, Args&... args)
{
    if (lhs != rhs) return false;
    return composite_equal(args...);
}

/**
 * @brief
 */
bool composite_equal(const char* lhs, const char* rhs);

/**
 * @brief
 */
template<typename... Args>
bool composite_equal(const char* lhs, const char* rhs, Args&... args)
{
    const auto i = ::strcmp(lhs, rhs);
    if (i != 0) return false;
    return composite_equal(args...);
}

/**
 * @brief
 */
bool iMatchedBackward(const char* a, const char* b);

/**
 * @brief
 */
std::string sGetSqlInList(const str_vct& vct);

/**
 * @brief
 * @param[in] name
 * - Pass a file name to open.
 * @param[in] mode
 * - Refer to <a href="http://www.boost.org/doc/libs/1_51_0/boost/filesystem/fstream.hpp"
 *   >boost/filesystem/fstream.hpp</a>.
 * @return
 * - A smart pointer will be given.
 * - Hand over the ownership to the caller.
 * @exception std::runtime_error
 */
tPtrFstream open_with_check(
    const boost::filesystem::path& name
    , const boost::filesystem::fstream::ios_base::openmode mode
);

/**
 * @class cSpinLock
 * @brief If there are many competing threads and the period
 *   of the critical section is extremely short, <br/>
 *   the effect of suppressing CPU consumption can be expected
 *   as compared with <a href="http://en.cppreference.com/w/cpp/thread/unique_lock">std::unique_lock</a>
 *   or <a href="http://en.cppreference.com/w/cpp/thread/lock_guard">std::lock_guard</a>.
 * @param Rep
 *   An arithmetic type representing the number of ticks.
 * @param Period
 *   A <a href="http://ja.cppreference.com/w/cpp/numeric/ratio/ratio">std::ratio</a>
 *   representing the tick period (i.e. the number of seconds per tick).
 * @par Example way to be used:
 * @code
    typedef ps::lib::cSpinLock<int64_t, std::micro> spinlock_t;
    spinlock_t spin_(std::chrono::microseconds(300));
    {
        std::lock_guard<spinlock_t> lk(spin_);
        ....
    }
   @endcode
 * @note However, it is not all-purpose.
 */
template<class Rep, class Period>
class cSpinLock
{
private:
    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;
    /**
     * See <a href="http://ja.cppreference.com/w/cpp/chrono/duration">std::chrono::duration</a>
     */
    std::chrono::duration<Rep, Period> dura_;
    /**
     * A uniformly-distributed integer random number generator that produces non-deterministic random numbers. <br/>
     * See also <a href="http://en.cppreference.com/w/cpp/numeric/random/random_device">
     * std::random_device</a>
     */
    std::random_device rnd_;
    /** A random number engine based on Linear congruential generator (LCG) <br/>
     * <a href="http://en.cppreference.com/w/cpp/numeric/random/linear_congruential_engine">
     * std::linear_congruential_engine</a>
     */
    std::minstd_rand gen_;
    std::function<int()> next_delta; ///< A flamework of the functor, For calculating the next delay time.
public:
    /**
     * @brief
     */
    cSpinLock(std::chrono::duration<Rep, Period> dura)
        : dura_(dura), gen_(rnd_())
        // Make a random time wait to reduce the probability of collision
        , next_delta(std::bind(std::normal_distribution<>(0.0, 50.0), gen_))
    {}
    /**
     * @brief
     */
    void lock()
    {
        while (locked_.test_and_set(std::memory_order_acquire))
        {
            // To prevent busy weight when the multiplicity is high
            // and being used for heavy critical secition.
            std::this_thread::sleep_for(dura_ + std::chrono::microseconds(next_delta()));
        }
    }
    /**
     * @brief
     */
    void unlock()
    {
        locked_.clear(std::memory_order_release);
    }
};

/**
 * @brief
 */
template<class T>
std::string sGetKeyListOfMap(const T& oMap, const char (&sEncls)[2])
{
    BOOST_ASSERT(sEncls[0] >= 0x20 && sEncls[0] <= 0x7f);
    BOOST_ASSERT(sEncls[1] >= 0x20 && sEncls[1] <= 0x7f);
    std::string ret;
    for (typename T::const_iterator it = oMap.cbegin(); it != oMap.cend(); ++it)
    {
        if (it != oMap.cbegin()) ret += ',';
        ret += sEncls[0] + it->first + sEncls[1];
    }
    return ret;
}

/**
 * @brief
 * Binary terminal comparator.
 * Calculate the last pair of operands.
 */
bool iCompBinary(
    int (*iBinary)(const char*, const char*)
    , const char* lhs, const char* rhs
);

/**
 * @brief
 * In order for the compiler to detect conditions
 * for terminating recursive function calls.
 * (This function itself is not called from anywhere)
 */
template<typename... Args>
bool iCascadeBinaryAnd(
    int (*iBinary)(const char*, const char*)
    , const char* lhs
){
    return true;
}

/**
 * @brief
 * - Binary cascadable foward comparator. To obtain logical AND.
 * @param [in] iBinary
 *   - String comparator without length restriction.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] lhs
 *   - Left operand given to the string comparator.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] rhs
 *   - Right operand given to the string comparator.
 * @param [in] args
 *   - If there are more than four arguments,
 *     the fifth and subsequent ones is taken over.
 * @return
 *   true: The left side is the same as the right side.
 * @par Example way to be used:
 * @code
    // It can be written concisely.
    using ps::lib::iCascadeBinaryAnd;
    if (iCascadeBinaryAnd(strcmp, szDataType, "NCHAR", "NVARCHAR2", "RAW"))
    {
        // What you want to execute if true.
    }
    // Instead of writing such expressions.
    if (
        strcmp(szDataType, "NCHAR") == 0
        && strcmp(szDataType, "NVARCHAR2") == 0
        && strcmp(szDataType, "RAW") == 0
    )
    {
        // What you want to execute if true.
    }
   @endcode
 */
template<typename... Args>
bool iCascadeBinaryAnd(
    int (*iBinary)(const char*, const char*)
    , const char* lhs, const char* rhs
    , Args&... args
){
    return iCompBinary(iBinary, lhs, rhs)
        // Do not pass "rhs" only
        && iCascadeBinaryAnd(iBinary, lhs, args...);
}

/**
 * @brief
 * In order for the compiler to detect conditions
 * for terminating recursive function calls.
 * (This function itself is not called from anywhere)
 */
template<typename... Args>
bool iCascadeBinaryOr(
    int (*iBinary)(const char*, const char*)
    , const char* lhs
){
    return false;
}

/**
 * @brief
 * - Binary cascadable foward comparator. To obtain logical OR.
 * @param [in] iBinary
 *   - String comparator without length restriction.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] lhs
 *   - Left operand given to the string comparator.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] rhs
 *   - Right operand given to the string comparator.
 * @param [in] args
 *   - If there are more than four arguments,
 *     the fifth and subsequent ones is taken over.
 * @return
 *   true: The left side is the same as the right side.
 * @par Example way to be used:
 * @code
    // It can be written concisely.
    using ps::lib::iCascadeBinaryOr;
    if (iCascadeBinaryOr(strcmp, szDataType, "NCHAR", "NVARCHAR2", "RAW"))
    {
        // What you want to execute if true.
    }
    // Instead of writing such expressions.
    if (
        strcmp(szDataType, "NCHAR") == 0
        || strcmp(szDataType, "NVARCHAR2") == 0
        || strcmp(szDataType, "RAW") == 0
    )
    {
        // What you want to execute if true.
    }
   @endcode
 */
template<typename... Args>
bool iCascadeBinaryOr(
    int (*iBinary)(const char*, const char*)
    , const char* lhs, const char* rhs
    , Args&... args
){
    return iCompBinary(iBinary, lhs, rhs)
        // Do not pass "rhs" only
        || iCascadeBinaryOr(iBinary, lhs, args...);
}

/**
 * @brief
 * Ternary terminal comparator.
 * Calculate the last pair of operands.
 */
bool iCompTernary(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs, const char* rhs, size_t len
);

/**
 * @brief
 * In order for the compiler to detect conditions
 * for terminating recursive function calls.
 * (This function itself is not called from anywhere)
 */
template<typename... Args>
bool iCascadeTernaryAnd(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs
){
    return true;
}

/**
 * @brief
 * - Ternary cascadable foward comparator. To obtain logical AND.
 * - Cut the comparison with the shorter length of the two operands.
 * @param [in] iTernary
 *   - String comparator with length restriction.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] lhs
 *   - Left operand given to the string comparator.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] rhs
 *   - Right operand given to the string comparator.
 * @param [in] args
 *   - If there are more than four arguments,
 *     the fifth and subsequent ones is taken over.
 * @return
 *   true: The left side is the same as the right side.
 * @par Example way to be used:
 * @code
    // It can be written concisely.
    using ps::lib::iCascadeTernaryAnd;
    if (iCascadeTernaryAnd(strncmp, szDataType, "NCHAR", "NVARCHAR2", "RAW"))
    {
        // What you want to execute if true.
    }
    // Instead of writing such expressions.
    if (
        strncmp(szDataType, "NCHAR", strlen("NCHAR")) == 0
        && strncmp(szDataType, "NVARCHAR2", strlen("NVARCHAR2")) == 0
        && strncmp(szDataType, "RAW", strlen("RAW")) == 0
    )
    {
        // What you want to execute if true.
    }
   @endcode
 */
template<typename... Args>
bool iCascadeTernaryAnd(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs, const char* rhs
    , Args&... args
){
    const auto len_lhs = ::strlen(lhs);
    const auto len_rhs = ::strlen(rhs);
    return iCompTernary(iTernary, lhs, rhs, len_lhs < len_rhs ? len_lhs : len_rhs)
        // Do not pass "rhs" only
        && iCascadeTernaryAnd(iTernary, lhs, args...);
}

/**
 * @brief
 * In order for the compiler to detect conditions
 * for terminating recursive function calls.
 * (This function itself is not called from anywhere)
 */
template<typename... Args>
bool iCascadeTernaryOr(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs
){
    return false;
}

/**
 * @brief
 * - Ternary cascadable foward comparator. To obtain logical OR.
 * - Cut the comparison with the shorter length of the two operands.
 * @param [in] iTernary
 *   - String comparator with length restriction.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] lhs
 *   - Left operand given to the string comparator.
 *   - If there are more than four arguments, this is taken over by the recursive call.
 * @param [in] rhs
 *   - Right operand given to the string comparator.
 * @param [in] args
 *   - If there are more than four arguments,
 *     the fifth and subsequent ones is taken over.
 * @return
 *   true: The left side is the same as the right side.
 * @par Example way to be used:
 * @code
    // It can be written concisely.
    using ps::lib::iCascadeTernaryOr;
    if (iCascadeTernaryOr(strncmp, szDataType, "NCHAR", "NVARCHAR2", "RAW"))
    {
        // What you want to execute if true.
    }
    // Instead of writing such expressions.
    if (
        strncmp(szDataType, "NCHAR", strlen("NCHAR")) == 0
        || strncmp(szDataType, "NVARCHAR2", strlen("NVARCHAR2")) == 0
        || strncmp(szDataType, "RAW", strlen("RAW")) == 0
    )
    {
        // What you want to execute if true.
    }
   @endcode
 */
template<typename... Args>
bool iCascadeTernaryOr(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs, const char* rhs
    , Args&... args
){
    const auto len_lhs = ::strlen(lhs);
    const auto len_rhs = ::strlen(rhs);
    return iCompTernary(iTernary, lhs, rhs, len_lhs < len_rhs ? len_lhs : len_rhs)
        // Do not pass "rhs" only
        || iCascadeTernaryOr(iTernary, lhs, args...);
}

/**
 * @brief
 *  convert from boost::any to std::string.
 */
extern std::string sHumanReadable(const boost::any& val);

/**
 * @brief
 */
extern const char *szBuildMessage(
    const ps::lib::tMsgKey iMsgKey
    , const boost::filesystem::path& fn
    ,  const std::string& msg
);

} // ps::lib

} // ps
