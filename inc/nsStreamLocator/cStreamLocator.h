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
namespace nsStreamLocator
{

class cStreamLocatorImpl;

/**
 * @class cStreamLocator
 * @brief
 * Class for managing and building output locations.
 * Analyze the output location string
 * and construct the appropriate output stream.
 * Details of the format of the output location string
 * conform to the operation manual.
 * @see cOstreamLocalProcess
 * @see cOstreamNamedPipe
 */
class cStreamLocator
    : public cStreamSupplier
{
public:
    /**
     * @brief
     * Construct an instance of this class.<br />
     *
     * @param[in] sOwner
     * @param[in] sTableName
     * @param[in] sPartitionName
     */
    cStreamLocator(
        const std::string& sOwner
        , const std::string& sTableName
        , const std::string& sPartitionName
    );
    /**
     * @brief
     * Default destructor.
     */
    virtual ~cStreamLocator();
    /**
     * Construct the appropriate output stream.
     *
     * @return
     *   Output stream to the specified location.
     * @exception
     *   Failed to construct output stream.
     */
    virtual std::unique_ptr<std::ostream> oOpen(const tExtType&, const std::string&);
    /**
     * @brief
     */
    virtual const boost::filesystem::path& oGetsLastOpendFilename() const;
    /**
     * @brief
     */
    virtual const std::string sGetPartitionName() const;
private:
    std::unique_ptr<cStreamLocatorImpl, void (*)(cStreamLocatorImpl *)> oImpl_;
};

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
