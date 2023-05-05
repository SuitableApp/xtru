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
 * @file cBaseException.h
 * @brief Exception handling
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CBADEXECPTION_H
#define CBADEXECPTION_H

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @brief 
 *  Exception return value
 * @enum tType
 *  Return type
 */
typedef enum 
{ iCompletedSuccessfully                 =   0
, iRequestToShowUsage                    =   0
, iNullPointer                       //  =   1
, iQueueIsEmpty                      //  =   2
, iNamedMtxExists                    //  =   3
//, iWin32ApiErr                       //  =   4
, iInvalidParamValue                 //  =   5
, iFileCouldNotOpen                  //  =   6
, iApplicationError                  //  =   7
, iDelimiterNotExists                //  =   8
} tType;

/**
 * @class cBaseException
 * @brief 
 *  This class is basic exception class.
 */
class cBaseException : public std::exception
{
private:
    tType iRc_;
    tstring sMessage_;
public:
    /**
     * @brief 
     *  Initialize exceptions with specified type and message
     * @param[in] tRc Return type
     * @param[in] sMsg Error message
     */
    cBaseException(const tType iRc, const tstring& sMsg = TEXT(""))
        : iRc_(iRc), sMessage_(sMsg)
    {}
    
    /**
     * @brief 
     */
    ~cBaseException() throw()
    {}
    
    /**
     * @brief 
     *  Retrieve the error message
     * @return Error message
     */
    virtual const tstring twhat() const
    { return sMessage_; }
    
    /**
     * @brief 
     *  Return value type
     * @return Return type
     */
    virtual int rc() const
    { return iRc_; }
};

/**
 * @class cRequestToShowUsage
 * @brief 
 *  This class is an exception class for usage indicators.
 */
class cRequestToShowUsage : public cBaseException
{
public:
    cRequestToShowUsage(const tstring& sMsg = TEXT("Thrown cRequestToShowUsage."))
        : cBaseException(iCompletedSuccessfully, sMsg) {}
};

/**
 * @class cNullPointer
 * @brief 
 *  This class is a null pointer error exception class.
 */
class cNullPointer : public cBaseException
{
public:
    cNullPointer(const tstring& sMsg = TEXT("Thrown cNullPointer."))
        : cBaseException(iNullPointer, sMsg) {}
};

/**
 * @class cQueueIsEmpty
 * @brief 
 *  This class is a queue is empty error exception class.
 */
class cQueueIsEmpty : public cBaseException
{
public:
    cQueueIsEmpty(const tstring& sMsg = TEXT("Thrown cQueueIsEmpty."))
        : cBaseException(iQueueIsEmpty, sMsg) {}
};

/**
 * @class cNamedMtxExists
 * @brief 
 *  This class is an exception class for multiple invocations.
 */
class cNamedMtxExists : public cBaseException
{
public:
    cNamedMtxExists(const tstring& sMsg = TEXT("Thrown cNamedMtxExists."))
        : cBaseException(iNamedMtxExists, sMsg) {}
};

/**
 * @class cInvalidParamValue
 * @brief 
 *  This class is an exception class for parameter errors.
 */
class cInvalidParamValue : public cBaseException
{
public:
    cInvalidParamValue(const tstring& sMsg = TEXT("Thrown cInvalidParamValue."))
        : cBaseException(iInvalidParamValue, sMsg) {}
};

/**
 * @class cFileCouldNotOpen
 * @brief 
 *  This class is an exception class for file could not open errors.
 */
class cFileCouldNotOpen : public cBaseException
{
public:
    cFileCouldNotOpen(const tstring& sMsg = TEXT("Thrown cFileCouldNotOpen."))
        : cBaseException(iFileCouldNotOpen, sMsg) {}
};

/**
 * @class cApplicationError
 * @brief 
 *  This class is an exception class for application errors.
 */
class cApplicationError : public cBaseException
{
public:
    cApplicationError(const tstring& sMsg = TEXT("Thrown cApplicationError."))
        : cBaseException(iApplicationError, sMsg) {}
};

/**
 * @class cDelimiterNotExists
 * @brief 
 *  This class is an exception class for delimiter not exists errors.
 */
class cDelimiterNotExists : public cBaseException
{
public:
    cDelimiterNotExists(const tstring& sMsg = TEXT("Thrown cDelimiterNotExists."))
        : cBaseException(iDelimiterNotExists, sMsg) {}
};

/**
 * @brief 
 *  It performs an assert check, and if it is an error it raises the specified exception.
 * @param[in] assert assert
 * @param[in] exception Generate exceptions
 */
template<class A, class E>
inline void Assert(A assert, const E& exception)
{
    if (!assert)
    {
        throw exception;
    }
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
#endif /* CBADEXECPTION_H */
