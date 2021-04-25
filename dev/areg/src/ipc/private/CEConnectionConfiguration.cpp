/************************************************************************
 * \file        areg/src/ipc/private/CEConnectionConfiguration.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Remote service connection configuration 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/ipc/CEConnectionConfiguration.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/ipc/private/NEConnection.hpp"

int CEConnectionConfiguration::getPosition(const CEString & strProperty)
{
    if ( strProperty == NERemoteService::CONFIG_KEY_PROP_ENABLE)
        return PropertyEnabled;
    else if ( strProperty == NERemoteService::CONFIG_KEY_PROP_NAME )
        return PropertyName;
    else if (strProperty == NERemoteService::CONFIG_KEY_PROP_ADDRESS )
        return PropertyHost;
    else if (strProperty == NERemoteService::CONFIG_KEY_PROP_PORT )
        return PropertyPort;
    else
        return NECommon::InvalidIndex;
}

CEConnectionConfiguration::CEConnectionConfiguration( void )
    : mConfigFile   ( )
    , mMapConfig    ( )
{
}

CEConnectionConfiguration::~CEConnectionConfiguration( void )
{
}

bool CEConnectionConfiguration::LoadConfiguration(const char * filePath)
{
    mConfigFile = CEFile::GetFileFullPath( NEString::isEmpty<char>(filePath) == false ? filePath : NERemoteService::DEFAULT_CONFIG_FILE );
    CEFile fileConfig( static_cast<const char *>(mConfigFile), CEFileBase::FO_MODE_EXIST | CEFileBase::FO_MODE_READ | CEFileBase::FO_MODE_TEXT | CEFileBase::FO_MODE_SHARE_READ );
    fileConfig.Open( );

    return LoadConfiguration(fileConfig);
}

bool CEConnectionConfiguration::LoadConfiguration(CEFileBase & file)
{
    mMapConfig.RemoveAll();

    if (file.IsOpened())
    {
        mConfigFile = file.GetName( );

        CEString line;
        CEProperty prop;

        NERemoteService::eServiceConnection section = NERemoteService::ConnectionUndefined;
        do
        {
            section = parseConnectionProperty( file, line, prop );
            if (section != NERemoteService::ConnectionUndefined)
            {
                ListProperties  listProp;
                if ( parseConnectionConfig(listProp, file, line, prop.GetValue().GetString()) )
                    mMapConfig.SetKey(section, listProp);
                else
                    section = NERemoteService::ConnectionUndefined;
            }
        } while (section != NERemoteService::ConnectionUndefined);
    }
    return (mMapConfig.IsEmpty() == false);
}

NERemoteService::eServiceConnection CEConnectionConfiguration::parseConnectionProperty(CEFileBase & file, CEString & inLine, CEProperty & outProp )
{
    NERemoteService::eServiceConnection result = NERemoteService::ConnectionUndefined;
    do
    {
        CEProperty prop;
        if (prop.ParseProperty(inLine))
        {
            outProp = prop;
            const CEPropertyKey & key   = prop.GetKey();
            const CEString & section    = key.GetSection();
            const CEString & target     = key.GetProperty();

            if ( (section   == NERemoteService::CONFIG_KEY_CONNECTION   ) && 
                 (target    == NERemoteService::CONFIG_KEY_PROP_TYPE    ) )
            {
                result = NERemoteService::GetServiceConnectionType(prop.GetValue().GetString(), false);
            }
            break;
        }
    } while (file.ReadLine( inLine ) > 0 );
    return result;
}

bool CEConnectionConfiguration::parseConnectionConfig(ListProperties & listProp, CEFileBase & file, CEString & inLine, const CEString & headModule )
{
    listProp.RemoveAll();
    listProp.ReserveElements( static_cast<int>(CEConnectionConfiguration::PropertyLen) );

    CEProperty prop;
    while ( (file.ReadLine( inLine ) > 0) && (parseConnectionProperty(file, inLine, prop) == NERemoteService::ConnectionUndefined) )
    {
        const CEPropertyKey & key = prop.GetKey( );
        if ( (key.GetSection( ) == NERemoteService::CONFIG_KEY_CONNECTION) && (key.GetModule( ) == headModule) )
        {
            int pos = getPosition( key.GetProperty( ) );
            if ( pos != NECommon::InvalidIndex )
                listProp[pos] = prop;
        }
        prop.ResetData();
    }
    return (listProp.IsEmpty() == false);
}

CEString CEConnectionConfiguration::getPropertyValue( NERemoteService::eServiceConnection key, CEConnectionConfiguration::eConnectionProperty entryIndex, const char * defaultValue ) const
{
    CEString result = defaultValue;
    if ( mMapConfig.IsEmpty( ) == false )
    {
        const ListProperties & listProp = mMapConfig.GetAt( key );
        if ( listProp.GetSize( ) > static_cast<int>(entryIndex) )
        {
            const CEProperty & prop = listProp[static_cast<int>(entryIndex)];
            result = prop.GetValue( ).GetString( );
        }
    }
    return result;
}

CEString CEConnectionConfiguration::GetConnectionName( NERemoteService::eServiceConnection section /*= NERemoteService::ConnectionTcpip */ ) const
{
    return getPropertyValue( section, CEConnectionConfiguration::PropertyName, CEString::EmptyString );
}

CEString CEConnectionConfiguration::GetConnectionHost( NERemoteService::eServiceConnection section /*= NERemoteService::ConnectionTcpip */ ) const
{
    return getPropertyValue( section, CEConnectionConfiguration::PropertyHost, NEConnection::DEFAULT_REMOTE_SERVICE_HOST );
}

bool CEConnectionConfiguration::GetConnectionEnabled( NERemoteService::eServiceConnection section /*= NERemoteService::ConnectionTcpip */ ) const
{
    CEString enabled = getPropertyValue( section, CEConnectionConfiguration::PropertyEnabled, NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED ? CEString::BOOLEAN_TRUE : CEString::BOOLEAN_FALSE);
    return enabled.ToBoolean();
}

unsigned short CEConnectionConfiguration::GetConnectionPort( NERemoteService::eServiceConnection section /*= NERemoteService::ConnectionTcpip */ ) const
{
    CEString port = getPropertyValue( section, CEConnectionConfiguration::PropertyPort, CEString::UInt32ToString(NEConnection::DEFAULT_REMOTE_SERVICE_PORT) );
    return static_cast<unsigned short>(port.ToUInt32( ));
}

bool CEConnectionConfiguration::GetConnectionHostIpAddress( unsigned char & field0, unsigned char & field1, unsigned char & field2, unsigned char & field3, NERemoteService::eServiceConnection section /*= NERemoteService::ConnectionTcpip*/ )
{
    bool result = false;
    field0 = field1 = field2 = field3 = 0u;

    CEString addr = GetConnectionHost(section);
    if ( addr.IsEmpty() == false )
    {
        const char * buffer = addr.String( );
        const char * next   = NULL;

        uint32_t f0 = CEString::StringToUInt32(buffer, NEString::RadixDecimal, &next);
        if ( (buffer != next) && (f0 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
        {
            buffer = next + 1;
            uint32_t f1 = CEString::StringToUInt32( buffer, NEString::RadixDecimal, &next );
            if ( (buffer != next) && (f1 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
            {
                buffer = next + 1;
                uint32_t f2 = CEString::StringToUInt32( buffer, NEString::RadixDecimal, &next );
                if ( (buffer != next) && (f2 <= 0xFFu) && (*next == NERemoteService::IP_SEPARATOR) )
                {
                    buffer = next + 1;
                    uint32_t f3 = CEString::StringToUInt32( buffer, NEString::RadixDecimal, &next );
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
