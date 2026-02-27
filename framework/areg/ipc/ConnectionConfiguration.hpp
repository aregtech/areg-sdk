#ifndef AREG_IPC_CONNECTIONCONFIGURATION_HPP
#define AREG_IPC_CONNECTIONCONFIGURATION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionConfiguration.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/RemoteServiceDefs.hpp"


//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Loads and manages connection configuration for remote services including protocol type,
 *          host name, and port number.
 **/
class AREG_API ConnectionConfiguration
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection configuration with service and connection type names.
     *
     * \param   service         Service name to read configuration properties.
     * \param   connectType     Connection type name to read configuration properties.
     **/
    ConnectionConfiguration( const String & service, const String & connectType );

    /**
     * \brief   Initializes connection configuration with service and connection type enums.
     *
     * \param   service         Service type to read configuration properties.
     * \param   connectType     Connection type to read configuration properties.
     **/
    ConnectionConfiguration(NERemoteService::RemoteServiceKind service, NERemoteService::ConnectionType connectType);

    /**
     * \brief   Destructor.
     **/
    virtual ~ConnectionConfiguration() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the remote service name.
     **/
    inline const String& remote_service_name() const;

    /**
     * \brief   Returns the connection type name.
     **/
    inline const String& connection_type() const;

    /**
     * \brief   Returns true if application is configured via file or defaults.
     **/
    bool is_configured() const;

    /**
     * \brief   Returns the connection enabled/disabled flag.
     **/
    bool connection_enable_flag() const;

    /**
     * \brief   Sets the connection enabled/disabled flag.
     *
     * \param   is_enabled       Flag indicating whether connection should be enabled.
     **/
    void set_connection_enable(bool is_enabled);

    /**
     * \brief   Returns the connection port number.
     **/
    uint16_t connection_port() const;

    /**
     * \brief   Sets the connection port number.
     *
     * \param   portNr      Port number to set.
     **/
    void set_connection_port(uint16_t portNr);

    /**
     * \brief   Returns the connection address.
     **/
    String connection_address() const;

    /**
     * \brief   Sets the connection address.
     *
     * \param   address     Address to set.
     **/
    void set_connection_address(const String& address);

    /**
     * \brief   Sets the connection address and port number.
     *
     * \param   address     Address to set.
     * \param   portNr      Port number to set.
     **/
    void set_connection_data(const String& address, uint16_t portNr);

    /**
     * \brief   Extracts IP address bytes from connection address.
     *
     * \param[out] field0      First byte of IP address.
     * \param[out] field1      Second byte of IP address.
     * \param[out] field2      Third byte of IP address.
     * \param[out] field3      Fourth byte of IP address.
     * \return  Returns true if valid IP address was extracted.
     **/
    bool connection_ip_address( uint8_t & field0
                               , uint8_t & field1
                               , uint8_t & field2
                               , uint8_t & field3);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
private:
    //! The name of the remote connection service name to look in configuration.
    const String    mServiceName;

    //! The name of the connection type to look in configuration.
    const String    mConnectType;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionConfiguration() = delete;
    AREG_NOCOPY_NOMOVE( ConnectionConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration inline methods.
//////////////////////////////////////////////////////////////////////////

inline const String& ConnectionConfiguration::remote_service_name() const
{
    return mServiceName;
}

inline const String& ConnectionConfiguration::connection_type() const
{
    return mConnectType;
}

#endif  // AREG_IPC_CONNECTIONCONFIGURATION_HPP
