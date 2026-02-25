#ifndef AREG_IPC_CONNECTIONPROVIDER_HPP
#define AREG_IPC_CONNECTIONPROVIDER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of Remote Service connection provider
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/RemoteMessage.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;

//////////////////////////////////////////////////////////////////////////
// ConnectionProvider interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for remote service connection handling on client and server sides.
 **/
class AREG_API ConnectionProvider
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief
     **/
    ConnectionProvider() = default;

    /**
     * \brief   Destructor
     **/
    virtual ~ConnectionProvider() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ConnectionProvider interface overrides
/************************************************************************/

    /**
     * \brief   Configures remote service with specified service type and connection types.
     *
     * \param   service         The service type to configure.
     * \param   connectTypes    Bitwise set of connection types (e.g., tcpip, udp).
     * \return  Returns true if configuration succeeded; false otherwise.
     **/
    virtual bool setup_connection_data( NERemoteService::RemoteServiceKind service, uint32_t connectTypes ) = 0;

    /**
     * \brief   Sets router service host name and port number. Does not restart service if already
     *          running.
     *
     * \param   hostName    IP address or host name of routing service.
     * \param   portNr      Port number of routing service.
     **/
    virtual void apply_connection_data( const String & hostName, uint16_t portNr ) = 0;

    /**
     * \brief   Starts remote service. Host name and port must already be set.
     *
     * \return  Returns true if start is triggered successfully.
     **/
    virtual bool connect_service_host() = 0;

    /**
     * \brief   Restarts remote service. Host name and port must already be set. Re-establishes lost
     *          connections or initiates new ones.
     *
     * \return  Returns true if restart succeeded.
     **/
    virtual bool reconnect_service_host() = 0;

    /**
     * \brief   Stops service and disables remote communication.
     **/
    virtual void disconnect_service_host() = 0;

    /**
     * \brief   Returns true if remote service is started and ready to operate.
     **/
    virtual bool is_host_connected() const = 0;

    /**
     * \brief   Returns true if remote service connection is pending (triggered but not yet
     *          connected).
     **/
    virtual bool is_host_pending() const = 0;

    /**
     * \brief   Returns true if service is configured and ready to start.
     **/
    virtual bool is_host_setup() const = 0;

    /**
     * \brief   Creates service connect request message with specified source, target, and message
     *          source type.
     *
     * \param   source          ID of the source sending the connect message.
     * \param   target          ID of the target receiving the connect message.
     * \param   msgSource       Message source type of the connecting client.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage connect_message( const ITEM_ID & source, const ITEM_ID & target, NEService::MessageSource msgSource) const = 0;

    /**
     * \brief   Creates service disconnect request message with specified source and target.
     *
     * \param   source      ID of the source sending the disconnect message.
     * \param   target      ID of the target receiving the disconnect message.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ConnectionProvider );
};

#endif  // AREG_IPC_CONNECTIONPROVIDER_HPP