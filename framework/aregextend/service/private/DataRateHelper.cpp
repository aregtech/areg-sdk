/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/DataRateHelper.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Extended features.
 *              Data rate helper object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/DataRateHelper.hpp"

//////////////////////////////////////////////////////////////////////////
// DataRateHelper class implementation
//////////////////////////////////////////////////////////////////////////

DataRateHelper::DataRateHelper(ServerSendThread& sendThread, ServerReceiveThread& receiveThread, bool verbose)
    : mSendThread   (sendThread)
    , mReceiveThread(receiveThread)
{
    mSendThread.setEnableCalculateData(verbose);
    mReceiveThread.setEnableCalculateData(verbose);
}

void DataRateHelper::setVerbose(bool verbose)
{
    mSendThread.setEnableCalculateData(verbose);
    mReceiveThread.setEnableCalculateData(verbose);
}

bool DataRateHelper::isVerbose(void) const
{
    return mSendThread.isCalculateDataEnabled() && mReceiveThread.isCalculateDataEnabled();
}

DataRateHelper::DataRate DataRateHelper::convertDataRateLiterals(uint32_t sizeBytes)
{
    DataRate dataRate{ 0.0f, "" };

    if (sizeBytes >= ONE_MEGABYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_MEGABYTE;
        dataRate.first = static_cast<float>(rate);
        dataRate.second = DataRateHelper::MSG_MEGABYTES;
    }
    else if (sizeBytes >= ONE_KILOBYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_KILOBYTE;
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
