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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @brief
 *   In order to prevent the soaring of CPU due to excessive creation
 *   and destruction of objects, use statically prepared std::ostringstream.
 * @note
 *   After outputting the contents to the target stream,
 *   do not forget to recall the contents by calling str("")
 *   to prevent contamination.
 */
static std::ostringstream oss_;

/**
 * @brief
 *
 * @return a string which is a string formed as a result
 *   of sOnRepeat, sPreRepeat, sPostRepeat and sNotFound operation.
 *
 */
template
<
    class C
    , class T
    , class tIter
    , class tCopare
    , class tOnRep
    , class tBgnRep
    , class tEndRep
    , class tNotFnd
>
std::string sConcatMatched(
    C& oCtx
    , const T&& rKey
    , const std::pair<tIter, tIter>& oPair
    , tCopare iKeyMatched
    , tOnRep sOnRepeat
    , tBgnRep sPreRepeat
    , tEndRep sPostRepeat
    , tNotFnd sNotFound
    , std::back_insert_iterator<ps::lib::cList<tIter> > oArr
){
    tIter it = oPair.first, ite = oPair.second;
    size_t iMatched = 0;
    oss_.str("");
    while (it != ite)
    {
        if (iKeyMatched(*it, rKey))
        {
            if (0 == iMatched)
            {
                // First matched element.
                oss_ << sPreRepeat(oCtx);
            }
            oss_ << sOnRepeat(*it, iMatched, oCtx);
            // std::back_insert_iterator::operator=()
            // calls push_back() of argument class.
            oArr = it;
            ++iMatched;
        }
         ++it;
    }
    if (0 < iMatched)
    {
        // There was more than one that matched rKey.
        oss_ << sPostRepeat(oCtx);
    }
    else
    {
        // There is nothing that matches rKey.
        oss_ << sNotFound(oCtx);
    }
    return oss_.str();
}

/**
 * @brief
 * -# erase item matching rKey from oList.
 *
 * @param [in] rKey
 *   - A key for comparing whether or not they match.
 * @param[in,out]  oList
 *   - A collection of T.
 * @param[in]      iKeyMatched
 *   - A callback function for comparing key items.
 * @return
 *   - Number of eraseed items.
 */
template
<
    class T
    , class Container = ps::lib::cList<T>
    , class tCopare
>
size_t iEraseMatchedKeyItem(
    const T&& rKey
    , Container& oList
    , tCopare iKeyMatched
){
    typedef typename Container::iterator tIter;
    // oArr stores position of items that is matched for rKey.
    ps::lib::cList<tIter> oArr;
    tIter it, ite = oList.end();
    for (it = oList.begin(); it != ite; ++it)
    {
        if (iKeyMatched && (*it.*iKeyMatched)(rKey))
        {
            oArr.push_back(it);
        }
    }
    const size_t iResult = oArr.size();
    if (0 < iResult)
    {
        boost::for_each(
            oArr, [&oList](tIter it){oList.erase(it);}
        );
    }
    return iResult;
}

/**
 * @brief
 * -# To select the target item from the list (Container),
 *    call back the user-defined comparator (tCopare).
 * -# To manipulate on one or more selected items,
 *    call back the user-defined manipulator (tOnRep).
 * -# It also calls back user-defined manipulators corresponding
 *    to the three situations
 *   -# before starting the operation on the first item (tBgnRep),
 *   -# after finishing the operation on the last item (tEndRep),
 *   -# and when any items were not found (tNotFnd).
 *
 * @details
 * -# This pulls out elements matching the rKey from the oList
 *   and passes it vOnRepeat that is defined in the item class.
 * -# If true is passed to iDispose, the item that is matched
 *    with rKey remove from the oList.
 *
 * @tparam C
 * - has an instance of Container.
 * - C maust have following member functions.
 *   -# vPreRepeat   Execute before operation if there is a match
 *   -# vPostRepeat  Execute after operation if there is a match
 *   -# vNotFound  Even if there is no matching match
 * - C is client class of this function.
 * - vPrintMatched is indirectly invoked from C::vPrintAllItems.
 * - implements the concrete algorithm of vPrintAllItems.
 *   An instance of the C is passed and used as context.
 * @tparam T
 * - is element type of sequence.
 * - T maust have following member functions,
 *   in order to be able to call back from this function.
 *   -# iKeyMatched
 *   -# vOnRepeat
 * - T is a key used to select the target item
 *   from the argument instance of Container.
 * @tparam tIter
 * - An Interface to pass the sequence of T to the algorithm.
 *
 * @param[in,out]  os
 *     A sink of processed data.
 * @param[in]      oCtx
 * - Specific operation for C are delegated to following three functions.
 *   -# vOnRepeat
 *   -# vPreRepeat
 *   -# vPostRepeat
 *   -# vNotFound
 * @param[in]      rKey
 * - rKey is used to select objects to be processed from oList.
 *   Selection criteria are implemented in iKeyMatched.
 * @param[in]      iKeyMatched
 * - Called back in oList to find items matching rKey.
 * - Pass a pointer to a member function of class T.
 *   If nullptr is passed, do not do anything.
 * - As a matter of course, if a valid pointer to vNotFound has been passed,
 *   it will only be called back once.
 * @param[in]      vOnRepeat
 * - Called back every time an item matching rKey in oList is found.
 * - Pass a pointer to a member function of class T.
 *   If nullptr is passed, do not do anything.
 * @param[in]      vPreRepeat
 * - Called back when you first find an item that matches rKey in oList.
 * - Pass a pointer to a member function of class C.
 *   If nullptr is passed, do not do anything.
 * @param[in]      vPostRepeat
 * - If oList has more than one item matching rKey
 *   , it will only be called back once.
 * - Pass a pointer to a member function of class C.
 *   If nullptr is passed, do not do anything.
 * @param[in]      vNotFound
 * - Called back only once if oList does not find anything matching rKey.
 * - Pass a pointer to a member function of class C.
 *   If nullptr is passed, do not do anything.
 * @param[in,out]  oArr
 * - uses to be hooked position of items that is matched for rKey.
 *
 */
template
<
    class C
    , class T
    , class tIter
    , class tCopare
    , class tOnRep
    , class tBgnRep
    , class tEndRep
    , class tNotFnd
>
void vPrintMatched(
    std::ostream& os
    , C& oCtx
    , const T&& rKey
    , const std::pair<tIter, tIter>& oPair
    , tCopare iKeyMatched
    , tOnRep vOnRepeat
    , tBgnRep vPreRepeat
    , tEndRep vPostRepeat
    , tNotFnd vNotFound
    , std::back_insert_iterator<ps::lib::cList<tIter> > oArr
){
    tIter it = oPair.first, ite = oPair.second;
    size_t iMatched = 0;
    while (it != ite)
    {
        if (iKeyMatched && (*it.*iKeyMatched)(rKey))
        {
            if (0 == iMatched)
            {
                // First matched element.
                if (vPreRepeat)
                {
                    (oCtx.*vPreRepeat)(os, rKey);
                }
            }
            if (vOnRepeat)
            {
                (*it.*vOnRepeat)(os, iMatched, oCtx);
            }
            // std::back_insert_iterator::operator=()
            // calls push_back() of argument class.
            oArr = it;
            ++iMatched;
        }
        ++it;
    }
    if (0 < iMatched)
    {
        // There was more than one that matched rKey.
        if (vPostRepeat)
        {
            (oCtx.*vPostRepeat)(os, rKey);
        }
    }
    else
    {
        // There is nothing that matches rKey.
        if (vNotFound)
        {
            (oCtx.*vNotFound)(os, rKey);
        }
    }
}

/**
 * @class cProxyPrinter
 *
 * @tparam C
 * - In C, a member function called vPrintAllItems is defined.
 * - It is intended to serialize the information of the instance of C to the stream.
 * - vPrintAllItems takes at least the following three types as arguments.
 *   -# std::ostream
 *   -# class K
 *   -# bool
 * - In addition to the above, up to three other arguments may be taken.
 * - A special template argument "class... Args" allows variations of functions
 *   with different number of arguments.
 * @tparam K
 * - K is key information for identifying the item of container type object C has.
 */
template<class C, class K, class... Args>
class cProxyPrinter
{
    friend std::ostream& operator<<(std::ostream& os, cProxyPrinter oMe)
    {
        oMe.vInvokeCbk(os, oMe.args_);
        return os;
    }
private:
    C& oCtx_;
    const K& oKey_;
    /// If true, items serialized are deleted from C container.
    const bool& iDispose_;
    const std::tuple<Args...> args_;
    void vInvokeCbk(std::ostream& os, const std::tuple</* no argument requires */>&)
    {
        oCtx_.vPrintAllItems(os, oKey_, iDispose_);
    }
    template<class T0>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0>& args)
    {
        oCtx_.vPrintAllItems(os, oKey_, iDispose_
            , std::get<0>(args));
    }
    template<class T0, class T1>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0, T1>& args)
    {
        oCtx_.vPrintAllItems(os, oKey_, iDispose_
            , std::get<0>(args), std::get<1>(args));
    }
    template<class T0, class T1, class T2>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0, T1, T2>& args)
    {
        oCtx_.vPrintAllItems(os, oKey_, iDispose_
            , std::get<0>(args), std::get<1>(args), std::get<2>(args));
    }
public:
    cProxyPrinter(
        C& oCtx
        , const K& oKey
        , const bool& iDispose
        , const Args&... args
    )
        : oCtx_(oCtx), oKey_(oKey), iDispose_(iDispose), args_(args...)
    {}
};

/**
 * @brief
 * - A function that simply describes the code intended to output C information
 *   to the stream using the operator<<() declared as a friend in cProxyPrinter.
 * - In order to delegate concrete operations of information output,
 *   it is necessary for C to define vPrintAllItems which is a member function.
 *
 * @param[in,out] oCtx
 *   Context to hand over to client (Entity that called this function).
 * @param[in] oKey
 *   A key for identifying the individual elements of the collection that C has.
 * @param[in] iDispose
 *   true: Delete it when the print operation on the element of the collection is over.
 * @param[in] args
 *   It is a part of the arguments received by vPrintAllItems defined in C,
 *   variable number and supplemental. This is not the default.
 */
template<class C, class K, class... Args>
cProxyPrinter<C, K, Args...> oPrintItems(
    C& oCtx
    , const K& oKey
    , const bool& iDispose
    , const Args&... args
){
    return cProxyPrinter<C, K, Args...>(oCtx, oKey, iDispose, args...);
}

/**
 * @class cProxyPrinterFN
 *
 * @tparam C
 * - In C, one or more member functions having a signature of a specific pattern are defined.
 * - It is intended to serialize the information of the instance of C to the stream.
 * - these function take at least the following three types as arguments.
 *   -# std::ostream
 *   -# class K
 *   -# bool
 * - In addition to the above, up to three other arguments may be taken.
 * - A special template argument "class... Args" allows variations of functions
 *   with different number of arguments.
 * @tparam K
 * - K is key information for identifying the item of container type object C has.
 * @tparam F
 * - F is a pointer type pointing to a member function defined in C.
 *
 */
template<class C, class F, class K, class... Args>
class cProxyPrinterFN
{
    friend std::ostream& operator<<(std::ostream& os, cProxyPrinterFN oMe)
    {
        oMe.vInvokeCbk(os, oMe.args_);
        return os;
    }
private:
    C& oCtx_;
    F func_;   ///< A pointer to member function that defined in C.
    const K& oKey_;
    /// If true, items serialized are deleted from C container.
    const bool& iDispose_;
    const std::tuple<Args...> args_;
    void vInvokeCbk(std::ostream& os, const std::tuple</* no argument requires */>&)
    {
        (oCtx_.*func_)(os, oKey_, iDispose_);
    }
    template<class T0>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0>& args)
    {
        (oCtx_.*func_)(os, oKey_, iDispose_
            , std::get<0>(args));
    }
    template<class T0, class T1>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0, T1>& args)
    {
        (oCtx_.*func_)(os, oKey_, iDispose_
            , std::get<0>(args), std::get<1>(args));
    }
    template<class T0, class T1, class T2>
    void vInvokeCbk(std::ostream& os, const std::tuple<T0, T1, T2>& args)
    {
        (oCtx_.*func_)(os, oKey_, iDispose_
            , std::get<0>(args), std::get<1>(args), std::get<2>(args));
    }
public:
    cProxyPrinterFN(
        C& oCtx
        , F func
        , const K& oKey
        , const bool& iDispose
        , const Args&... args
    )
        : oCtx_(oCtx), func_(func), oKey_(oKey), iDispose_(iDispose), args_(args...)
    {}
};

/**
 * @brief
 * - cProxyPrinterFN is a more generalized interface of cProxyPrinter.
 *   Even if a function named vPrintAllItems is not defined in C,
 *   it can be substituted by another member function with the same signature.
 * - A function that simply describes the code intended to output C information
 *   to the stream using the operator<<() declared as a friend in cProxyPrinterFN.
 * - In order to delegate concrete operations of information output,
 *   it is necessary for C to define "*func" which is a member function.
 *
 * @param[in,out] oCtx
 *   Context to hand over to client (Entity that called this function).
 * @param[in] func
 *   A pointer to member function that defined in C.
 * @param[in] oKey
 *   A key for identifying the individual elements of the collection that C has.
 * @param[in] iDispose
 *   true: Delete it when the print operation on the element of the collection is over.
 * @param[in] args
 *   It is a part of the arguments received by "*func" defined in C,
 *   variable number and supplemental. This is not the default.
 */
template<class C, class F, class K, class... Args>
cProxyPrinterFN<C, F, K, Args...> oPrintItemsFN(
    C& oCtx
    , F func
    , const K& oKey
    , const bool& iDispose
    , const Args&... args
){
    return cProxyPrinterFN<C, F, K, Args...>(oCtx, func, oKey, iDispose, args...);
}

/**
 * @brief
 * - Comment out and output 4 digit decimal number filled with 0 on the left side.
 *
 * @param [in] iColumnId Index of column.
 * @return
 * - Column number commented out,
 *   when value is true parameter "printcolid" in the configure file.
 *   The number is formatted in four fixed digit.
 * - Empty string when value is false.
 */
std::string sColumnIdCooment(const uint32_t& iColumnId);

/**
 * @brief
 * - Output the sub partition definition in composite partitioning.
 *
 * @param [in] sSubpartitioningType
 *   One of "RANGE" or "LIST".
 * @param [in] iThreshOnRange
 *   When the number of characters of sExpression exceeds this integer,
 *   the text is displayed in folded form. If type is RANGE.
 * @param [in] iThreshOnList
 *   When the number of characters of sExpression exceeds this integer,
 *   the text is displayed in folded form. If type is LIST.
 * @param [in] iNumSpacesBfr
 *   Number of blank space characters used for indenting when folded back.
 * @param [in] iNumSpacesAft
 *   Number of blank space characters used for indenting when folded back.
 * @param [in] sExpression
 *   Expression that determines criteria for subpartitioning.
 *
 * @return
 *   A string that is containing subpartition clause.
 */
std::string sGetSubpertitionElement(
    const std::string& sSubpartitioningType
    , const size_t iThreshOnRange
    , const size_t iThreshOnList
    , const size_t iNumSpacesBfr
    , const size_t iNumSpacesAft
    , const std::string& sExpression
);

/**
 * @brief
 *   A function for outputting a string that labels the create statement.
 *
 * @param [in] sObjectType
 * @param [in] iOrder
 * @param [in] sObjectName
 *
 * @par Example way to be used:
   @code
       os << sPrintLabel("Tabs", 1234, "Object Name") ...;
   @endcode
 * @par The following string will be written to os.
   @code
       <comment start character string>
        * Tabs: 001234. Object Name
       <comment end character string>
   @endcode
 *
 */
std::string sPrintLabel(
    const std::string& sObjectType
    , const ub4& iOrder
    , const std::string& sObjectName
);

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
