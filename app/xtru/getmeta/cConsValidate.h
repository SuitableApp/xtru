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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cConsValidate
 * @brief
 * provides the subset data of ALL_CONSTRAINTS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the privileges exercisable.
 *   The information includes the following:
 *   - 
 *   - 
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cAllConsColumns oAllConsColumnsKey(true, ...);;
    ...
    ps::app::xtru::getmeta::cConsValidate oObj(oAllConsColumnsKey);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    oObj.oPrintItems(
        std::cout, std::make_tuple("SCOTT", "EMP", "PK_EMP")
        , true, true, cConsValidate::iConsInTable);
   @endcode
 */
class cConsValidate
{
public:
    /// @enum tConsType
    typedef enum
    { iC1024 ///< FK constraints
    , iC1025 ///< check constraints
    , iC1026 ///< PK or AK constraints
    , iC1027 ///< Not NULL constraints
    , iC1028 ///< triggers
    } tConsType
    ;
    /**
     * @copybrief cAllSequences::tKeyTuple
     *
     * @details
     * - first value is passed to tAttributes::szOwner.
     * - second value is passed to tAttributes::szKeyName.
     * - third value is passed to tAttributes::szConstraintName.
     *    (option:  only calling iSubtractCtxItemList)
     */
    typedef std::tuple<const char*, const char*, const char*> tKeyTuple;
private:
    /**
     * @struct tAttributes
     * @brief
     * The structure of the row data is defined.
     * Details are defined in cpp.
     */
    struct tAttributes;
    /**
     * @class cRetriever
     * Details are defined in cpp.
     */
    class cRetriever;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cConsValidate::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cConsValidate&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cConsValidate::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    typedef void (cConsValidate::tAttributes::*tPrintAny)(
        std::ostream&, const size_t&, cConsValidate&
    ) const;
    struct tCandidate
    {
        int iMask; ///< Using upper 5-bits. 1,2, ... 11 bit are reserved.
        const std::string sSelectStmt;
        std::string sCommentStr;  ///< 
        const std::string sProcTitle;
        const std::string sAltTableDisable;
        const std::string sAltTableEnable;
        tPrintAny vPrintForward;
        tPrintAny vPrintBackward;
    };
    typedef std::map<tConsType, tCandidate> tMap;
    struct cSingleton
    {
        cSingleton(const int iMngScrOut, tMap& oMap);
    };
    static tMap oMap;
    tMap::mapped_type oVal_;
public:
    /**
     * @brief
     */
    static void vSetMask(const int iMngScrOut);
    /**
     * @brief
     */
    cConsValidate(const tConsType& iConsType);
    virtual ~cConsValidate();
    /**
     * @brief
     * @param [in,out] oDb
     *     is connection to Sqlite.
     * @param [in] oOwners
     *     is concrete name of owners.
     */
    void vExecuteAndFetch(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , const ps::lib::str_vct& oOwners
    );
    /**
     * @copydoc cAllSequences::vPrintAllItems()
     * @param [in] iIotType
     * @param [in] iWayToNext
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const tWayToNext& iWayToNext
    );
    /**
     * @param [in] oKey
     */
    size_t iSubtractCtxItemList(
        const tKeyTuple& oKey
    );
    /**
     * @brief
     */
    void vReverse();
private:
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintNotFound()
    void vPrintNotFound(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
