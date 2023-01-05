//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEHelloWatchdog.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/NEHelloWatchdog.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEHelloWatchdog.hpp"

//////////////////////////////////////////////////////////////////////////
// NEHelloWatchdog functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEHelloWatchdog::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep       ) // requestStartSleep( unsigned int timeoutSleep )
        , static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_requestStopService      ) // requestStopService( void )
        , static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService  ) // requestShutdownService( void )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep      ) // responseStartSleep( unsigned int timeoutSleep )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_ServiceState            ) // NEHelloWatchdog::eState mServiceState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep      ) // requestStartSleep( unsigned int timeoutSleep )
        , static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed            ) // requestStopService( void )
        , static_cast<unsigned int>( NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed            ) // requestShutdownService( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseStartSleep( unsigned int timeoutSleep )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * HelloWatchdog Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEHelloWatchdog::ServiceName
        , NEHelloWatchdog::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 3
        , 1
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEHelloWatchdog::eMessageIDs NEHelloWatchdog::getResponseId( NEHelloWatchdog::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEHelloWatchdog::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEHelloWatchdog::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEHelloWatchdog::eMessageIDs::MsgId_Invalid );
}

NEHelloWatchdog::eMessageIDs NEHelloWatchdog::getRequestId( NEHelloWatchdog::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEHelloWatchdog::getInterfaceData();
    NEHelloWatchdog::eMessageIDs result = NEHelloWatchdog::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEHelloWatchdog::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEHelloWatchdog::eMessageIDs>(sid.idRequestList[i]) : NEHelloWatchdog::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEHelloWatchdog.cpp file
//////////////////////////////////////////////////////////////////////////
