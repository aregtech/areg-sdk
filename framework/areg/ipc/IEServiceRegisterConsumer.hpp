#ifndef AREG_IPC_IESERVICEREGISTERCONSUMER_HPP
#define AREG_IPC_IESERVICEREGISTERCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IEServiceRegisterConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote service consumer interface
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
// IEServiceRegisterConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote service provider and consumer interface with callbacks,
 *          which are triggered when serves change the states.
 **/
class AREG_API IEServiceRegisterConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default destructor
     **/
    IEServiceRegisterConsumer( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IEServiceRegisterConsumer( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceRegisterConsumer
/************************************************************************/

    /**
     * \brief   Call to extract the list of addresses of registered and valid remote service providers and consumers of specified cookie.
     *          If cookie value is 'NEService::COOKIE_ANY' it retrieves the list of all remote service providers and consumers.
     *          On output out_listStubs and out_lisProxies contain the list of remote services.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering.
     * \param   out_listStubs   On output this contains the list of address of the remote service providers of specified cookie.
     * \param   out_lisProxies  On output this contains the list of address of the remote service consumers of specified cookie.
     **/
    virtual void extractRemoteServiceAddresses(const ITEM_ID & cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const = 0;

    /**
     * \brief   Triggered when a remote service provider is registered in the system.
     * \param   stub    The address of remote service provider that has been registered.
     **/
    virtual void registeredRemoteServiceProvider( const StubAddress & stub ) = 0;

    /**
     * \brief   Triggered when a remote service consumer is registered in the system.
     * \param   proxy   The address of remote service consumer that has been registered.
     **/
    virtual void registeredRemoteServiceConsumer( const ProxyAddress & proxy ) = 0;

    /**
     * \brief   Triggered when a remote service provider is unregistered from the system.
     * \param   stub    The address of the remote service provider that has been unregistered.
     * \param   reason  The reason that remote service provider is unregistered.
     * \param   cookie  The cookie of source that has initiated to unregister provider.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisteredRemoteServiceProvider( const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ ) = 0;

    /**
     * \brief   Triggered when a remote service consumer is unregistered from the system.
     * \param   proxy   The address of the remote service consumer that has been unregistered.
     * \param   reason  The reason that remote service consumer is unregistered.
     * \param   cookie  The cookie of source that has initiated to unregister consumer.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisteredRemoteServiceConsumer( const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEServiceRegisterConsumer );
};

#endif  // AREG_IPC_IESERVICEREGISTERCONSUMER_HPP
