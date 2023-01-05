//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/NEDirectMessager.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/NEDirectMessager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectMessager::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_requestChatJoin             ) // requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_requestMessageSend          ) // requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_requestMessageType          ) // requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_requestChatLeave            ) // requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_responseChatJoin            ) // responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent        ) // broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped       ) // broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined  ) // broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft    ) // broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed         ) // broadcastChatClosed( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_ChatParticipants            ) // NEDirectMessager::ListParticipants mChatParticipants;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_responseChatJoin            ) // requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_NotProcessed                ) // requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_NotProcessed                ) // requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
        , static_cast<unsigned int>( NEDirectMessager::eMessageIDs::MsgId_NotProcessed                ) // requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
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
        , NEService::eServiceType::ServicePublic
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

NEDirectMessager::eMessageIDs NEDirectMessager::getResponseId( NEDirectMessager::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEDirectMessager::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectMessager::eMessageIDs::MsgId_Invalid );
}

NEDirectMessager::eMessageIDs NEDirectMessager::getRequestId( NEDirectMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEDirectMessager::getInterfaceData();
    NEDirectMessager::eMessageIDs result = NEDirectMessager::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEDirectMessager::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestList[i]) : NEDirectMessager::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////
