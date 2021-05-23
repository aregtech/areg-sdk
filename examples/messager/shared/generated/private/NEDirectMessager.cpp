//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     23.05.2021  00:18:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/NEDirectMessager.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shared/generated/NEDirectMessager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectMessager::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NEDirectMessager::MSG_ID_requestChatJoin             ) // requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_requestMessageSend          ) // requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_requestMessageType          ) // requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_requestChatLeave            ) // requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NEDirectMessager::MSG_ID_responseChatJoin            ) // responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_broadcastMessageSent        ) // broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_broadcastMessageTyped       ) // broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_broadcastParticipantJoined  ) // broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_broadcastParticipantLeft    ) // broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_broadcastChatClosed         ) // broadcastChatClosed( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEDirectMessager::MSG_ID_ChatParticipants            ) // NEDirectMessager::ListParticipants mChatParticipants;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NEDirectMessager::MSG_ID_responseChatJoin            ) // requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        4, // void responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        3, // void broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        2, // void broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
        2, // void broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
        2, // void broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
        0, // void broadcastChatClosed( void )

    };
    
    /************************************************************************
     * DirectMessager Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEDirectMessager::ServiceName
        , NEDirectMessager::InterfaceVersion
        , NEService::ServiceRemote
        , 4
        , 6
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEDirectMessager::eMessageIDs NEDirectMessager::getResponseId( NEDirectMessager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NEDirectMessager::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectMessager::MSG_ID_INVALID );
}

NEDirectMessager::eMessageIDs NEDirectMessager::getRequestId( NEDirectMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEDirectMessager::getInterfaceData();
    NEDirectMessager::eMessageIDs result = NEDirectMessager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEDirectMessager::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestList[i]) : NEDirectMessager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////
