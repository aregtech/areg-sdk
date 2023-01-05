//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEHelloUnblock.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/NEHelloUnblock.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEHelloUnblock.hpp"

//////////////////////////////////////////////////////////////////////////
// NEHelloUnblock functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEHelloUnblock::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier     ) // requestIdentifier( void )
        , static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock    ) // requestHelloUblock( unsigned int clientId, unsigned int seqNr )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier    ) // responseIdentifier( unsigned int clientId )
        , static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock  ) // responseHelloUnblock( unsigned int clientId, unsigned int seqNr )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState     ) // NEHelloUnblock::eServiceState mHelloServiceState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier    ) // requestIdentifier( void )
        , static_cast<unsigned int>( NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock  ) // requestHelloUblock( unsigned int clientId, unsigned int seqNr )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseIdentifier( unsigned int clientId )
        2, // void responseHelloUnblock( unsigned int clientId, unsigned int seqNr )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * HelloUnblock Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEHelloUnblock::ServiceName
        , NEHelloUnblock::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 2
        , 2
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEHelloUnblock::eMessageIDs NEHelloUnblock::getResponseId( NEHelloUnblock::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEHelloUnblock::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEHelloUnblock::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEHelloUnblock::eMessageIDs::MsgId_Invalid );
}

NEHelloUnblock::eMessageIDs NEHelloUnblock::getRequestId( NEHelloUnblock::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEHelloUnblock::getInterfaceData();
    NEHelloUnblock::eMessageIDs result = NEHelloUnblock::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEHelloUnblock::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEHelloUnblock::eMessageIDs>(sid.idRequestList[i]) : NEHelloUnblock::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEHelloUnblock.cpp file
//////////////////////////////////////////////////////////////////////////
