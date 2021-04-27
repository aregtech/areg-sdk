#ifndef AREG_COMPONENT_PRIVATE_CESERVICEMANAGEREVENTS_HPP
#define AREG_COMPONENT_PRIVATE_CESERVICEMANAGEREVENTS_HPP
/************************************************************************
 * \file        areg/component/private/CEServiceManagerEvents.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager communication Events
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/CESharedBuffer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEProxyAddress;
class CEStubAddress;
class CEChannel;
class CEString;

//////////////////////////////////////////////////////////////////////////
// CEServiceManagerEventData class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Service Manager event data object, which contains broker
 *          commands to execute and appropriate data for command
 **/
class CEServiceManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The Service Manager event commands
     **/
    typedef enum E_ServiceManagerCommands
    {
          CMD_StopRoutingClient         //!< Requested to stop Service Manager client connection
        , CMD_RegisterProxy             //!< Requested to register Proxy
        , CMD_UnregisterProxy           //!< Requested to unregister Proxy
        , CMD_RegisterStub              //!< Requested to register Stub
        , CMD_UnregisterStub            //!< Requested to unregister Stub
        , CMD_ConfigureConnection       //!< Requested to configure connection
        , CMD_StartConnection           //!< Requested to start connection, the data is configuration file
        , CMD_StartNetConnection        //!< Requested to start network connection, the data is IP-address and port number
        , CMD_StopConnection            //!< Requested to stop connection
        , CMD_SetEnableService          //!< Requested to enables or disables remote service
        , CMD_RegisterConnection        //!< Requested to register connection
        , CMD_UnregisterConnection      //!< Requested to unregister connection
        , CMD_LostConnection            //!< Requested to send notifications to components that connection is lost.
    } eServiceManagerCommands;

    /**
     * \brief   Converts event command to string
     * \param   cmdBroker   The command to convert
     **/
    static const char * GetString( CEServiceManagerEventData::eServiceManagerCommands cmdBroker );

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and returns Service Manager event data with command to stop Broker client connection
     **/
    static CEServiceManagerEventData StopMessageRouterClient( void );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Proxy
     **/
    static CEServiceManagerEventData RegisterProxy( const CEProxyAddress & addrProxy );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Proxy
     **/
    static CEServiceManagerEventData UnregisterProxy( const CEProxyAddress & addrProxy );

    /**
     * \brief   Creates and returns Service Manager event data with command to register Stub
     **/
    static CEServiceManagerEventData RegisterStub( const CEStubAddress & addrStub );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister Stub
     **/
    static CEServiceManagerEventData UnregisterStub( const CEStubAddress & addrStub );

    /**
     * \brief   Creates and returns Service Manager event data with command to configure connection
     **/
    static CEServiceManagerEventData ConfigureConnection( const CEString & configFile );

    /**
     * \brief   Creates and returns Service Manager event data with command to start Broker client connection
     **/
    static CEServiceManagerEventData StartConnection( const CEString & configFile );

    /**
     * \brief   Creates and returns Service Manager event data with command to start Broker client connection.
     * \param   ipAddress   The IP-Address of remote Routing Service.
     * \param   portNr      The port number to connect.
     **/
    static CEServiceManagerEventData StartNetConnection( const CEString & ipAddress, unsigned short portNr );

    /**
     * \brief   Creates and returns Service Manager event data with command to stop connection
     **/
    static CEServiceManagerEventData StopConnection( void );

    /**
     * \brief   Creates Service Manager event data with command to enable or disable remote service.
     *          If disabled, services cannot be accessed outside of process.
     *          If remote service is already running and it is requested to disable, it will be stopped.
     * \param  enable   If true, the remote services are accessed from outside of process.
     *                  If false, all services run and accessed only within process.
     * \return  Returns Service Manager Event data object with command to enable or disable remote service.
     **/
    static CEServiceManagerEventData EnableRemoteService( bool enable );

    /**
     * \brief   Creates and returns Service Manager event data with command to register connection
     * \param   channel     The Channel information to set for registered connection
     **/
    static CEServiceManagerEventData RegisterConnection( const CEChannel & channel );

    /**
     * \brief   Creates and returns Service Manager event data with command to unregister connection
     * \param   channel     The Channel information to set for unregistered connection
     **/
    static CEServiceManagerEventData UnregisterConnection( const CEChannel & channel );

    /**
     * \brief   Creates and returns Service Manager event data with command of lost connection
     * \param   channel     The Channel information to set for lost connection
     **/
    static CEServiceManagerEventData LostConnection( const CEChannel & channel );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor. Sets Service Manager event command
     * \param   cmdServiceManager    The command to set
     **/
    inline CEServiceManagerEventData( CEServiceManagerEventData::eServiceManagerCommands cmdServiceManager );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline CEServiceManagerEventData( const CEServiceManagerEventData & source );

    /**
     * \brief   Destructor.
     **/
    inline ~CEServiceManagerEventData( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Service Manager event data with comment from given source.
     * \param   source  The source of data to copy
     **/
    inline void operator = ( const CEServiceManagerEventData & source );

    /**
     * \brief   Returns streaming object to read data
     **/
    inline const IEInStream & GetReadStream( void ) const;

    /**
     * \brief   Returns streaming object to write data
     **/
    inline IEOutStream & GetWriteStream( void );

    /**
     * \brief   Returns Service Manager command
     **/
    inline CEServiceManagerEventData::eServiceManagerCommands GetCommand( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    eServiceManagerCommands mCommand;   //!< Service Manager event data command
    CESharedBuffer          mStream;    //!< Service Manager event data streaming object to write and read data.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServiceManagerEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// Service Manager event and consumer declaration
//////////////////////////////////////////////////////////////////////////
DECLARE_EVENT(CEServiceManagerEventData, CEServiceManagerEvent, IEServiceManagerEventConsumer)

//////////////////////////////////////////////////////////////////////////
// CEServiceManagerEventData inline methods
//////////////////////////////////////////////////////////////////////////
inline CEServiceManagerEventData::CEServiceManagerEventData( CEServiceManagerEventData::eServiceManagerCommands cmdServiceManager )
    : mCommand  ( cmdServiceManager )
    , mStream   ( )
{   ;   }

inline CEServiceManagerEventData::CEServiceManagerEventData( const CEServiceManagerEventData & source )
    : mCommand  ( source.mCommand )
    , mStream   ( source.mStream )
{   ;   }

inline CEServiceManagerEventData::~CEServiceManagerEventData( void )
{   ;   }

inline void CEServiceManagerEventData::operator = (const CEServiceManagerEventData & source)
{
    mCommand= source.mCommand;
    mStream = source.mStream;
}

inline const IEInStream & CEServiceManagerEventData::GetReadStream( void ) const
{   return static_cast<const IEInStream &>(mStream);        }

inline IEOutStream & CEServiceManagerEventData::GetWriteStream( void )
{   return static_cast<IEOutStream &>(mStream);             }

inline CEServiceManagerEventData::eServiceManagerCommands CEServiceManagerEventData::GetCommand( void ) const
{   return mCommand;                                        }

inline const char * CEServiceManagerEventData::GetString( CEServiceManagerEventData::eServiceManagerCommands cmdBroker )
{
    switch ( cmdBroker )
    {
    case CEServiceManagerEventData::CMD_StopRoutingClient:
        return "CEServiceManagerEventData::CMD_StopRoutingClient";
    case CEServiceManagerEventData::CMD_RegisterProxy:
        return "CEServiceManagerEventData::CMD_RegisterProxy";
    case CEServiceManagerEventData::CMD_UnregisterProxy:
        return "CEServiceManagerEventData::CMD_UnregisterProxy";
    case CEServiceManagerEventData::CMD_RegisterStub:
        return "CEServiceManagerEventData::CMD_RegisterStub";
    case CEServiceManagerEventData::CMD_UnregisterStub:
        return "CEServiceManagerEventData::CMD_UnregisterStub";
    case CEServiceManagerEventData::CMD_ConfigureConnection:
        return "CEServiceManagerEventData::CMD_ConfigureConnection";
    case CEServiceManagerEventData::CMD_StartConnection:
        return "CEServiceManagerEventData::CMD_StartConnection";
    case CEServiceManagerEventData::CMD_StartNetConnection:
        return "CEServiceManagerEventData::CMD_StartNetConnection";
    case CEServiceManagerEventData::CMD_StopConnection:
        return "CEServiceManagerEventData::CMD_StopConnection";
    case CEServiceManagerEventData::CMD_SetEnableService:
        return "CEServiceManagerEventData::CMD_SetEnableService";
    case CEServiceManagerEventData::CMD_RegisterConnection:
        return "CEServiceManagerEventData::CMD_RegisterConnection";
    case CEServiceManagerEventData::CMD_UnregisterConnection:
        return "CEServiceManagerEventData::CMD_UnregisterConnection";
    case CEServiceManagerEventData::CMD_LostConnection:
        return "CEServiceManagerEventData::CMD_LostConnection";
    default:
        return "ERR: undefined CEServiceManagerEventData::eServiceManagerCommands value!!!";
    }
}
#endif  // AREG_COMPONENT_PRIVATE_CESERVICEMANAGEREVENTS_HPP
