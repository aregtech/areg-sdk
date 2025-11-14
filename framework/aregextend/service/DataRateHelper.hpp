#ifndef AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
#define AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/DataRateHelper.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Extended features.
 *              Data rate helper object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "aregextend/service/private/ServerSendThread.hpp"
#include "aregextend/service/private/ServerReceiveThread.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ServerSendThread;
class ServerReceiveThread;

//////////////////////////////////////////////////////////////////////////
// DataRateHelper class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple helper class to calculate data rate.
 **/
class DataRateHelper
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    //!< Bytes in 1 Kilobyte.
    static constexpr uint32_t           ONE_KILOBYTE    { NECommon::ONE_KILOBYTE };
    //!< Bytes in 1 megabyte.
    static constexpr uint32_t           ONE_MEGABYTE    { NECommon::ONE_MEGABYTE };
    //!< String kilobytes per second
    static constexpr std::string_view   MSG_KILOBYTES   { "KBytes / sec." };
    //!< String megabytes per second
    static constexpr std::string_view   MSG_MEGABYTES   { "MBytes / sec." };
    //!< String bytes per second
    static constexpr std::string_view   MSG_BYTES       { " Bytes / sec." };

    //!< The type of data rate. Contains value and the associated literal.
    using DataRate  = std::pair<float, std::string>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object, sets threads that the rate can be queried.
     *          If passed 'verbose' parameter is 'false' on each query it returns zero.
     *          Otherwise, returns the actual value.
     * \param   sendThread      The thread that can be queried the data size sent.
     * \param   receiveThread   The threat that can be queried the data size received.
     * \param   verbose         The flag, indicating whether the actual size should be
     *                          computed or should return zero.
     **/
    DataRateHelper(ServerSendThread& sendThread, ServerReceiveThread& receiveThread, bool verbose);

    ~DataRateHelper(void) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and actions.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets the verbose state of the helper class.
     * \param   verbose     The verbose flag to set.
     **/
    void setVerbose(bool verbose);

    /**
     * \brief   Returns the verbose flag, indicating whether the data send / receive computation is enabled.
     **/
    bool isVerbose(void) const;

    /**
     * \brief   Return the size in bytes of data sent since last query.
     *          If verbose flag is false, returns zero.
     **/
    inline uint32_t queryBytesSent(void) const;

    /**
     * \brief   Return the size in bytes of data received since last query.
     *          If verbose flag is false, returns zero.
     **/
    inline uint32_t queryBytesReceived(void) const;

    /**
     * \brief   Return the size of data sent since last query with literal.
     *          If verbose flag is false, returns zero.
     **/
    inline DataRate queryBytesSentWithLiterals(void) const;

    /**
     * \brief   Return the size of data received since last query with literal.
     *          If verbose flag is false, returns zero.
     **/
    inline DataRate queryBytesReceivedWithLiterals(void) const;

    //!< This pair contains size in bytes and message indicating MB, KB or Bytes.
    static DataRate convertDataRateLiterals(uint32_t sizeBytes);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread &      mSendThread;    //!< The thread to query the sent data size in bytes.
    ServerReceiveThread &   mReceiveThread; //!< The thread to query the received data size in bytes.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DataRateHelper(void) = delete;
    DECLARE_NOCOPY_NOMOVE(DataRateHelper);
};

//////////////////////////////////////////////////////////////////////////
// DataRateHelper class inline methods.
//////////////////////////////////////////////////////////////////////////

inline uint32_t DataRateHelper::queryBytesSent(void) const
{
    return mSendThread.extractDataSend();
}

inline uint32_t DataRateHelper::queryBytesReceived(void) const
{
    return mReceiveThread.extractDataReceive();
}

inline DataRateHelper::DataRate DataRateHelper::queryBytesSentWithLiterals(void) const
{
    return DataRateHelper::DataRateHelper::convertDataRateLiterals(queryBytesSent());
}

inline DataRateHelper::DataRate DataRateHelper::queryBytesReceivedWithLiterals(void) const
{
    return DataRateHelper::DataRateHelper::convertDataRateLiterals(queryBytesReceived());
}

#endif  // AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
