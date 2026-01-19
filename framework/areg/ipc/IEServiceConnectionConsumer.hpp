#ifndef AREG_IPC_IESERVICECONNECTIONCONSUMER_HPP
#define AREG_IPC_IESERVICECONNECTIONCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IEServiceConnectionConsumer.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of remote service connection consumer
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Channel;

//////////////////////////////////////////////////////////////////////////
// IEServiceConnectionConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote service connection consumer interface with callbacks, which are
 *          triggered when connection state update is requested.
 **/
class AREG_API IEServiceConnectionConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default destructor
     **/
    IEServiceConnectionConsumer(void) = default;

    virtual ~IEServiceConnectionConsumer(void) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceConnectionConsumer
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel(const Channel& channel) = 0;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel(const Channel& channel) = 0;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel(const Channel& channel) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEServiceConnectionConsumer);
};

#endif  // AREG_IPC_IESERVICECONNECTIONCONSUMER_HPP
