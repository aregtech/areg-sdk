//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NEConnectionManager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NECONNECTIONMANAGER_HPP
#define  SHARED_GENERATED_NECONNECTIONMANAGER_HPP

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     03.09.2019  02:48:08 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NEConnectionManager.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "shared/NECommonSettings.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/CEDateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/CEVersion.hpp"
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
    const CEVersion       InterfaceVersion    = "1.0.0";

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
    inline const char * const GetString( const NEConnectionManager::eConnectionResult enumValue );

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
         * \brief   Request ID: MSG_ID_RequestConnet
         *          The request to connect to system
         **/
          MSG_ID_RequestConnet                = NEService::REQUEST_ID_FIRST     // void RequestConnet( const CEString & nickName, const CEDateTime & dateTime );
        /**
         * \brief   Request ID: MSG_ID_RequestRegisterConnection
         *          Request to register direct connection service.
         **/
        , MSG_ID_RequestRegisterConnection                                      // void RequestRegisterConnection( const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister );
        /**
         * \brief   Request ID: MSG_ID_RequestDiconnect
         *          Request to disconnect.
         **/
        , MSG_ID_RequestDiconnect                                               // void RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_ResponseConnect
         *          The result of connection request.
         **/
        , MSG_ID_ResponseConnect              = NEService::RESPONSE_ID_FIRST    // void ResponseConnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result );
        /**
         * \brief   Response ID: MSG_ID_ResponseRegisterConnection
         *          The result of request to register direct connection service.
         **/
        , MSG_ID_ResponseRegisterConnection                                     // void ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastConnectionUpdated
         *          Notifies all client components the connection has been updated.
         **/
        , MSG_ID_BroadcastConnectionUpdated                                     // void BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastClientConnected
         *          Notifies that a new client has been connected.
         **/
        , MSG_ID_BroadcastClientConnected                                       // void BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastClientDisconnected
         *          Notifies a client has been disconnected.
         **/
        , MSG_ID_BroadcastClientDisconnected                                    // void BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );
     
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
    inline const char * const GetString( const NEConnectionManager::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & CreateInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs GetResponseId( const NEConnectionManager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs GetRequestId( const NEConnectionManager::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_RequestConnet
        , MSG_ID_RequestRegisterConnection
        , MSG_ID_RequestDiconnect
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
          MSG_ID_ResponseConnect
        , MSG_ID_ResponseRegisterConnection

        , MSG_ID_BroadcastConnectionUpdated
        , MSG_ID_BroadcastClientConnected
        , MSG_ID_BroadcastClientDisconnected
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
IMPLEMENT_STREAMABLE( NEConnectionManager::eConnectionResult );

/**
 * Return string value of NEConnectionManager::eConnectionResult
 **/
inline const char * const NEConnectionManager::GetString( const NEConnectionManager::eConnectionResult enumValue )
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
inline const char * const NEConnectionManager::GetString( const NEConnectionManager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEConnectionManager::MSG_ID_RequestConnet:
        return "NEConnectionManager::MSG_ID_RequestConnet";
    case    NEConnectionManager::MSG_ID_RequestRegisterConnection:
        return "NEConnectionManager::MSG_ID_RequestRegisterConnection";
    case    NEConnectionManager::MSG_ID_RequestDiconnect:
        return "NEConnectionManager::MSG_ID_RequestDiconnect";
    case    NEConnectionManager::MSG_ID_ResponseConnect:
        return "NEConnectionManager::MSG_ID_ResponseConnect";
    case    NEConnectionManager::MSG_ID_ResponseRegisterConnection:
        return "NEConnectionManager::MSG_ID_ResponseRegisterConnection";
    case    NEConnectionManager::MSG_ID_BroadcastConnectionUpdated:
        return "NEConnectionManager::MSG_ID_BroadcastConnectionUpdated";
    case    NEConnectionManager::MSG_ID_BroadcastClientConnected:
        return "NEConnectionManager::MSG_ID_BroadcastClientConnected";
    case    NEConnectionManager::MSG_ID_BroadcastClientDisconnected:
        return "NEConnectionManager::MSG_ID_BroadcastClientDisconnected";
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
