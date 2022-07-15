/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ConnectionConfiguration.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/base/File.hpp"
#include "areg/ipc/NEConnection.hpp"

ConnectionConfiguration::eConnectionProperty ConnectionConfiguration::getPosition(const String & strProperty)
{
    if ( strProperty == NERemoteService::CONFIG_KEY_PROP_ENABLE.data( ) )
        return eConnectionProperty::PropertyEnabled;
    else if ( strProperty == NERemoteService::CONFIG_KEY_PROP_NAME.data( ) )
        return eConnectionProperty::PropertyName;
    else if (strProperty == NERemoteService::CONFIG_KEY_PROP_ADDRESS.data( ) )
        return eConnectionProperty::PropertyHost;
    else if (strProperty == NERemoteService::CONFIG_KEY_PROP_PORT.data( ) )
        return eConnectionProperty::PropertyPort;
    else
        return eConnectionProperty::PropertyInvalid;
}

bool ConnectionConfiguration::loadConfiguration(const char * filePath /* = nullptr */)
{
    mConfigFile = File::getFileFullPath( NEString::isEmpty<char>(filePath) ? NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() : filePath );
    File fileConfig( static_cast<const char *>(mConfigFile), FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
    fileConfig.open( );

    return loadConfiguration(fileConfig);
}

bool ConnectionConfiguration::loadConfiguration(FileBase & file)
{
    mMapConfig.clear();

    if (file.isOpened())
    {
        mConfigFile = file.getName( );

        String line;
        Property prop;

        NERemoteService::eServiceConnection section = NERemoteService::eServiceConnection::ConnectionUndefined;
        do
        {
            section = _parseConnectionProperty( file, line, prop );
            if (section != NERemoteService::eServiceConnection::ConnectionUndefined)
            {
                ListProperties  listProp;
                if ( _parseConnectionConfig(listProp, file, line, prop.getValue().getString()) )
                    mMapConfig.setAt(section, listProp);
                else
                    section = NERemoteService::eServiceConnection::ConnectionUndefined;
            }
        } while (section != NERemoteService::eServiceConnection::ConnectionUndefined);
    }
    return (mMapConfig.isEmpty() == false);
}

NERemoteService::eServiceConnection ConnectionConfiguration::_parseConnectionProperty(FileBase & file, String & inLine, Property & outProp )
{
    NERemoteService::eServiceConnection result = NERemoteService::eServiceConnection::ConnectionUndefined;
    do
    {
        Property prop;
        if (prop.parseProperty(inLine))
        {
            outProp = prop;
            const PropertyKey & key   = prop.getKey();
            const String & section    = key.getSection();
            const String & target     = key.getProperty();

            if ( (section   == NERemoteService::CONFIG_KEY_CONNECTION.data( )   ) &&
                 (target    == NERemoteService::CONFIG_KEY_PROP_TYPE.data( )    ) )
            {
                result = NERemoteService::getServiceConnectionType(prop.getValue().getString(), false);
            }
            break;
        }
    } while (file.readLine( inLine ) > 0 );
    return result;
}

bool ConnectionConfiguration::_parseConnectionConfig(ListProperties & listProp, FileBase & file, String & inLine, const String & headModule )
{
    listProp.clear();
    listProp.resize( static_cast<int>(ConnectionConfiguration::PropertyLen) );

    Property prop;
    while ( (file.readLine( inLine ) > 0) && (_parseConnectionProperty(file, inLine, prop) == NERemoteService::eServiceConnection::ConnectionUndefined) )
    {
        const PropertyKey & key = prop.getKey( );
        if ( (key.getSection( ) == NERemoteService::CONFIG_KEY_CONNECTION.data()) && (key.getModule( ) == headModule) )
        {
            eConnectionProperty pos = getPosition( key.getProperty( ) );
            if ( pos != eConnectionProperty::PropertyInvalid )
            {
                listProp[static_cast<uint32_t>(pos)] = prop;
            }
        }
        prop.resetData();
    }
    return (listProp.isEmpty() == false);
}

String ConnectionConfiguration::_getPropertyValue( NERemoteService::eServiceConnection key, ConnectionConfiguration::eConnectionProperty entryIndex, const char * defaultValue ) const
{
    String result = defaultValue;
    if ( mMapConfig.isEmpty( ) == false )
    {
        const ListProperties & listProp = mMapConfig.getAt( key );
        if ( listProp.getSize( ) > static_cast<uint32_t>(entryIndex) )
        {
            const Property & prop = listProp[static_cast<uint32_t>(entryIndex)];
            result = prop.getValue( ).getString( );
        }
    }
    return result;
}

String ConnectionConfiguration::getConnectionName( NERemoteService::eServiceConnection section /*= NERemoteService::eServiceConnection::ConnectionTcpip */ ) const
{
    return _getPropertyValue( section, ConnectionConfiguration::PropertyName, String::EmptyString.data() );
}

String ConnectionConfiguration::getConnectionHost( NERemoteService::eServiceConnection section /*= NERemoteService::eServiceConnection::ConnectionTcpip */ ) const
{
    return _getPropertyValue( section, ConnectionConfiguration::eConnectionProperty::PropertyHost, NEConnection::DEFAULT_REMOTE_SERVICE_HOST.data( ) );
}

bool ConnectionConfiguration::getConnectionEnableFlag( NERemoteService::eServiceConnection section /*= NERemoteService::eServiceConnection::ConnectionTcpip */ ) const
{
    String enabled = _getPropertyValue( section
                                      , ConnectionConfiguration::eConnectionProperty::PropertyEnabled
                                      , NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED ? NECommon::BOOLEAN_TRUE.data() : NECommon::BOOLEAN_FALSE.data());
    return enabled.toBool();
}

unsigned short ConnectionConfiguration::getConnectionPort( NERemoteService::eServiceConnection section /*= NERemoteService::eServiceConnection::ConnectionTcpip */ ) const
{
    String port = _getPropertyValue( section, ConnectionConfiguration::PropertyPort, String::toString(NEConnection::DEFAULT_REMOTE_SERVICE_PORT) );
    return static_cast<unsigned short>(port.toUInt32( ));
}

bool ConnectionConfiguration::getConnectionHostIpAddress( unsigned char & OUT field0
                                                        , unsigned char & OUT field1
                                                        , unsigned char & OUT field2
                                                        , unsigned char & OUT field3
                                                        , NERemoteService::eServiceConnection section /*= NERemoteService::eServiceConnection::ConnectionTcpip*/ )
{
    bool result = false;
    field0 = field1 = field2 = field3 = 0u;

    String addr = getConnectionHost(section);
    if ( addr.isEmpty() == false )
    {
        const char * buffer = addr.getString( );
        const char * next   = nullptr;

        uint32_t f0 = String::makeUInt32(buffer, NEString::eRadix::RadixDecimal, &next);
        if ( (buffer != next) && (f0 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
        {
            buffer = next + 1;
            uint32_t f1 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
            if ( (buffer != next) && (f1 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
            {
                buffer = next + 1;
                uint32_t f2 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
                if ( (buffer != next) && (f2 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
                {
                    buffer = next + 1;
                    uint32_t f3 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
                    if ( (buffer != next) && (f3 <= 0xFFu) )
                    {
                        field0 = static_cast<unsigned char>(f0);
                        field1 = static_cast<unsigned char>(f1);
                        field2 = static_cast<unsigned char>(f2);
                        field3 = static_cast<unsigned char>(f3);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}
