//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     12.05.2021  16:41:24 GMT+02:00 
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
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   DirectMessager Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NEDirectMessager::ServiceName;
        _InterfaceData.idVersion        = NEDirectMessager::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceRemote;
        _InterfaceData.idRequestCount   = NEDirectMessager::NumberofRequests;
        _InterfaceData.idResponseCount  = NEDirectMessager::NumberofResponses;
        _InterfaceData.idAttributeCount = NEDirectMessager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEDirectMessager::MSG_ID_responseChatJoin            ) // requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
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

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectMessager::createInterfaceData( void )
{
    return _getInterfaceData();
}

NEDirectMessager::eMessageIDs NEDirectMessager::getResponseId( NEDirectMessager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectMessager::MSG_ID_INVALID );
}

NEDirectMessager::eMessageIDs NEDirectMessager::getRequestId( NEDirectMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NEDirectMessager::eMessageIDs result = NEDirectMessager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEDirectMessager::MSG_ID_INVALID && i < NEDirectMessager::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NEDirectMessager::RequestIds[i] : NEDirectMessager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////
