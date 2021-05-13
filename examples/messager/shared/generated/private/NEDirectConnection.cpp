//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     12.05.2021  16:41:23 GMT+02:00 
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
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NEDirectConnection::ServiceName;
        _InterfaceData.idVersion        = NEDirectConnection::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceRemote;
        _InterfaceData.idRequestCount   = NEDirectConnection::NumberofRequests;
        _InterfaceData.idResponseCount  = NEDirectConnection::NumberofResponses;
        _InterfaceData.idAttributeCount = NEDirectConnection::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseConnectoinSetup    ) // requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseAddParticipant     ) // requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseRemoveParticipant  ) // requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            , static_cast<unsigned int>( NEDirectConnection::MSG_ID_NO_PROCEED                 ) // requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int _ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            4, // void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
            2, // void responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
            2, // void responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )

        /************************************************************************
         * Broadcasts
         ************************************************************************/

        };

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectConnection::createInterfaceData( void )
{
    return _getInterfaceData();
}

NEDirectConnection::eMessageIDs NEDirectConnection::getResponseId( NEDirectConnection::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectConnection::MSG_ID_INVALID );
}

NEDirectConnection::eMessageIDs NEDirectConnection::getRequestId( NEDirectConnection::eMessageIDs respId )
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
