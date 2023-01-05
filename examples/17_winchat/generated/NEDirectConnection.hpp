//////////////////////////////////////////////////////////////////////////
// Begin generate generated/NEDirectConnection.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_NEDIRECTCONNECTION_HPP
#define  GENERATED_NEDIRECTCONNECTION_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/NEDirectConnection.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
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
// NEDirectConnection namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   DirectConnection Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Direct Connection service interface. It is used to send connection messages 
 *          to distributed applications. As soon as the distributed applications are connected, 
 *          each application has one instance of implemented service where 
 *          the role name is a nick name of a registered application.
 *
 **/
namespace   NEDirectConnection
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEDirectConnection::ServiceName
     **/
    constexpr   char    ServiceName[]   { "DirectConnection" };
    /**
     * \brief   Implemented Version.
     *          NEDirectConnection::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   Indicates Invalid session
     *          NEDirectConnection::InvalidSession
     **/
    constexpr unsigned long long  InvalidSession      = NECommon::InvalidSession;

    /**
     * \brief   Indicates Invalid cookie
     *          NEDirectConnection::InvalidCookie
     **/
    constexpr unsigned int        InvalidCookie       = NECommon::InvalidCookie;

    /**
     * \brief   The prefix to add when create stub instance
     *          NEDirectConnection::ServicePrefixStub
     **/
    const     String              ServicePrefixStub   = "ChatStub";

    /**
     * \brief   The prefix when instantiate chat proxy.
     *          NEDirectConnection::ServicePrefixClient
     **/
    const     String              ServicePrefixClient = "ChatClient";

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Localizing Participant structure
     *          NEDirectConnection::sParticipant
     **/
     typedef    NECommon::sParticipant         sParticipant;

    /**
     * \brief   Localizing Initiator structure
     *          NEDirectConnection::sInitiator
     **/
     typedef    NECommon::sInitiator           sInitiator;

    /**
     * \brief   Localizing the list of participants
     *          NEDirectConnection::ListParticipants
     **/
     typedef    NECommon::ListParticipants     ListParticipants;

    /**
     * \brief   Localizing the map of participants
     *          NEDirectConnection::MapParticipants
     **/
     typedef    NECommon::MapParticipants      MapParticipants;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEDirectConnection::eMessageIDs
     *          DirectConnection Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestConnectoinSetup
         *          Request to setup the direct connection to chat room initiator.
         **/
          MsgId_requestConnectoinSetup    = NEService::REQUEST_ID_FIRST   // void requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MsgId_requestAddParticipant
         *          The request to add a participant in the direct chat-room.
         **/
        , MsgId_requestAddParticipant                                       // void requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MsgId_requestRemoveParticipant
         *          Request to remove initiator from chat-room
         **/
        , MsgId_requestRemoveParticipant                                    // void requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MsgId_requestCloseConnection
         *          Request to close chat-room. There is no response expected.
         **/
        , MsgId_requestCloseConnection                                      // void requestCloseConnection( const NEDirectConnection::sInitiator & initiator );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseConnectoinSetup
         *          The response of connection setup
         **/
        , MsgId_responseConnectoinSetup   = NEService::RESPONSE_ID_FIRST  // void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MsgId_responseAddParticipant
         *          The response to add initiator to chat room
         **/
        , MsgId_responseAddParticipant                                      // void responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MsgId_responseRemoveParticipant
         *          Response to remove initiator from chat-room
         **/
        , MsgId_responseRemoveParticipant                                   // void responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_InitiatedConnections
         *          The map of initiated connections and list of participants.
         **/
        , MsgId_InitiatedConnections      = NEService::ATTRIBUTE_ID_FIRST // NEDirectConnection::MapParticipants mInitiatedConnections;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed              = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                   = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEDirectConnection::eMessageIDs
     **/
    inline const char * getString( NEDirectConnection::eMessageIDs msgId );

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
    NEDirectConnection::eMessageIDs getResponseId( NEDirectConnection::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectConnection::eMessageIDs getRequestId( NEDirectConnection::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEDirectConnection methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEDirectConnection::eMessageIDs );

/**
 * Returns string value of NEDirectConnection::eMessageIDs
 **/
inline const char * NEDirectConnection::getString( NEDirectConnection::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup:
        return "NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup";
    case    NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant:
        return "NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant";
    case    NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant:
        return "NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant";
    case    NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection:
        return "NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection";
    case    NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup:
        return "NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup";
    case    NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant:
        return "NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant";
    case    NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant:
        return "NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant";
    case    NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
        return "NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections";

    case NEDirectConnection::eMessageIDs::MsgId_NotProcessed:
        return "NEDirectConnection::eMessageIDs::MsgId_NotProcessed";
    case NEDirectConnection::eMessageIDs::MsgId_Invalid:
        return "NEDirectConnection::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEDirectConnection::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_NEDIRECTCONNECTION_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/NEDirectConnection.hpp file
//////////////////////////////////////////////////////////////////////////
