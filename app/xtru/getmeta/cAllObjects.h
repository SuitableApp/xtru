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
 * @class cAllObjects
 * @brief
 * provides the subset data of ALL_OBJECTS.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the schema object.
 *   The information includes the following:
 *   - Name of the schema object.
 *   - Name of the object type.
 *   - Name of the sub-object type.
 * -# calls implicitly the vPrintAllItems which is declared on template argument 'C'
 *   using the member function @ref oPrintItems.
 * -# @ref oPrintItems calls vPrintAllItems by the quantity of the schema object
 *   of type cAllObjects::tAttributes::szObjectType which is owned by
 *   cAllObjects::tAttributes::szOwner.
 * -# The order of calling is ascending order of cAllObjects::tAttributes::szObjectName.
 *
 * @par Example way to be used:
 * @code
    cAnyClass oAny(...);  // cAnyClass has vPrintAllItems member as public.
       ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    ...
    cAllObjects oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    for (auto& sOwner: oOwners_)
    {
        std::cout << oObj.oPrintItems(
            oAny
            , std::make_tuple(sOwner.c_str(), "<type_of_schema_name>")
        );
    }
    // Implicitly cAnyClass::vPrintAllItems will be invoked and passed szOwner and szObjectName as argument.
    // <type_of_schema_name> : e.g. "TABLE", "SEQUENCE", "FUNCTION", "PACKAGE BODY", etc...
   @endcode
 *
 */
class cAllObjects
{
private:
    /**
     * @copybrief cAllSequences::tKeyTuple
     *
     * @details
     * - first value is passed to tAttributes::szOwner.
     * - second value is passed to tAttributes::szObjectType.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /*
     * @struct tAttributes
     */
    struct tAttributes
    {
        char szOwner[OBJECT_NAME_LEN];         ///< Name of the object owner. (e.g. "SCOTT")
        char szObjectType[OBJECT_TYPE_LEN];    ///< Name of the object type. (s.g. "TABLE","PROCEDURE",...)
        ps::lib::sql::ind_t nObjectTypeInd;    ///< ind_t::VAL_IS_NULL: value is NULL
        char szObjectName[OBJECT_NAME_LEN];    ///< Name of the schema object.
        char szSubObjectName[OBJECT_NAME_LEN]; ///< Name of the schema sub-object.
        ps::lib::sql::ind_t nSubObjectNameInd; ///< ind_t::VAL_IS_NULL: value is NULL
        /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
        tAttributes(const tKeyTuple& oKey);
        /// @copydoc cAllSequences::tAttributes::tAttributes()
        tAttributes();
        /// @copydoc cAllSequences::tAttributes::operator<()
        bool operator<(const tAttributes& rhs) const;
        /// @copydoc cAllSequences::tAttributes::iKeyMatched()
        bool iKeyMatched(const tAttributes& rhs) const;
        /**
         * @details
         * To assemble the SQL statement and drop into the stream (os) it,
         * delegate the operation to the T type object. Most T types
         * can be identified by owner and name only.
         */
        template<class C>
        void vPrinter(std::ostream& os, const size_t& i, const std::string& sLabel, C& oCtx) const
        {
            os << sPrintLabel(sLabel, i + 1, szObjectName)
               << oPrintItemsFN(oCtx, &C::vPrintAllItems
                    , std::make_tuple(szOwner, szObjectName), true);
        }
        /**
         * @details
         * In the cAllSource, it is not unique unless a type name is added to the key.
         * - example for "PACKAGE" or "PACKAGE BODY"
         * - Furthermore "TYPE" or "TYPE BODY"
         */
        void vPrinter(std::ostream& os, const size_t& i, const std::string& sLabel, cAllSource& oCtx) const
        {
            os << sPrintLabel(sLabel, i + 1, szObjectName)
               << oPrintItemsFN(oCtx, &cAllSource::vPrintAllItems
                    , std::make_tuple(szOwner, szObjectName, szObjectType), true);
        }
        /**
         * @copydetails cAllSequences::tAttributes::sDebugPrint()
         */
        std::string sDebugPrint() const;
    };
    /**
     * @class cRetriever
     * Details are defined in cpp.
     */
    class cRetriever;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_OBJECTS
    static const char szAllObjects[];
    std::map<std::string, size_t> oNumBojs_;
public:
    /**
     * @class cProxyPrinter
     * Details are defined in cpp.
     */
    template<class C>
    class cProxyPrinter
    {
        friend std::ostream& operator<<(std::ostream& os, cProxyPrinter oMe)
        {
            oMe.vInvokeCbk(os);
            return os;
        }
    private:
        C& oCtx_;
        const tKeyTuple& oKey_;
        const std::string& sLabel_;
        ps::lib::cList<tAttributes>& oList_;
        std::map<std::string, size_t>& oNumBojs_;
        void vInvokeCbk(std::ostream& os)
        {
            typedef ps::lib::cList<tAttributes>::iterator tIter;
            // oArr stores position of items that is matched for rKey.
            ps::lib::cList<tIter> oArr;
            tIter it, ite = oList_.end(), first = oList_.begin();
            size_t i = 0;
            for (it = first; it != ite; ++it)
            {
                if (it->iKeyMatched(oKey_))
                {
                    it->vPrinter(os, i++, sLabel_, oCtx_);
                    oArr.push_back(it);
                }
            }
            if (0 < oArr.size())
            {
                std::for_each(
                    oArr.begin(), oArr.end()
                    , [this](tIter it){oList_.erase(it);}
                );
            }
            oNumBojs_.insert({sLabel_, oArr.size()});
        }
    public:
        cProxyPrinter(
            C& oCtx
            , const tKeyTuple& oKey
            , const std::string& sLabel
            , ps::lib::cList<tAttributes>& oList
            , std::map<std::string, size_t>& oNumBojs
        )
            : oCtx_(oCtx)
            , oKey_(oKey)
            , sLabel_(sLabel)
            , oList_(oList)
            , oNumBojs_(oNumBojs)
        {}
    };
    /**
     * @details
     */
    cAllObjects();
    virtual ~cAllObjects();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
    );
    /**
     * @copydoc cAllSequences::iGetNumRows()
     */
    size_t iGetNumRows(const std::string& sLabel) const
    {
        return oNumBojs_.at(sLabel);
    }
    /*
     * @brief
     * @param [in] oCtx
     * @param [in] oKey
     */
    template<class C>
    cProxyPrinter<C> oPrintItems(
        C& oCtx
        , const tKeyTuple& oKey
        , const std::string& sLabel
    ){
        return cProxyPrinter<C>(oCtx, oKey, sLabel, oList_, oNumBojs_);
    }
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

