#ifndef AREG_IPC_CONNECTIONCONSUMER_HPP
#define AREG_IPC_CONNECTIONCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of remote service connection consumer
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Channel;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ConnectionConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for remote service connection state callbacks.
 **/
class AREG_API ConnectionConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    ConnectionConsumer() = default;

    virtual ~ConnectionConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ConnectionConsumer
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     *
     * \param   channel     The established communication channel.
     **/
    virtual void on_service_channel_connected(const Channel& channel) = 0;

    /**
     * \brief   Triggered when remote service connection is disconnected.
     *
     * \param   channel     The disconnected communication channel.
     **/
    virtual void on_service_channel_disconnected(const Channel& channel) = 0;

    /**
     * \brief   Triggered when remote service connection is unexpectedly lost (not stopped by API
     *          call).
     *
     * \param   channel     The lost communication channel.
     **/
    virtual void on_service_channel_lost(const Channel& channel) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ConnectionConsumer);
};

} // namespace areg
#endif  // AREG_IPC_CONNECTIONCONSUMER_HPP
