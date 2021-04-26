//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/NEDirectConnection.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shared/generated/NEDirectConnection.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   DirectConnection Service Interface data
     **/
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NEDirectConnection::ServiceName;
        InterfaceData.idVersion         = NEDirectConnection::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceRemote;
        InterfaceData.idRequestCount    = NEDirectConnection::NumberofRequests;
        InterfaceData.idResponseCount   = NEDirectConnection::NumberofResponses;
        InterfaceData.idAttributeCount  = NEDirectConnection::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEDirectConnection::MSG_ID_ResponseConnectoinSetup    ) // RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_ResponseAddParticipant     ) // RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_ResponseRemoveParticipant  ) // RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_NO_PROCEED                 ) // RequestCloseConnection( const NEDirectConnection::sInitiator & initiator )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            4, // void ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            2, // void ResponseAddParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants )
            2, // void ResponseRemoveParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants )

        /************************************************************************
         * Broadcasts
         ************************************************************************/

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectConnection::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NEDirectConnection::eMessageIDs NEDirectConnection::GetResponseId( NEDirectConnection::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectConnection::MSG_ID_INVALID );
}

NEDirectConnection::eMessageIDs NEDirectConnection::GetRequestId( NEDirectConnection::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NEDirectConnection::eMessageIDs result = NEDirectConnection::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEDirectConnection::MSG_ID_INVALID && i < NEDirectConnection::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NEDirectConnection::RequestIds[i] : NEDirectConnection::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////
