#ifndef AREG_IPC_REGISTRATIONCONSUMER_HPP
#define AREG_IPC_REGISTRATIONCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/RegistrationConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote service consumer interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/ArrayList.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class StreamableEvent;
class Channel;

//////////////////////////////////////////////////////////////////////////
// RegistrationConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for remote service registration callbacks triggered when service states
 *          change.
 **/
class AREG_API RegistrationConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    RegistrationConsumer() = default;
    virtual ~RegistrationConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// RegistrationConsumer
/************************************************************************/

    /**
     * \brief   Extracts lists of registered remote service providers and consumers matching the
     *          cookie filter.
     *
     * \param   cookie              Cookie filter; pass areg::COOKIE_ANY to retrieve all
     *                              services.
     * \param[out] listProviders       On output contains addresses of remote service providers
     *                                 matching filter.
     * \param[out] listConsumers       On output contains addresses of remote service consumers
     *                                 matching filter.
     **/
    virtual void extract_service_addresses(const ITEM_ID & cookie, ArrayList<StubAddress> & listProviders, ArrayList<ProxyAddress> & listConsumers ) const = 0;

    /**
     * \brief   Triggered when a remote service provider is registered.
     *
     * \param   stub    Address of the newly registered service provider.
     **/
    virtual void on_provider_registered( const StubAddress & stub ) = 0;

    /**
     * \brief   Triggered when a remote service consumer is registered.
     *
     * \param   proxy       Address of the newly registered service consumer.
     **/
    virtual void on_consumer_registered( const ProxyAddress & proxy ) = 0;

    /**
     * \brief   Triggered when a remote service provider is unregistered.
     *
     * \param   stub        Address of the unregistered service provider.
     * \param   reason      Reason for unregistration.
     * \param   cookie      Cookie of source initiating unregistration; ignored if
     *                      areg::COOKIE_ANY.
     **/
    virtual void on_provider_unregistered( const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) = 0;

    /**
     * \brief   Triggered when a remote service consumer is unregistered.
     *
     * \param   proxy       Address of the unregistered service consumer.
     * \param   reason      Reason for unregistration.
     * \param   cookie      Cookie of source initiating unregistration; ignored if
     *                      areg::COOKIE_ANY.
     **/
    virtual void on_consumer_unregistered( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RegistrationConsumer );
};

} // namespace areg
#endif  // AREG_IPC_REGISTRATIONCONSUMER_HPP
