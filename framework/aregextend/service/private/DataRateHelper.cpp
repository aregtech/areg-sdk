/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/DataRateHelper.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Extended features.
 *              Data rate helper object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/DataRateHelper.hpp"
#include "aregextend/service/ServiceCommunicationBase.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// DataRateHelper class implementation
//////////////////////////////////////////////////////////////////////////

DataRateHelper::DataRateHelper(ServiceCommunicationBase& server, bool verbose)
    : mServer(server)
{
    mServer.enable_data_rate(verbose);
}

void DataRateHelper::set_verbose(bool verbose) noexcept
{
    mServer.enable_data_rate(verbose);
}

bool DataRateHelper::is_verbose() const noexcept
{
    return mServer.is_data_rate_enabled();
}

void DataRateHelper::query_data_sent(uint64_t& sizeSent, uint32_t& msgSent) noexcept
{
    mServer.query_data_sent(sizeSent, msgSent);
}

void DataRateHelper::query_data_received(uint64_t& sizeRecv, uint32_t& msgRecv) noexcept
{
    mServer.query_data_received(sizeRecv, msgRecv);
}

DataRateHelper::DataRate DataRateHelper::convert_data_rate_literals(uint64_t sizeBytes)
{
    DataRate dataRate{ 0.0f, "" };

    if (sizeBytes >= ONE_MEGABYTE)
    {
        double rate = static_cast<double>(sizeBytes / static_cast<double>(ONE_MEGABYTE));
        dataRate.first = static_cast<float>(rate);
        dataRate.second = DataRateHelper::MSG_MEGABYTES;
    }
    else if (sizeBytes >= ONE_KILOBYTE)
    {
        double rate = static_cast<double>(sizeBytes / static_cast<double>(ONE_KILOBYTE));
        dataRate.first = static_cast<float>(rate);
        dataRate.second = DataRateHelper::MSG_KILOBYTES;
    }
    else
    {
        dataRate.first = static_cast<float>(sizeBytes);
        dataRate.second = DataRateHelper::MSG_BYTES;
    }

    return dataRate;
}

} // namespace areg::ext
