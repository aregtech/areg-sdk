//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/NEDirectConnection.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/NEDirectConnection.hpp"

//////////////////////////////////////////////////////////////////////////
// NEDirectConnection functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEDirectConnection::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup     ) // requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant      ) // requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant   ) // requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection     ) // requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup    ) // responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant     ) // responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant  ) // responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections       ) // NEDirectConnection::MapParticipants mInitiatedConnections;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup    ) // requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant     ) // requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant  ) // requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
        , static_cast<unsigned int>( NEDirectConnection::eMessageIDs::MsgId_NotProcessed               ) // requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
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
        , NEService::eServiceType::ServicePublic
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

NEDirectConnection::eMessageIDs NEDirectConnection::getResponseId( NEDirectConnection::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEDirectConnection::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEDirectConnection::eMessageIDs::MsgId_Invalid );
}

NEDirectConnection::eMessageIDs NEDirectConnection::getRequestId( NEDirectConnection::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEDirectConnection::getInterfaceData();
    NEDirectConnection::eMessageIDs result = NEDirectConnection::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEDirectConnection::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEDirectConnection::eMessageIDs>(sid.idRequestList[i]) : NEDirectConnection::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NEDirectConnection.cpp file
//////////////////////////////////////////////////////////////////////////
