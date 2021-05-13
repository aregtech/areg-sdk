//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NEConnectionManager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NECONNECTIONMANAGER_HPP
#define  SHARED_GENERATED_NECONNECTIONMANAGER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     12.05.2021  16:41:22 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NEConnectionManager.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "shared/NECommonSettings.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   ConnectionManager Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          The connection manager to register each application with client
 *
 **/
namespace   NEConnectionManager
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NEConnectionManager::ServiceName
     **/
    const char * const    ServiceName         = "ConnectionManager";
    /**
     * \brief   Implemented Version
     *          NEConnectionManager::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   Indicates invalid cookie value
     *          NEConnectionManager::InvalidCookie
     **/
    const unsigned int    InvalidCookie       = NECommonSettings::InvalidCookie;

    /**
     * \brief   The maximum length of nick name to register.
     *          NEConnectionManager::NicknameMaxLen
     **/
    const unsigned int    NicknameMaxLen      = NECommonSettings::MaxlenNickname;

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The result of connection
     *          NEConnectionManager::eConnectionResult
     **/
    enum eConnectionResult
    {
        /**
         * \brief   The connection is accepted by system
         **/
          ConnectionAccepted      = 0
        /**
         * \brief   The connection is rejected, client exists
         **/
        , ConnectionClientExist   
        /**
         * \brief   The name is reserved and cannot be used.
         **/
        , ConnectionNameReserved  
        /**
         * \brief   The connection is closed by system.
         **/
        , ConnectionClosed        
        /**
         * \brief   The client is invalid
         **/
        , InvalidClient           
    };
    /**
     * \brief   Returns string value of NEConnectionManager::eConnectionResult
     **/
    inline const char * getString( NEConnectionManager::eConnectionResult enumValue );

    /**
     * \brief   The connection structure.
     *          NEConnectionManager::sConnection
     **/
     typedef    NECommonSettings::sConnection          sConnection;

    /**
     * \brief   List of connections.
     *          NEConnectionManager::ListConnection
     **/
     typedef    NECommonSettings::ListConnections      ListConnection;

    /**
     * \brief   Map of connections.
     *          NEConnectionManager::MapConnection
     **/
     typedef    NECommonSettings::MapConnections       MapConnection;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEConnectionManager::eMessageIDs
     *          ConnectionManager Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestConnet
         *          The request to connect to system
         **/
          MSG_ID_requestConnet                = NEService::REQUEST_ID_FIRST     // void requestConnet( const String & nickName, const DateTime & dateTime );
        /**
         * \brief   Request ID: MSG_ID_requestRegisterConnection
         *          Request to register direct connection service.
         **/
        , MSG_ID_requestRegisterConnection                                      // void requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister );
        /**
         * \brief   Request ID: MSG_ID_requestDiconnect
         *          Request to disconnect.
         **/
        , MSG_ID_requestDiconnect                                               // void requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseConnect
         *          The result of connection request.
         **/
        , MSG_ID_responseConnect              = NEService::RESPONSE_ID_FIRST    // void responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result );
        /**
         * \brief   Response ID: MSG_ID_responseRegisterConnection
         *          The result of request to register direct connection service.
         **/
        , MSG_ID_responseRegisterConnection                                     // void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastConnectionUpdated
         *          Notifies all client components the connection has been updated.
         **/
        , MSG_ID_broadcastConnectionUpdated                                     // void broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastClientConnected
         *          Notifies that a new client has been connected.
         **/
        , MSG_ID_broadcastClientConnected                                       // void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastClientDisconnected
         *          Notifies a client has been disconnected.
         **/
        , MSG_ID_broadcastClientDisconnected                                    // void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_ConnectionList
         *          The map of all active connections
         **/
        , MSG_ID_ConnectionList               = NEService::ATTRIBUTE_ID_FIRST   // NEConnectionManager::MapConnection mConnectionList;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED                   = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID                      = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
    /**
     * \brief   Returns string value of NEConnectionManager::eMessageIDs
     **/
    inline const char * getString( NEConnectionManager::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & createInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs getResponseId( NEConnectionManager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs getRequestId( NEConnectionManager::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_requestConnet
        , MSG_ID_requestRegisterConnection
        , MSG_ID_requestDiconnect
    };
    /**
     * \brief   Number of supported Request entries
     **/
    const unsigned int NumberofRequests     = MACRO_ARRAYLEN(RequestIds);

    /**
     * \brief   Array of supported Response IDs
     **/
    const eMessageIDs    ResponseIds[] = 
    {
          MSG_ID_responseConnect
        , MSG_ID_responseRegisterConnection

        , MSG_ID_broadcastConnectionUpdated
        , MSG_ID_broadcastClientConnected
        , MSG_ID_broadcastClientDisconnected
    };
    /**
     * \brief   Number of supported Response entries
     **/
    const unsigned int  NumberofResponses   = MACRO_ARRAYLEN(ResponseIds);

    /**
     * \brief   Array of supported Attribute IDs
     **/
    const eMessageIDs    AttributeIds[] =
    {
          MSG_ID_ConnectionList
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

}
//////////////////////////////////////////////////////////////////////////
// NEConnectionManager inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEConnectionManager methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEConnectionManager::eMessageIDs );

/************************************************************************
 * NEConnectionManager::eConnectionResult enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEConnectionManager::eConnectionResult )

/**
 * Return string value of NEConnectionManager::eConnectionResult
 **/
inline const char * NEConnectionManager::getString( NEConnectionManager::eConnectionResult enumValue )
{
    switch ( enumValue )
    {
    case    NEConnectionManager::ConnectionAccepted:
        return "NEConnectionManager::ConnectionAccepted";
    case    NEConnectionManager::ConnectionClientExist:
        return "NEConnectionManager::ConnectionClientExist";
    case    NEConnectionManager::ConnectionNameReserved:
        return "NEConnectionManager::ConnectionNameReserved";
    case    NEConnectionManager::ConnectionClosed:
        return "NEConnectionManager::ConnectionClosed";
    case    NEConnectionManager::InvalidClient:
        return "NEConnectionManager::InvalidClient";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEConnectionManager::eConnectionResult value!";
    }
}

/**
 * Returns string value of NEConnectionManager::eMessageIDs
 **/
inline const char * NEConnectionManager::getString( NEConnectionManager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEConnectionManager::MSG_ID_requestConnet:
        return "NEConnectionManager::MSG_ID_requestConnet";
    case    NEConnectionManager::MSG_ID_requestRegisterConnection:
        return "NEConnectionManager::MSG_ID_requestRegisterConnection";
    case    NEConnectionManager::MSG_ID_requestDiconnect:
        return "NEConnectionManager::MSG_ID_requestDiconnect";
    case    NEConnectionManager::MSG_ID_responseConnect:
        return "NEConnectionManager::MSG_ID_responseConnect";
    case    NEConnectionManager::MSG_ID_responseRegisterConnection:
        return "NEConnectionManager::MSG_ID_responseRegisterConnection";
    case    NEConnectionManager::MSG_ID_broadcastConnectionUpdated:
        return "NEConnectionManager::MSG_ID_broadcastConnectionUpdated";
    case    NEConnectionManager::MSG_ID_broadcastClientConnected:
        return "NEConnectionManager::MSG_ID_broadcastClientConnected";
    case    NEConnectionManager::MSG_ID_broadcastClientDisconnected:
        return "NEConnectionManager::MSG_ID_broadcastClientDisconnected";
    case    NEConnectionManager::MSG_ID_ConnectionList:
        return "NEConnectionManager::MSG_ID_ConnectionList";

    case NEConnectionManager::MSG_ID_NO_PROCEED:
        return "NEConnectionManager::MSG_ID_NO_PROCEED";
        
    case NEConnectionManager::MSG_ID_INVALID:
        return "NEConnectionManager::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEConnectionManager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_NECONNECTIONMANAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/NEConnectionManager.hpp file
//////////////////////////////////////////////////////////////////////////
