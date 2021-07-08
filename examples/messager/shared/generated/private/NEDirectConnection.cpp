//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     04.07.2021  04:30:02 GMT+02:00 
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
// NEDirectConnection functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectConnection::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NEDirectConnection::MSG_ID_requestConnectoinSetup     ) // requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_requestAddParticipant      ) // requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_requestRemoveParticipant   ) // requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_requestCloseConnection     ) // requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseConnectoinSetup    ) // responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseAddParticipant     ) // responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseRemoveParticipant  ) // responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEDirectConnection::MSG_ID_InitiatedConnections       ) // NEDirectConnection::MapParticipants mInitiatedConnections;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseConnectoinSetup    ) // requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseAddParticipant     ) // requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_responseRemoveParticipant  ) // requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::MSG_ID_NO_PROCEED                 ) // requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
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
    
    /************************************************************************
     * DirectConnection Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEDirectConnection::ServiceName
        , NEDirectConnection::InterfaceVersion
        , NEService::ServiceRemote
        , 4
        , 3
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEDirectConnection::eMessageIDs NEDirectConnection::getResponseId( NEDirectConnection::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NEDirectConnection::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectConnection::MSG_ID_INVALID );
}

NEDirectConnection::eMessageIDs NEDirectConnection::getRequestId( NEDirectConnection::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEDirectConnection::getInterfaceData();
    NEDirectConnection::eMessageIDs result = NEDirectConnection::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEDirectConnection::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestList[i]) : NEDirectConnection::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////
