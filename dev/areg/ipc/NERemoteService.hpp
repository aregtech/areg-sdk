#ifndef AREG_IPC_NEREMOTESERVICE_HPP
#define AREG_IPC_NEREMOTESERVICE_HPP
/************************************************************************
 * \file        areg/ipc/NERemoteService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Remote service namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/NEApplication.hpp"

//////////////////////////////////////////////////////////////////////////
// NERemoteService namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing namespace with shared objects and helper methods
 **/
namespace NERemoteService
{
    /**
     * \brief   NERemoteService::eServiceConnection
     *          Remote servicing connection type
     **/
    typedef enum E_ServiceConnection
    {
          ConnectionTcpip       = 0 //!< Connection type open TCP/IP
        , ConnectionUndefined       //!< Connection type undefined
    } eServiceConnection;

    /**
     * \brief   NERemoteService::DEFAULT_CONFIG_FILE
     *          Default name of routing service connection configuration file.
     **/
    const char * const      DEFAULT_CONFIG_FILE         = NEApplication::DEFAULT_ROUTER_CONFIG_FILE;
    /**
     * \brief   NERemoteService::DEFAULT_ROUTER_HOST
     *          Default IP-Address of routing service.
     **/
    const char * const      DEFAULT_ROUTER_HOST         = "127.0.0.1";
    /**
     * \brief   NERemoteService::DEFAULT_ROUTER_PORT
     *          Default connection port number of routing service.
     **/
    const unsigned short    DEFAULT_ROUTER_PORT         = 8181;
    /**
     * \brief   NERemoteService::IP_SEPARATOR
     *          The property separator
     **/
    const char              IP_SEPARATOR                = '.';
    /**
     * \brief   NERemoteService::STR_CONNECTION_TYPE_TCPIP
     *          String value of TCP/IP connection type
     **/
    const char * const      STR_CONNECTION_TYPE_TCPIP   = "tcpip";

    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_NAME
     *          Maximum length of property name in configuration file.
     **/
    const int               MAXLEN_PROPERTY_NAME        = 256;
    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_HOST
     *          Maximum length of message router host name in configuration file
     **/
    const int               MAXLEN_PROPERTY_HOST        = 256;
    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_PORT
     *          Maximum length of message router connection port number in configuration file
     **/
    const int               MAXLEN_PROPERTY_PORT        = 8;

    /**
     * \brief   NERemoteService::CONFIG_KEY_CONNECTION
     *          The name of property for connection
     **/
    const char * const      CONFIG_KEY_CONNECTION       = "connection";
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_TYPE
     *          The name of property for connection type
     **/
    const char * const      CONFIG_KEY_PROP_TYPE        = "type";
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_ENABLE
     *          The name of value in configuration
     **/
    const char * const      CONFIG_KEY_PROP_ENABLE      = "enable";
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_NAME
     *          The name of property for connection name
     **/
    const char * const      CONFIG_KEY_PROP_NAME        = "name";
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_ADDRESS
     *          The name of property for connection address
     **/
    const char * const      CONFIG_KEY_PROP_ADDRESS     = "address";
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_PORT
     *          The name of property for connection port
     **/
    const char * const      CONFIG_KEY_PROP_PORT        = "port";

    /**
     * \brief   NERemoteService::GetServiceConnectionTypeString
     *          Returns the string value of service connection type
     * \param   connectionType  The connection type to convert
     **/
    const char * getServiceConnectionTypeString( NERemoteService::eServiceConnection connectionType );
    /**
     * \brief   NERemoteService::GetServiceConnectionType
     *          Converts string to connection type value.
     * \param   connectionType  The string of connection type name to convert
     * \param   caseSensitive   Flag, indicating whether the given name to convert is case sensitive or not.
     **/
    NERemoteService::eServiceConnection getServiceConnectionType( const char * connectionType, bool caseSensitive = true );
}

#endif  // AREG_IPC_NEREMOTESERVICE_HPP
