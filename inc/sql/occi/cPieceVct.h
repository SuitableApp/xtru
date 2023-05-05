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
 * @class cPieceVct
 * @brief
 * -# This class defines cPieceVct::iCbkFunc that is a type of OCICallbackDefine,
 *    used to be passed to
 *    [OCIDefineDynamic()](https://docs.oracle.com/database/121/LNOCI/oci16rel003.htm#GUID-DD404526-9615-4E44-B790-1D17396C82A9
 *    "OCIDefineDynamic").
 * -# By using this object, it is not necessary to reserve memory
 *    according to the maximum data length when retrieving the data
 *    of the table containing LONG or LONG RAW.
 * -# Demand of memory can be minimized by dividing the data into pieces
 *    that are less than the specified length and retrieving it multiple times.<br />
 *    See also [About Providing FETCH Information at Run Time](https://docs.oracle.com/database/121/LNOCI/oci05bnd.htm#LNOCI16434).
 *
 * @par Example way to be used:
 * @code
    // usually following comannd is used when you want to dump the values of member.
    ps::lib::sql::occi::cDefine& oDefine_;
    ...
    ps::lib::sql::occi::cPieceVct v_(sizeof(tAttributes));
    ...
    oDefine_.vAddItem(SB4MAXVAL, SQLT_LNG
        , OCI_DYNAMIC_FETCH, ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
        );
   @endcode
 */
class cPieceVct
{
private:
    const ps::lib::cConfigures& conf_; ///< Values from a configuration file.
    /**
     * See cPieceVct::cPieceVct() to study detail.
     */
    const size_t iSkip_;
    /**
     * Number of repeating piecewise operations.
     * Originate from 1.
     */
    int32_t iNumPcs_;
    /**
     * - Before each piecewise operation, initialize
     *   with the maximum acceptable data length.
     * - When the callback function returns with OCI_CONTINUE,
     *   the length of the subsequent piecewise operation
     *   acquired piece is stored.
     * - Therefore, in the last piece, the size is less than
     *   or equal to cPieceVct::iPcsLen_.
     */
    ub4 iActual_;
    /**
     * To store the largest number among each actual data length
     * stored in cPieceVct::length_
     */
    std::string::size_type iLongest_;
    /**
     * - Maximum length of one piece:
     *   specified in conf as "maxlongsize".
     * - It determines the maximum amount of data received
     *   at one time by piece operation and is used for
     *   allocating memory and resetting cPieceVct::iActual_.
     */
    const int32_t iPcsLen_;
    char **data_;
    ps::lib::sql::ind_t *ind_;
    ub4 *length_;
    static char *& pGetDataAddr(void *src, const size_t& diff);
    static ub4 * pGetLengthAddr(ub4 *src, const size_t& diff);
    cPieceVct(const cPieceVct& rhs) =delete;
    cPieceVct& operator=(const cPieceVct& rhs) =delete;
public:
    /**
     * @brief
     * @param [in] iSkip
     * - Distance (in bytes) between two adjacent elements
     *   e.g. Assign operation result such as "sizeof (struct NAME)".
     * @param [in,out] data
     * - Pointer to the first element of the structured array
     *   (data part).
     * - For a pointer of type char of which each element in data is keeping,
     *   the caller must release the memory using the delete[] operator.
     * @param [in,out] ind
     * - Pointer to the first element of the structured array
     *   (indicator variable part).
     * @param [in,out] length
     * - Pointer to the first element of the structured array
     *   (data length variable part).
     */
    cPieceVct(
        const size_t iSkip
        , char **data
        , ps::lib::sql::ind_t *ind
        , ub4 *length
    );
    explicit cPieceVct(
        const size_t iSkip
    );
    ~cPieceVct();
    /**
     * @brief
     * @param [in,out] data
     * - Pointer to the first element of the structured array
     *   (data part).
     * - For a pointer of type char of which each element in data is keeping,
     *   the caller must release the memory using the delete[] operator.
     * @param [in,out] ind
     * - Pointer to the first element of the structured array
     *   (indicator variable part).
     * @param [in,out] length
     * - Pointer to the first element of the structured array
     *   (data length variable part).
     */
    void vSetAddr(
        char **data
        , ps::lib::sql::ind_t *ind
        , ub4 *length
    );
    /**
     * @brief
     */
    static void vTerminateLatest(cPieceVct *pv, const ub4& iter);
    /**
     * @brief
     * - An instance of OCICallbackDefine.
     * - This is invoked at run time to get a pointer to the buffer
     *   into which the fetched data or a piece of it is to be retrieved.
     * - The callback also specifies the indicator, the return code,
     *   and the lengths of the data piece and indicator.
     * - It is passed as the fourth argument when calling OCIDefineDynamic().
     *
     * @param [in,out] octxp
     *   A context pointer passed as an argument to all the callback functions.
     *   When the client library needs a buffer to return the fetched data,
     *   the callback is invoked and the runtime buffers provided return a piece or all of the data.
     * @param [out] defnp
     *   The define handle.
     * @param [out] iter
     *   Specifies which row of this current fetch; 0-based.
     * @param [in] bufpp
     *   Returns a pointer to a buffer to store the column value; that is,
     *   *bufpp points to some appropriate storage for the column value.
     * @param [in,out] alenpp
     *   Used by the application to set the size of the storage it is providing in *bufpp.
     *   After data is fetched into the buffer, alenpp indicates the actual size of the data in bytes.
     *   If the buffer length provided in the first call is insufficient to store
     *   all the data returned by the server, then the callback is called again, and so on.
     * @param [in,out] piecep
     *   Returns a piece value from the callback (application) to OCI, as follows:
     *   The piecep parameter indicates whether the piece to be fetched is the first piece,
     *   OCI_FIRST_PIECE, a subsequent piece, OCI_NEXT_PIECE, or the last piece, OCI_LAST_PIECE.
     *   The program can process the piece the next time the callback is called,
     *   or after the series of callbacks is over.
     *   - IN: The value can be OCI_ONE_PIECE, OCI_FIRST_PIECE, or OCI_NEXT_PIECE.
     *   - OUT: Depends on the IN value:
     *     - The OUT value can be OCI_ONE_PIECE if the IN value was OCI_ONE_PIECE.
     *     - The OUT value can be OCI_ONE_PIECE or OCI_FIRST_PIECE if the IN value was OCI_FIRST_PIECE.
     *     - The OUT value can be OCI_NEXT_PIECE or OCI_LAST_PIECE if the IN value was OCI_NEXT_PIECE.
     * @param [out] indpp
     *   Indicator variable pointer.
     * @param [out] rcodep
     *   Return code variable pointer.
     */
    static sb4 iCbkFunc(
        dvoid *octxp
        , OCIDefine *defnp
        , ub4 iter
        , dvoid **bufpp
        , ub4 **alenpp
        , ub1 *piecep
        , dvoid **indpp
        , ub2 **rcodep
    );
    std::string::size_type iMaxValSize() const ;
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps



