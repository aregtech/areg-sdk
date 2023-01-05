//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:07:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/NEHelloWorld.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld        ) // requestHelloWorld( const String & roleName )
        , static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_requestShutdownService   ) // requestShutdownService( void )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld       ) // responseHelloWorld( void )
        , static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum  ) // broadcastReachedMaximum( int maxNumber )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    constexpr unsigned int * _AttributeList { nullptr };    /* There are no attributes. Set Invalid message ID */

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld       ) // requestHelloWorld( const String & roleName )
        , static_cast<unsigned int>( NEHelloWorld::eMessageIDs::MsgId_NotProcessed             ) // requestShutdownService( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        0, // void responseHelloWorld( void )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        1, // void broadcastReachedMaximum( int maxNumber )

    };

    /************************************************************************
     * HelloWorld Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEHelloWorld::ServiceName
        , NEHelloWorld::InterfaceVersion
        , NEService::eServiceType::ServiceLocal
        , 2
        , 2
        , 0
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEHelloWorld::eMessageIDs NEHelloWorld::getResponseId( NEHelloWorld::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEHelloWorld::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEHelloWorld::eMessageIDs::MsgId_Invalid );
}

NEHelloWorld::eMessageIDs NEHelloWorld::getRequestId( NEHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEHelloWorld::getInterfaceData();
    NEHelloWorld::eMessageIDs result = NEHelloWorld::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEHelloWorld::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NEHelloWorld::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
