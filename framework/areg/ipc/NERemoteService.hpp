#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/NERemoteService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Remote service namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/NESocket.hpp"

#include <string_view>

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
    typedef enum class E_ServiceConnection
    {
          ConnectionTcpip       = 0 //!< Connection type open TCP/IP
        , ConnectionUndefined       //!< Connection type undefined
    } eServiceConnection;

    /**
     * \brief   NERemoteService::DEFAULT_ROUTER_HOST
     *          Default IP-Address of routing service.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_HOST         { NESocket::LocalAddress };
    /**
     * \brief   NERemoteService::DEFAULT_ROUTER_PORT
     *          Default connection port number of routing service.
     **/
    constexpr unsigned short    DEFAULT_ROUTER_PORT         { 8181 };
    /**
     * \brief   NERemoteService::IP_SEPARATOR
     *          The property separator
     **/
    constexpr char              IP_SEPARATOR                { '.' };
    /**
     * \brief   NERemoteService::STR_CONNECTION_TYPE_TCPIP
     *          String value of TCP/IP connection type
     **/
    constexpr std::string_view  STR_CONNECTION_TYPE_TCPIP   { "tcpip" };

    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_NAME
     *          Maximum length of property name in configuration file.
     **/
    constexpr int               MAXLEN_PROPERTY_NAME        { 256 };
    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_HOST
     *          Maximum length of message router host name in configuration file
     **/
    constexpr int               MAXLEN_PROPERTY_HOST        { 256 };
    /**
     * \brief   NERemoteService::MAXLEN_PROPERTY_PORT
     *          Maximum length of message router connection port number in configuration file
     **/
    constexpr int               MAXLEN_PROPERTY_PORT        { 8 };

    /**
     * \brief   NERemoteService::CONFIG_KEY_CONNECTION
     *          The name of property for connection
     **/
    constexpr std::string_view  CONFIG_KEY_CONNECTION       { "connection" };
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_TYPE
     *          The name of property for connection type
     **/
    constexpr std::string_view  CONFIG_KEY_PROP_TYPE        { "type" };
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_ENABLE
     *          The name of value in configuration
     **/
    constexpr std::string_view  CONFIG_KEY_PROP_ENABLE      { "enable" };
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_NAME
     *          The name of property for connection name
     **/
    constexpr std::string_view  CONFIG_KEY_PROP_NAME        { "name" };
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_ADDRESS
     *          The name of property for connection address
     **/
    constexpr std::string_view  CONFIG_KEY_PROP_ADDRESS     { "address" };
    /**
     * \brief   NERemoteService::CONFIG_KEY_PROP_PORT
     *          The name of property for connection port
     **/
    constexpr std::string_view  CONFIG_KEY_PROP_PORT        { "port" };

    /**
     * \brief   NERemoteService::GetServiceConnectionTypeString
     *          Returns the string value of service connection type
     * \param   connectionType  The connection type to convert
     **/
    AREG_API const char * getServiceConnectionTypeString( NERemoteService::eServiceConnection connectionType );
    /**
     * \brief   NERemoteService::GetServiceConnectionType
     *          Converts string to connection type value.
     * \param   connectionType  The string of connection type name to convert
     * \param   caseSensitive   Flag, indicating whether the given name to convert is case sensitive or not.
     **/
    AREG_API NERemoteService::eServiceConnection getServiceConnectionType( const char * connectionType, bool caseSensitive = true );
}
