//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NEDirectConnection.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NEDIRECTCONNECTION_HPP
#define  SHARED_GENERATED_NEDIRECTCONNECTION_HPP

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     03.09.2019  02:48:09 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NEDirectConnection.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "shared/NECommonSettings.hpp"
#include "areg/base/CEString.hpp"

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
    const CEVersion           InterfaceVersion    = "1.0.0";

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
    const CEString            ServicePrefixStub   = "ChatStub";

    /**
     * \brief   The prefix when instantiate chat proxy.
     *          NEDirectConnection::ServicePrefixClient
     **/
    const CEString            ServicePrefixClient = "ChatClient";

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
         * \brief   Request ID: MSG_ID_RequestConnectoinSetup
         *          Request to setup the direct connection to chat room initiator.
         **/
          MSG_ID_RequestConnectoinSetup       = NEService::REQUEST_ID_FIRST     // void RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_RequestAddParticipant
         *          The request to add a participant in the direct chat-room.
         **/
        , MSG_ID_RequestAddParticipant                                          // void RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_RequestRemoveParticipant
         *          Request to remove initiator from chat-room
         **/
        , MSG_ID_RequestRemoveParticipant                                       // void RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Request ID: MSG_ID_RequestCloseConnection
         *          Request to close chat-room. There is no response expected.
         **/
        , MSG_ID_RequestCloseConnection                                         // void RequestCloseConnection( const NEDirectConnection::sInitiator & initiator );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_ResponseConnectoinSetup
         *          The response of connection setup
         **/
        , MSG_ID_ResponseConnectoinSetup      = NEService::RESPONSE_ID_FIRST    // void ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MSG_ID_ResponseAddParticipant
         *          The response to add initiator to chat room
         **/
        , MSG_ID_ResponseAddParticipant                                         // void ResponseAddParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );
        /**
         * \brief   Response ID: MSG_ID_ResponseRemoveParticipant
         *          Response to remove initiator from chat-room
         **/
        , MSG_ID_ResponseRemoveParticipant                                      // void ResponseRemoveParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );     
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
    inline const char * const GetString( const NEDirectConnection::eMessageIDs msgId );
     
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
    NEDirectConnection::eMessageIDs GetResponseId( const NEDirectConnection::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectConnection::eMessageIDs GetRequestId( const NEDirectConnection::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_RequestConnectoinSetup
        , MSG_ID_RequestAddParticipant
        , MSG_ID_RequestRemoveParticipant
        , MSG_ID_RequestCloseConnection
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
          MSG_ID_ResponseConnectoinSetup
        , MSG_ID_ResponseAddParticipant
        , MSG_ID_ResponseRemoveParticipant

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
          MSG_ID_InitiatedConnections
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

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
inline const char * const NEDirectConnection::GetString( const NEDirectConnection::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEDirectConnection::MSG_ID_RequestConnectoinSetup:
        return "NEDirectConnection::MSG_ID_RequestConnectoinSetup";
    case    NEDirectConnection::MSG_ID_RequestAddParticipant:
        return "NEDirectConnection::MSG_ID_RequestAddParticipant";
    case    NEDirectConnection::MSG_ID_RequestRemoveParticipant:
        return "NEDirectConnection::MSG_ID_RequestRemoveParticipant";
    case    NEDirectConnection::MSG_ID_RequestCloseConnection:
        return "NEDirectConnection::MSG_ID_RequestCloseConnection";
    case    NEDirectConnection::MSG_ID_ResponseConnectoinSetup:
        return "NEDirectConnection::MSG_ID_ResponseConnectoinSetup";
    case    NEDirectConnection::MSG_ID_ResponseAddParticipant:
        return "NEDirectConnection::MSG_ID_ResponseAddParticipant";
    case    NEDirectConnection::MSG_ID_ResponseRemoveParticipant:
        return "NEDirectConnection::MSG_ID_ResponseRemoveParticipant";
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
