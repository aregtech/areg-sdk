//////////////////////////////////////////////////////////////////////////
// Begin generate generated/NEConnectionManager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_NECONNECTIONMANAGER_HPP
#define  GENERATED_NECONNECTIONMANAGER_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/NEConnectionManager.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "generated/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg//component/NERegistry.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

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
     * \brief   Service Interface Name.
     *          NEConnectionManager::ServiceName
     **/
    constexpr   char    ServiceName[]   { "ConnectionManager" };
    /**
     * \brief   Implemented Version.
     *          NEConnectionManager::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   Indicates invalid cookie value
     *          NEConnectionManager::InvalidCookie
     **/
    constexpr unsigned int    InvalidCookie       = NECommon::InvalidCookie;

    /**
     * \brief   The maximum length of nick name to register.
     *          NEConnectionManager::NicknameMaxLen
     **/
    constexpr unsigned int    NicknameMaxLen      = NECommon::MaxlenNickname;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The result of connection
     *          NEConnectionManager::eConnectionResult
     **/
    enum class eConnectionResult
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
     typedef    NECommon::sConnection          sConnection;

    /**
     * \brief   List of connections.
     *          NEConnectionManager::ListConnection
     **/
     typedef    NECommon::ListConnections      ListConnection;

    /**
     * \brief   Map of connections.
     *          NEConnectionManager::MapConnection
     **/
     typedef    NECommon::MapConnections       MapConnection;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEConnectionManager::eMessageIDs
     *          ConnectionManager Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestConnect
         *          The request to connect to system
         **/
          MsgId_requestConnect                = NEService::REQUEST_ID_FIRST   // void requestConnect( const String & nickName, const DateTime & dateTime );
        /**
         * \brief   Request ID: MsgId_requestRegisterConnection
         *          Request to register direct connection service.
         **/
        , MsgId_requestRegisterConnection                                       // void requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister );
        /**
         * \brief   Request ID: MsgId_requestDisconnect
         *          Request to disconnect.
         **/
        , MsgId_requestDisconnect                                               // void requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseConnect
         *          The result of connection request.
         **/
        , MsgId_responseConnect               = NEService::RESPONSE_ID_FIRST  // void responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result );
        /**
         * \brief   Response ID: MsgId_responseRegisterConnection
         *          The result of request to register direct connection service.
         **/
        , MsgId_responseRegisterConnection                                      // void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastConnectionUpdated
         *          Notifies all client components the connection has been updated.
         **/
        , MsgId_broadcastConnectionUpdated                                      // void broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );
        /**
         * \brief   Broadcast ID: MsgId_broadcastClientConnected
         *          Notifies that a new client has been connected.
         **/
        , MsgId_broadcastClientConnected                                        // void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
        /**
         * \brief   Broadcast ID: MsgId_broadcastClientDisconnected
         *          Notifies a client has been disconnected.
         **/
        , MsgId_broadcastClientDisconnected                                     // void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ConnectionList
         *          The map of all active connections
         **/
        , MsgId_ConnectionList                = NEService::ATTRIBUTE_ID_FIRST // NEConnectionManager::MapConnection mConnectionList;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed                  = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                       = NEService::RESPONSE_ID_NONE

    };
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
    const NEService::SInterfaceData & getInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs getResponseId( NEConnectionManager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEConnectionManager::eMessageIDs getRequestId( NEConnectionManager::eMessageIDs respId );
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
    case    NEConnectionManager::eConnectionResult::ConnectionAccepted:
        return "NEConnectionManager::eConnectionResult::ConnectionAccepted";
    case    NEConnectionManager::eConnectionResult::ConnectionClientExist:
        return "NEConnectionManager::eConnectionResult::ConnectionClientExist";
    case    NEConnectionManager::eConnectionResult::ConnectionNameReserved:
        return "NEConnectionManager::eConnectionResult::ConnectionNameReserved";
    case    NEConnectionManager::eConnectionResult::ConnectionClosed:
        return "NEConnectionManager::eConnectionResult::ConnectionClosed";
    case    NEConnectionManager::eConnectionResult::InvalidClient:
        return "NEConnectionManager::eConnectionResult::InvalidClient";
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
    case    NEConnectionManager::eMessageIDs::MsgId_requestConnect:
        return "NEConnectionManager::eMessageIDs::MsgId_requestConnect";
    case    NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection:
        return "NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection";
    case    NEConnectionManager::eMessageIDs::MsgId_requestDisconnect:
        return "NEConnectionManager::eMessageIDs::MsgId_requestDisconnect";
    case    NEConnectionManager::eMessageIDs::MsgId_responseConnect:
        return "NEConnectionManager::eMessageIDs::MsgId_responseConnect";
    case    NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection:
        return "NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection";
    case    NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated:
        return "NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated";
    case    NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected:
        return "NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected";
    case    NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected:
        return "NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected";
    case    NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
        return "NEConnectionManager::eMessageIDs::MsgId_ConnectionList";

    case NEConnectionManager::eMessageIDs::MsgId_NotProcessed:
        return "NEConnectionManager::eMessageIDs::MsgId_NotProcessed";
    case NEConnectionManager::eMessageIDs::MsgId_Invalid:
        return "NEConnectionManager::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEConnectionManager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_NECONNECTIONMANAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/NEConnectionManager.hpp file
//////////////////////////////////////////////////////////////////////////
