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

namespace lib
{

namespace sql
{

namespace occi
{

class cBind
{
private:
    class tBind
    {
        friend class cBind;
        uint32_t pos_; // 1 origin.
        void *valuep_;
        int32_t value_sz_;   // A size of data value in byte.
        std::string tnm_;        // stores a type name of the host valiable.
        int16_t *indp_;     // Pointer to arrayed indicators.
        uint16_t *alenp_;  // Pointer to arrayed actual length variable.
        uint16_t *rcodep_; // Pointer to array of column-level return codes.
        uint32_t pvskip_;   // A distance in bytes for the next value.
        uint32_t indskip_;  // same as above, for the next indicator.
        uint32_t alskip_;   // same as above, for the next actual length variable.
        uint32_t rcskip_;   // same as above, for the next column-level return codes.
        std::string sVal_;       // Representation of value as string;
    public:
        tBind(
            uint32_t pos
            , void *valuep
            , int32_t value_sz
            , const std::string& tnm
            , int16_t *indp
            , uint16_t *alenp
            , uint16_t *rcodep
            , uint32_t pvskip
            , uint32_t indskip
            , uint32_t alskip
            , uint32_t rcskip
            , const std::string& sVal
        );
        tBind(const tBind& rhs);
        tBind& operator=(const tBind& rhs);
    };
    typedef ps::lib::cMap<std::string, tBind> tType;
    tType oMap_;
    ps::lib::cVector<boost::any> oStoredValues_;

public:
    cBind();
    cBind(const cBind& rhs);
    cBind& operator=(const cBind& rhs);
    /**
     * @brief
     */
    template<typename T>
    void vAddItem(
        const char *phn // means a place-holder name.
        , const T* val
        , int16_t *indp =0
        , uint16_t *alenp =0
        , uint16_t *rcodep =0
        , uint32_t pvskip =0
        , uint32_t indskip =0
        , uint32_t alskip =0
        , uint32_t rcskip =0
    ){
        oMap_.insert(
            tType::value_type(
                phn
                , tBind(
                    oMap_.size() + 1, (void*) val, sizeof(T)
                    , typeid(T).name(), indp, alenp, rcodep
                    , pvskip, indskip, alskip, rcskip
                    , boost::lexical_cast<std::string>(*val)
                )
            )
        );
    }
    /**
     * @brief
     */
    void vAddItem(
        const char *phn // means a place-holder name.
        , const std::string& val
        , int16_t *indp =0
        , uint16_t *alenp =0
        , uint16_t *rcodep =0
        , uint32_t pvskip =0
        , uint32_t indskip =0
        , uint32_t alskip =0
        , uint32_t rcskip =0
    ){
        oMap_.insert(
            tType::value_type(
                phn
                , tBind(
                    oMap_.size() + 1, (void*) val.c_str(), val.length()
                    , typeid(std::string).name(), indp, alenp, rcodep
                    , pvskip, indskip, alskip, rcskip, val
                )
            )
        );
    }
    /**
     * @brief
     * to output value into a C-style string.
     */
    void vAddItem(
        const char *phn // means a place-holder name.
        , char * val    // output mode variable
        , const uint16_t& max
        , int16_t *indp =0
        , uint16_t *alenp =0
        , uint16_t *rcodep =0
        , uint32_t pvskip =0
        , uint32_t indskip =0
        , uint32_t alskip =0
        , uint32_t rcskip =0
    ){
        oMap_.insert(
            tType::value_type(
                phn
                , tBind(
                    oMap_.size() + 1, (void*) val, max
                    , typeid(char *).name(), indp, alenp, rcodep
                    , pvskip, indskip, alskip, rcskip, val
                )
            )
        );
    }
    std::string sAttachTo(oracle::occi::Statement *stmt, const std::string& phn, const int32_t param_pos) const;
    bool find(const tType::key_type& phn) const ;
    std::string  sGetTypeIdName(const tType::key_type& phn) const;
    tType::size_type iSize() const
    {
        return oMap_.size();
    }
    ps::lib::str_vct sPrintItems() const;
    void vImportParameters(const boost::filesystem::path& sFile);
    template<typename T, typename F>
    void vStoreItem(
        const std::string& phn // means a place-holder name.
        , const T& val
        , F oAdaptor
    ){
        oStoredValues_.push_back(val);
        auto& oObj = boost::any_cast<T&>(oStoredValues_.back());
        vAddItem(phn.c_str(), oAdaptor(oObj));
    }
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

