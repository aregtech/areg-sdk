//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NEDirectConnection.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NEDIRECTCONNECTION_HPP
#define  SHARED_GENERATED_NEDIRECTCONNECTION_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     04.07.2021  04:30:02 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NEDirectConnection.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Namespace declaration.
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
     * \brief   Service Interface Name
     *          NEDirectConnection::ServiceName
     **/
    const char * const        ServiceName         = "DirectConnection";
    /**
     * \brief   Implemented Version
     *          NEDirectConnection::InterfaceVersion
     **/
    const Version             InterfaceVersion    = "1.0.0";

    /**
     * \brief   Indicates Invalid session
     *          NEDirectConnection::InvalidSession
     **/
    const unsigned long long  InvalidSession      = NECommonSettings::InvalidSession;

    /**
     * \brief   Indicates Invalid cookie
     *          NEDirectConnection::InvalidCookie
     **/
    const unsigned int        InvalidCookie       = NECommonSettings::InvalidCookie;

    /**
     * \brief   The prefix to add when create stub instance
     *          NEDirectConnection::ServicePrefixStub
     **/
    const String              ServicePrefixStub   = "ChatStub";

    /**
     * \brief   The prefix when instantiate chat proxy.
     *          NEDirectConnection::ServicePrefixClient
     **/
    const String              ServicePrefixClient = "ChatClient";

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Localizing Participant structure
     *          NEDirectConnection::sParticipant
     **/
     typedef    NECommonSettings::sParticipant         sParticipant;

    /**
     * \brief   Localizing Initiator structure
     *          NEDirectConnection::sInitiator
     **/
     typedef    NECommonSettings::sInitiator           sInitiator;

    /**
     * \brief   Localizing the list of participants
     *          NEDirectConnection::ListParticipants
     **/
     typedef    NECommonSettings::ListParticipants     ListParticipants;

    /**
     * \brief   Localizing the map of participants
     *          NEDirectConnection::MapParticipants
     **/
     typedef    NECommonSettings::MapParticipants      MapParticipants;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEDirectConnection::eMessageIDs
     *          DirectConnection Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestConnectoinSetup
         *          Request to setup the direct connection to chat room initiator.
         **/
          MSG_ID_requestConnectoinSetup       = NEService::REQUEST_ID_FIRST     // void requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_requestAddParticipant
         *          The request to add a participant in the direct chat-room.
         **/
        , MSG_ID_requestAddParticipant                                          // void requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_requestRemoveParticipant
         *          Request to remove initiator from chat-room
         **/
        , MSG_ID_requestRemoveParticipant                                       // void requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_requestCloseConnection
         *          Request to close chat-room. There is no response expected.
         **/
        , MSG_ID_requestCloseConnection                                         // void requestCloseConnection( const NEDirectConnection::sInitiator & initiator );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseConnectoinSetup
         *          The response of connection setup
         **/
        , MSG_ID_responseConnectoinSetup      = NEService::RESPONSE_ID_FIRST    // void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MSG_ID_responseAddParticipant
         *          The response to add initiator to chat room
         **/
        , MSG_ID_responseAddParticipant                                         // void responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MSG_ID_responseRemoveParticipant
         *          Response to remove initiator from chat-room
         **/
        , MSG_ID_responseRemoveParticipant                                      // void responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_InitiatedConnections
         *          The map of initiated connections and list of participants.
         **/
        , MSG_ID_InitiatedConnections         = NEService::ATTRIBUTE_ID_FIRST   // NEDirectConnection::MapParticipants mInitiatedConnections;
     
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
    case    NEDirectConnection::MSG_ID_requestConnectoinSetup:
        return "NEDirectConnection::MSG_ID_requestConnectoinSetup";
    case    NEDirectConnection::MSG_ID_requestAddParticipant:
        return "NEDirectConnection::MSG_ID_requestAddParticipant";
    case    NEDirectConnection::MSG_ID_requestRemoveParticipant:
        return "NEDirectConnection::MSG_ID_requestRemoveParticipant";
    case    NEDirectConnection::MSG_ID_requestCloseConnection:
        return "NEDirectConnection::MSG_ID_requestCloseConnection";
    case    NEDirectConnection::MSG_ID_responseConnectoinSetup:
        return "NEDirectConnection::MSG_ID_responseConnectoinSetup";
    case    NEDirectConnection::MSG_ID_responseAddParticipant:
        return "NEDirectConnection::MSG_ID_responseAddParticipant";
    case    NEDirectConnection::MSG_ID_responseRemoveParticipant:
        return "NEDirectConnection::MSG_ID_responseRemoveParticipant";
    case    NEDirectConnection::MSG_ID_InitiatedConnections:
        return "NEDirectConnection::MSG_ID_InitiatedConnections";

    case NEDirectConnection::MSG_ID_NO_PROCEED:
        return "NEDirectConnection::MSG_ID_NO_PROCEED";
        
    case NEDirectConnection::MSG_ID_INVALID:
        return "NEDirectConnection::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEDirectConnection::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_NEDIRECTCONNECTION_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/NEDirectConnection.hpp file
//////////////////////////////////////////////////////////////////////////
