#ifndef AREG_IPC_CONNECTIONCONFIGURATION_HPP
#define AREG_IPC_CONNECTIONCONFIGURATION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionConfigurator.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Containers.hpp"
#include "areg/base/String.hpp"
#include "areg/persist/Property.hpp"
#include "areg/ipc/NERemoteService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class FileBase;

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
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   List of configuration properties
     **/
    using ListProperties        = TEArrayList<Property>;
    /**
     * \brief   Map of configuration type and list of configuration properties.
     * \tparam  NERemoteService::eServiceConnection         The section / type of configuration.
     * \tparam  ConnectionConfiguration::ListProperties   The list of configuration properties.
     **/
    using MapConfiguration		= TEMap<NERemoteService::eServiceConnection, ListProperties>;

    /**
     * \brief   Connection configuration property indexes.
     *          Each of property has fixed index.
     **/
    typedef enum E_ConnectionProperty : int
    {
          PropertyInvalid   = NECommon::INVALID_INDEX
        , PropertyEnabled   = 0 //!< Index of property remote connection is enabled / disabled
        , PropertyName          //!< Index of property remote connection service name
        , PropertyHost          //!< Index of property remote connection host address
        , PropertyPort          //!< Index of property remote connection port number

        , PropertyLen           //!< Total length of connection properties list. Not used as property index.

    } eConnectionProperty;

    /**
     * \brief   By given property name, return the index of property in the property list.
     * \param   strProperty     The name of property.
     * \return  If passed property name is recognized and valid, returns valid property entry index
     *          in the list. Otherwise, returns NECommon::INVALID_INDEX value.
     **/
    static eConnectionProperty getPosition( const String & strProperty );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection configuration object and internals.
     **/
    ConnectionConfiguration( void ) = default;
    /**
     * \brief   Destructor.
     **/
    virtual ~ConnectionConfiguration( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Loads configuration from given file path.
     * \param   filePath    The relative or absolute file path of configuration file to load properties.
     *                      If nullptr or empty, loads default configuration file (NEApplication::DEFAULT_ROUTER_CONFIG_FILE)
     *\return   Returns true if succeeded to load file and configure properties.
     **/
    bool loadConfiguration( const char * filePath = nullptr );

    /**
     * \brief   Loads configuration from given opened file.
     * \param   file        The file object to read data. The file should exist and be opened for reading.
     *\return   Returns true, if succeeded to load file and configure properties.
     *          Returns false, if file was not opened for reading or the could not parse configuration file.
     **/
    bool loadConfiguration( FileBase & file );

    /**
     * \brief   Returns connection enabled / disabled flag of given connection section.
     * \param   section     The connection section, which property is requested.
     *                      By default, it is NERemoteService::ConnectionTcpip section, which is simple TCP/IP connection.
     * \return  Returns true, if connection for specified section is enabled.
     *          Returns false, if connection for specified section is disabled.
     *          Returns NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED, if the property of specified connection does not exist, i.e. invalid.
     **/
    bool getConnectionEnableFlag( NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionTcpip ) const;

    /**
     * \brief   Returns connection port number of given connection section.
     * \param   section     The connection section, which property is requested.
     *                      By default, it is NERemoteService::ConnectionTcpip section, which is simple TCP/IP connection.
     * \return  Returns the port number of specified connection section.
     *          Returns NEConnection::DEFAULT_REMOTE_SERVICE_PORT, if the property of specified connection does not exist, i.e. invalid.
     *          If returns invalid port, the remote connection port number is set, but the connection is disabled.
     **/
    unsigned short getConnectionPort( NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionTcpip ) const;

    /**
     * \brief   Returns connection name of given connection section. The property is optional and is used to differentiate
     *          different connection sections by name.
     * \param   section     The connection section, which property is requested.
     *                      By default, it is NERemoteService::ConnectionTcpip section, which is simple TCP/IP connection.
     * \return  Returns connection name of specified connection section.
     *          Returns empty string, if the property of specified connection does not exist, i.e. invalid.
     **/
    String getConnectionName( NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionTcpip ) const;

    /**
     * \brief   Returns connection host address (IP address of host name) of given connection section.
     * \param   section     The connection section, which property is requested.
     *                      By default, it is NERemoteService::ConnectionTcpip section, which is simple TCP/IP connection.
     * \return  Returns connection host name of specified connection section.
     *          Returns NEConnection::DEFAULT_REMOTE_SERVICE_HOST, if the property of specified connection does not exist, i.e. invalid.
     **/
    String getConnectionHost( NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionTcpip ) const;

    /**
     * \brief   Returns byte sets of connection host IP address of given connection section.
     * \param   section     The connection section, which property is requested.
     *                      By default, it is NERemoteService::ConnectionTcpip section, which is simple TCP/IP connection.
     * \return  Returns true if the property of specified section is valid and all 4 bytes of IP address are set.
     *          Returns IP address of NEConnection::DEFAULT_REMOTE_SERVICE_HOST, if the property of specified connection does not exist, i.e. invalid.
     **/
    bool getConnectionHostIpAddress( unsigned char & OUT field0
                                   , unsigned char & OUT field1
                                   , unsigned char & OUT field2
                                   , unsigned char & OUT field3
                                   , NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionTcpip);

    /**
     * \brief   Returns configuration file full path.
     **/
    inline const String & getConfigFileName( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Parses single line of connection configuration. Returns valid section type
     *          if could find appropriate property entry.
     * \param   file        The file object, which is opened for reading.
     * \param   inLine      The line of configuration to parse. The passed parameter is modified,
     *                      until could not find the appropriate section type entry in configuration.
     * \param   outProp     On output, this parameter contains appropriate connection section property data.
     *\return   Returns connection section type if found any entry. Otherwise, returns invalid section value.
     **/
    NERemoteService::eServiceConnection _parseConnectionProperty( FileBase & file, String & inLine, Property & outProp );

    /**
     * \brief   Parses single line of connection configuration and initializes appropriate property data.
     * \param   listProp    The list of properties to modify.
     * \param   file        The file object, which is opened for reading.
     * \param   inLine      The line of configuration to parse. Continues in the loop until succeeded to find valid property entry.
     * \param   headModule  The name of module (executable) to parse configuration.
     **/
    bool _parseConnectionConfig(ListProperties & listProp, FileBase & file, String & inLine, const String & headModule );

    /**
     * \brief   Returns the value as string of appropriate property index of specified section.
     * \param   key             The section type to return value.
     * \param   entryIndex      The index of requested property. See eConnectionProperty.
     * \param   defaultValue    The default value to return, in case if property is not set and invalid.
     * \return  Returns the value of specified property or specified default value if configuration is invalid.
     **/
    String _getPropertyValue( NERemoteService::eServiceConnection key, ConnectionConfiguration::eConnectionProperty entryIndex, const char * defaultValue ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
private:
    String            mConfigFile;    //!< The full path of configuration file

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    MapConfiguration    mMapConfig;     //!< The configuration map where the keys are configuration section types and value is property list.

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ConnectionConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration class inline methods
//////////////////////////////////////////////////////////////////////////
inline const String & ConnectionConfiguration::getConfigFileName( void ) const
{
    return mConfigFile;
}

#endif  // AREG_IPC_CONNECTIONCONFIGURATION_HPP
