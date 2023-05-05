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

namespace sql
{

namespace occi
{

/**
 * @class cDefine
 * @brief
 * - This can bind the column value of SQL-Select to the memory
 *   that is allocated by client program.
 * - When the rows of the table is fetched,
 *   column values are retrieved and copied into the memory.
 * - Supported the technique that enhance the apps performance
 *   like as bulk-fetch or the structured array memory.
 * - For fetching LOBs, LONG and LONG RAW that store
 *   unstructured data with large amounts of data,
 *   such as documents and multimedia, you can minimize
 *   memory usage using piecewise fetching.
 *
 * @par Example way to be used:
 * @code
   @endcode
 */
class cDefine
{
public:
    /**
     * @enum tTiming
     * - It is used to regulate the timing and number of times
     *   cDefine::vAttachTo is called by cStmt::iFetch.
     * - These constants indicate a policy whether to prepare a new memory
     *   for each fetch or to repeatedly use the prepared memory.
     */
    enum tTiming
    { iOnce   ///< cDefine::vAttachTo is called for the number of fetches while fetching.
    , iRepeat ///< cDefine::vAttachTo is called only once before starting fetching.
    };
      // Specifies the timing of presenting the address of the buffer.
private:
    /**
     * @class tDefine
     */
    class tDefine
    {
        friend class cDefine;
        uint32_t pos_; // 1 origin.
        void *valuep_;
        int32_t value_sz_;
        uint16_t dty_;
        ps::lib::sql::ind_t *indp_;
        uint16_t *rlenp_;
        uint16_t *rcodep_;
        uint32_t pvskip_;
        uint32_t indskip_;
        uint32_t rlskip_;
        uint32_t rcskip_;
        uint32_t mode_;
        OCICallbackDefine func_;
        void* ctxp_;
        uint8_t csfrm_;
        uint16_t csid_;
        uint16_t pdprc_;
        int16_t pdscl_;
    public:
        tDefine(
            uint32_t pos
            , void *valuep
            , int32_t value_sz
            , uint16_t dty
            , ps::lib::sql::ind_t *indp
            , uint16_t *rlenp
            , uint16_t *rcodep
            , uint32_t pvskip
            , uint32_t indskip
            , uint32_t rlskip
            , uint32_t rcskip
            , uint32_t mode
            , OCICallbackDefine func
            , void* ctxp
            , uint8_t csfrm
            , uint16_t csid
            , uint16_t pdprc
            , int16_t pdscl
        );
        tDefine(
            uint32_t pos
            , void *valuep
            , int32_t value_sz
            , uint16_t dty
            , ps::lib::sql::ind_t *indp
            , uint16_t *rlenp
            , uint16_t *rcodep
            , uint32_t pvskip
            , uint32_t indskip
            , uint32_t rlskip
            , uint32_t rcskip
        );
        tDefine(const tDefine& rhs);
        tDefine& operator=(const tDefine& rhs);
        tDefine& oAssign(
            void *valuep
            , int32_t value_sz
            , ps::lib::sql::ind_t *indp
            , uint16_t *rlenp
            , uint16_t *rcodep
        );
    };
    /**
     * @brief
     */
    ps::lib::cVector<tDefine> oMap_;
    /**
     * @brief
     * -# cDefine::iOnce:   to prepare a new memory for each fetch.
     * -# cDefine::iRepeat: to repeatedly use the prepared memory.
     */
    tTiming iTiming_;

public:
    /**
     * @brief
     * @param [in] iTiming
     * Select one of the policies below.
     * -# cDefine::iOnce:   to prepare a new memory for each fetch.
     * -# cDefine::iRepeat: to repeatedly use the prepared memory.
     */
    explicit cDefine(const tTiming& iTiming);
    /**
     * @brief
     * duplicates object.
     */
    cDefine(const cDefine& rhs);
    /**
     * @brief
     * duplicates object.
     */
    cDefine& operator=(const cDefine& rhs);
    /**
     * @brief
     * Set memory allocation policy.
     * Call from cDefine's client code.
     */
    void vSetTiming(const tTiming& iTiming);
    /**
     * @brief
     * This is called by cStmt::iFetch, not from client code.
     */
    const cDefine::tTiming& iGetTiming() const;
    /**
     * @brief
     * - Call this as many times as the number
     *   of columns in the list of select statements.
     * - The Oracle data type of each column and the type of each
     *   member of the user defined structure must be compatible.
     * - Once the passed arguments are saved in the map,
     *   cStmt::iFetch is executed and when fetch begins,
     *   cDefine::vAttachTo is called implicitly and is
     *   associated with the SQL-Select statement.
     *
     * @param [in,out] valuep
     *   A pointer to an array of buffers of the type
     *   specified in the dty parameter.
     * @param [in] value_sz
     *   The size of each valuep buffer in bytes.
     *   If the data is stored internally in VARCHAR2 format,
     *   the number of characters desired.
     * @param [in] dty
     *   The data type. See also [External Data Types](https://docs.oracle.com/database/121/LNOCI/oci03typ.htm#LNOCI16271)
     *   for a listing of data type codes and values
     * @param [in] indp
     *   Pointer to an indicator variable or array.
     *   For scalar data types, pointer to an array of sb2s.
     * @param [in,out] rlenp
     *   Pointer to array of length of data fetched.
     * @param [out] rcodep
     *   Pointer to array of column-level return codes.
     * @param [in] pvskip
     *   Skip parameter for the next data value.
     * @param [in] indskip
     *   Skip parameter for the next indicator location.
     * @param [in] rlskip
     *   Skip parameter for the next return length value.
     * @param [in] rcskip
     *   Skip parameter for the next return code.
     * @param [in] mode
     *   The valid modes are:
     *   - OCI_DEFAULT - This is the default mode.
     *   - OCI_DYNAMIC_FETCH - For applications requiring dynamically
     *     allocated data at the time of fetch, this mode must be used.<br />
     *     You can define a callback using the [OCIDefineDynamic()](https://docs.oracle.com/database/121/LNOCI/oci16rel003.htm#GUID-DD404526-9615-4E44-B790-1D17396C82A9) call.<br />
     *     The value_sz parameter defines the maximum size of the data
     *     that is to be provided at run time.
     * @param [in] func
     * - Points to a callback function.
     * - This is invoked at run time to get a pointer to the buffer
     *   into which the fetched data or a piece of it is to be retrieved.
     * - The callback also specifies the indicator, the return code,
     *   and the lengths of the data piece and indicator.
     * @param [in] ctxp
     * - A context pointer passed as an argument to all the callback functions.
     *   When the client library needs a buffer to return the fetched data,
     *   the callback is invoked and the runtime buffers provided return a piece
     *   or all of the data.
     * @param [in] csfrm
     * - If not 0 is passed, OCIAttrSet will execute with this and OCI_ATTR_CHARSET_FORM.
     *   to handle the national character set.
     * - See also [Define Handle Attributes](https://docs.oracle.com/database/121/LNOCI/ociaahan.htm#LNOCI17888) .
     * @param [in] csid
     *   If not 0 is passed, OCIAttrSet will execute with this and OCI_ATTR_CHARSET_ID.
     * @param [in] pdprc
     *   If not 0 is passed, OCIAttrSet will execute with this and OCI_ATTR_PDPRC.
     * @param [in] pdscl
     *   If not 0 is passed, OCIAttrSet will execute with this and OCI_ATTR_PDSCL.
     *
     * @details
     * - The value given to the argument is mainly handed over
     *   to the call of
     *   [OCIDefineByPos](https://docs.oracle.com/database/121/LNOCI/oci16rel003.htm#GUID-CFE5AA54-DEBC-42D3-8A27-AFF1E7815691)
     *   and [CIDefineArrayOfStruct](https://docs.oracle.com/database/121/LNOCI/oci16rel003.htm#GUID-D3F74828-5DA7-48AD-A170-120AD547DEB3)
     */
    void vAddItem(
        void *valuep
        , int32_t value_sz
        , uint16_t dty
        , ps::lib::sql::ind_t *indp
        , uint16_t *rlenp
        , uint16_t *rcodep
        , uint32_t pvskip
        , uint32_t indskip
        , uint32_t rlskip
        , uint32_t rcskip
        , uint32_t mode
        , OCICallbackDefine func
        , void* ctxp
        , uint8_t csfrm
        , uint16_t csid
        , uint16_t pdprc
        , int16_t pdscl
    );
    /**
     * @brief
     */
    template<typename T>
    void vAddItem(
        T& value
        , uint16_t dty
        , ps::lib::sql::ind_t *indp
        , uint16_t *rlenp
        , uint16_t *rcodep
        , uint32_t skip
    ){
        vAddItem(
            &value, sizeof(T), dty, indp, rlenp, rcodep, skip, skip, skip, skip
            , OCI_DEFAULT, (OCICallbackDefine) 0, (void*) 0, 0, 0, 0, 0
        );
    }
    /**
     * @brief
     *   For LONG or LONG RAW column.
     *
     * @note
     *   Before calling cDefine::vChangeAddr first,
     *   cDefine::vSetTiming ( cDefine::iOnce )
     *   must be executed only once to switch the memory supply policy.
     */
    void vAddItem(
        int32_t value_sz
        , uint16_t dty
        , uint32_t mode
        , OCICallbackDefine func
        , void* ctxp
    );
    /**
     * @brief
     */
    void vAddItem(
        uint16_t dty
        , uint32_t pvskip
    );
    /**
     * @brief
     * - Change the address of the member of the first element of the array
     *   to be assigned to the i th (i = 1, 2, ..., n) column.
     * - cFetchable::vPreBulkAction
     * - Call this with the vPreBulkAction in the code of each user-defined class
     *   inheriting ps::lib::sql::cFetchable.
     * - Since cDefine::vAttachTo is called back immediately after
     *   cFetchable::vPreBulkAction returns, indirectly it can notify
     *   the new memory address to return the data.
     *
     * @param [in] i        Originated 1.
     * @param [in] value    body of the menber.
     * @param [in] indp     pointer to an indicator variable or array.
     * @param [in] rlenp    Pointer to array of length of data fetched.
     * @param [in] rcodep   Pointer to array of column-level return codes.
     *
     * @note
     *   Before calling cDefine::vChangeAddr first,
     *   cDefine::vSetTiming ( cDefine::iOnce )
     *   must be executed only once to switch the memory supply policy.
     */
    template<typename T>
    void vChangeAddr(
        const int32_t i
        , T& value
        , ps::lib::sql::ind_t *indp
        , uint16_t *rlenp
        , uint16_t *rcodep
    ){
        BOOST_ASSERT(iTiming_ == tTiming::iOnce);
        oMap_.at(i - 1).oAssign(
            &value, sizeof(T), indp, rlenp, rcodep
        );
    }
    /**
     * @brief
     * - Sets the address of the structured memory to receive
     *   the value of each column returned from the fetched statement.
     * - This is called by cStmt::iFetch.
     * - Time and number of calling with cDefine::iTiming_ value
     *   are regulated. @ref cDefine::tTiming
     *
     * @details
     * - Internally calling ps::lib::sql::occi::vDefineArrayOfStruct
     *   to delegate the actual operation.
     */
    void vAttachTo(cOciStmt& oOciStmt);
    /**
     * @brief
     * @return Return the number of registered columns.
     */
    size_t size() const
    {
        return oMap_.size();
    }
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps


