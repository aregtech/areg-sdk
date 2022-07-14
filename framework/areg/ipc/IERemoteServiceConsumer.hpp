#ifndef AREG_IPC_IEREMOTESERVICECONSUMER_HPP
#define AREG_IPC_IEREMOTESERVICECONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IERemoteServiceConsumer.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote service consumer interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TEArrayList.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class StreamableEvent;
class Channel;

//////////////////////////////////////////////////////////////////////////
// IERemoteServiceConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing consumer interface with callbacks, which are
 *          triggered when servicing state update is requested.
 **/
class AREG_API IERemoteServiceConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default destructor
     **/
    IERemoteServiceConsumer( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteServiceConsumer( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteServiceConsumer
/************************************************************************/

    /**
     * \brief   Call to receive list of registered remote stub and proxy services, which connection cookie is equal to 
     *          specified value. In output out_listStubs and out_lisProxies contain list of remote stub and proxy addresses.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering
     * \param   out_listStubs   On output this will contain list of remote stub addresses connected with specified cookie value.
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses connected with specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const = 0;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void registerRemoteStub( const StubAddress & stub ) = 0;

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void registerRemoteProxy( const ProxyAddress & proxy ) = 0;

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     * \param   cookie  The cookie that has initiated unregister message.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisterRemoteStub( const StubAddress & stub, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ ) = 0;

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     * \param   cookie  The cookie that has initiated unregister message.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisterRemoteProxy( const ProxyAddress & proxy, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ ) = 0;

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceStarted( const Channel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceStopped( const Channel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote routing service is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceConnectionLost( const Channel & channel ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IERemoteServiceConsumer );
};

#endif  // AREG_IPC_IEREMOTESERVICECONSUMER_HPP
