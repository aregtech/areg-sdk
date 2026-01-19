#ifndef AREG_IPC_CONNECTIONCONFIGURATION_HPP
#define AREG_IPC_CONNECTIONCONFIGURATION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionConfiguration.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/NERemoteService.hpp"


//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected configuration class. The object loads configuration file,
 *          parses and initializes properties such as connection enable / disable,
 *          connection type (protocol), host name and port number, etc.
 **/
class AREG_API ConnectionConfiguration
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection configuration object to use properties based
     *          on specified service and connection type.
     * \param   service     The name of the service to read configuration properties.
     * \param   connectType The name of the connection type to read configuration properties.
     **/
    ConnectionConfiguration( const String & service, const String & connectType );

    /**
     * \brief   Initializes connection configuration object to use properties based
     *          on specified service and connection type.
     * \param   service     The type of the service to read configuration properties.
     * \param   connectType The type of the connection to read configuration properties.
     **/
    ConnectionConfiguration(NERemoteService::eRemoteServices service, NERemoteService::eConnectionTypes connectType);

    /**
     * \brief   Destructor.
     **/
    virtual ~ConnectionConfiguration( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the optional name of the remote service.
     **/
    inline const String& getRemoteServiceName(void) const;

    /**
     * \brief   Returns the name of the connection type of the remote service.
     **/
    inline const String& getConnectionType(void) const;

    /**
     * \brief   Returns true if the application already configured by reading configuration file or by default configuration.
     **/
    bool isConfigured(void) const;

    /**
     * \brief   Returns connection enabled / disabled flag of the remote service and type.
     **/
    bool getConnectionEnableFlag( void ) const;

    /**
     * \brief   Set service connection enable flag of the remote service and type.
     **/
    void setConnectionEnableFlag(bool isEnabled);

    /**
     * \brief   Returns the connection port number of the remote service and type.
     **/
    unsigned short getConnectionPort( void ) const;

    /**
     * \brief   Sets the connection port number of the remote service and type.
     **/
    void setConnectionPort(unsigned short portNr);

    /**
     * \brief   Returns the connection address of the remote service and type.
     **/
    String getConnectionAddress( void ) const;

    /**
     * \brief   Sets the connection address of the remote service and type.
     **/
    void setConnectionAddress(const String& address);

    /**
     * \brief   Sets the connection address and port number of the remote service and type.
     * \param   address     The connection address.
     * \param   portNr      The connection port number.
     **/
    void setConnectionData(const String& address, unsigned short portNr);

    /**
     * \brief   Returns byte sets of connection host IP address of given connection section.
     **/
    bool getConnectionIpAddress( unsigned char & OUT field0
                               , unsigned char & OUT field1
                               , unsigned char & OUT field2
                               , unsigned char & OUT field3);

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
    ConnectionConfiguration(void) = delete;
    DECLARE_NOCOPY_NOMOVE( ConnectionConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration inline methods.
//////////////////////////////////////////////////////////////////////////

inline const String& ConnectionConfiguration::getRemoteServiceName(void) const
{
    return mServiceName;
}

inline const String& ConnectionConfiguration::getConnectionType(void) const
{
    return mConnectType;
}

#endif  // AREG_IPC_CONNECTIONCONFIGURATION_HPP
