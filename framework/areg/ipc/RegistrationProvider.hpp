#ifndef AREG_IPC_REGISTRATIONPROVIDER_HPP
#define AREG_IPC_REGISTRATIONPROVIDER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/RegistrationProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of public service registration provider.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceDefs.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {

    class StubAddress;
    class ProxyAddress;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RegistrationProvider interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for registering and unregistering remote service providers and consumers.
 **/
class AREG_API RegistrationProvider
{
//////////////////////////////////////////////////////////////////////////
// Protected default constructor and destructor
//////////////////////////////////////////////////////////////////////////
protected:
    RegistrationProvider() = default;

    virtual ~RegistrationProvider() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// RegistrationProvider interface overrides
/************************************************************************/

    /**
     * \brief   Registers remote service provider and connects with waiting service consumers. All
     *          parties receive connection notifications.
     *
     * \param   stubService     Address of service provider to register.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool register_service_provider(const StubAddress& stubService) = 0;

    /**
     * \brief   Unregisters service provider and disconnects all service consumers. Connected
     *          consumers receive disconnect notifications.
     *
     * \param   stubService     Address of service provider to unregister.
     * \param   reason          Reason for unregistration.
     **/
    virtual void unregister_service_provider(const StubAddress& stubService, const areg::DisconnectReason reason) = 0;

    /**
     * \brief   Registers remote service consumer and connects to service provider if available.
     *          Both parties receive connection notification.
     *
     * \param   proxyService    Address of service consumer to register.
     * \return  Returns true if registration started successfully.
     **/
    virtual bool register_service_consumer(const ProxyAddress& proxyService) = 0;

    /**
     * \brief   Unregisters service consumer and disconnects from service provider. Both parties
     *          receive disconnect notification.
     *
     * \param   proxyService    Address of service consumer to unregister.
     * \param   reason          Reason for unregistration.
     **/
    virtual void unregister_service_consumer(const ProxyAddress& proxyService, const areg::DisconnectReason reason) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(RegistrationProvider);
};

} // namespace areg
#endif // AREG_IPC_REGISTRATIONPROVIDER_HPP
