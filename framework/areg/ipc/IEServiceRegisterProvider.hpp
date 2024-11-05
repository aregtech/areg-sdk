#ifndef AREG_IPC_IESERVICEREGISTERPROVIDER_HPP
#define AREG_IPC_IESERVICEREGISTERPROVIDER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IEServiceRegisterProvider.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of public service registration provider.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NEService.hpp"

class StubAddress;
class ProxyAddress;

//////////////////////////////////////////////////////////////////////////
// IEServiceRegisterProvider interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is an interface to register and register the public
 *          service providers and service consumers.
 **/
class AREG_API IEServiceRegisterProvider
{
//////////////////////////////////////////////////////////////////////////
// Protected default constructor and destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor.
     **/
    IEServiceRegisterProvider( void ) = default;

    /**
     * \brief   Protected destructor.
     **/
    virtual ~IEServiceRegisterProvider( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceRegisterProvider interface overrides
/************************************************************************/

    /**
     * \brief   Call to register the remote service provider in the system and connect with service consumers.
     *          When service provider is registered, the service provider and all waiting service consumers
     *          receive appropriate connection notifications.
     * \param   stubService     The address of service provider to register in the system.
     * \return  Returns true if succeeded registration.
     **/
    virtual bool registerServiceProvider(const StubAddress& stubService) = 0;

    /**
     * \brief   Call to unregister the service provider from the system and disconnect service consumers.
     *          All connected service consumers automatically receive disconnect notifications.
     * \param   stubService     The address of service provider to unregister in the system.
     * \param   reason          The reason to unregister and disconnect the service provider.
     **/
    virtual void unregisterServiceProvider(const StubAddress& stubService, const NEService::eDisconnectReason reason) = 0;

    /**
     * \brief   Call to register the remote service consumer in the system and connect to service provider.
     *          If the service provider is already available, the service consumer and the service provider
     *          receive a connection notification.
     * \param   proxyService    The address of the service consumer to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceConsumer(const ProxyAddress& proxyService) = 0;

    /**
     * \brief   Call to unregister the service consumer from the system and disconnect service provider.
     *          Both, the service provider and the service consumer receive appropriate disconnect notification.
     * \param   proxyService    The address of the service consumer to unregister from the system.
     * \param   reason          The reason to unregister and disconnect the service consumer.
     **/
    virtual void unregisterServiceConsumer(const ProxyAddress& proxyService, const NEService::eDisconnectReason reason) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEServiceRegisterProvider);
};

#endif // AREG_IPC_IESERVICEREGISTERPROVIDER_HPP
