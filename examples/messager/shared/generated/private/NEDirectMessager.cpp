//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     25.04.2021  23:50:46 GMT+02:00 
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
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NEDirectMessager::ServiceName;
        InterfaceData.idVersion         = NEDirectMessager::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceRemote;
        InterfaceData.idRequestCount    = NEDirectMessager::NumberofRequests;
        InterfaceData.idResponseCount   = NEDirectMessager::NumberofResponses;
        InterfaceData.idAttributeCount  = NEDirectMessager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEDirectMessager::MSG_ID_ResponseChatJoin            ) // RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
            , static_cast<unsigned int>( NEDirectMessager::MSG_ID_NO_PROCEED                  ) // RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            4, // void ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected )

        /************************************************************************
         * Broadcasts
         ************************************************************************/
            3, // void BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
            2, // void BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
            2, // void BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined )
            2, // void BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft )
            0, // void BroadcastChatClosed( void )

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectMessager::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NEDirectMessager::eMessageIDs NEDirectMessager::GetResponseId( NEDirectMessager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectMessager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectMessager::MSG_ID_INVALID );
}

NEDirectMessager::eMessageIDs NEDirectMessager::GetRequestId( NEDirectMessager::eMessageIDs respId )
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
