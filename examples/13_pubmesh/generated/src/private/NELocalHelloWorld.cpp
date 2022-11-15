//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NELocalHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  13:59:46 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/NELocalHelloWorld.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NELocalHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NELocalHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NELocalHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld   ) // requestHelloWorld( const String & roleName )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld  ) // responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NELocalHelloWorld::eMessageIDs::MsgId_ConnectedClients    ) // NELocalHelloWorld::ConnectionList mConnectedClients;
        , static_cast<unsigned int>( NELocalHelloWorld::eMessageIDs::MsgId_RemainOutput        ) // short mRemainOutput;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld  ) // requestHelloWorld( const String & roleName )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * LocalHelloWorld Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NELocalHelloWorld::ServiceName
        , NELocalHelloWorld::InterfaceVersion
        , NEService::eServiceType::ServiceLocal
        , 1
        , 1
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NELocalHelloWorld::eMessageIDs NELocalHelloWorld::getResponseId( NELocalHelloWorld::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NELocalHelloWorld::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NELocalHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NELocalHelloWorld::eMessageIDs::MsgId_Invalid );
}

NELocalHelloWorld::eMessageIDs NELocalHelloWorld::getRequestId( NELocalHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NELocalHelloWorld::getInterfaceData();
    NELocalHelloWorld::eMessageIDs result = NELocalHelloWorld::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NELocalHelloWorld::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NELocalHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NELocalHelloWorld::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NELocalHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
