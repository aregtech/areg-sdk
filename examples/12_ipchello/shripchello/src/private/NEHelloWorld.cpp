//////////////////////////////////////////////////////////////////////////
// Begin generate shripchello/src/private/NEHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     04.07.2021  04:21:09 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shripchello/src/private/NEHelloWorld.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shripchello/src/NEHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NEHelloWorld::MSG_ID_requestHelloWorld            ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NEHelloWorld::MSG_ID_requestClientShutdown        ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NEHelloWorld::MSG_ID_responseHelloWorld           ) // responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo )
        , static_cast<unsigned int>( NEHelloWorld::MSG_ID_broadcastHelloClients        ) // broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList )
        , static_cast<unsigned int>( NEHelloWorld::MSG_ID_broadcastServiceUnavailable  ) // broadcastServiceUnavailable( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEHelloWorld::MSG_ID_ConnectedClients             ) // NEHelloWorld::ConnectionList mConnectedClients;
        , static_cast<unsigned int>( NEHelloWorld::MSG_ID_RemainOutput                 ) // short mRemainOutput;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NEHelloWorld::MSG_ID_responseHelloWorld           ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NEHelloWorld::MSG_ID_NO_PROCEED                   ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        1, // void broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList )
        0, // void broadcastServiceUnavailable( void )

    };
    
    /************************************************************************
     * HelloWorld Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEHelloWorld::ServiceName
        , NEHelloWorld::InterfaceVersion
        , NEService::ServiceRemote
        , 2
        , 3
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEHelloWorld::eMessageIDs NEHelloWorld::getResponseId( NEHelloWorld::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NEHelloWorld::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEHelloWorld::MSG_ID_INVALID );
}

NEHelloWorld::eMessageIDs NEHelloWorld::getRequestId( NEHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEHelloWorld::getInterfaceData();
    NEHelloWorld::eMessageIDs result = NEHelloWorld::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEHelloWorld::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NEHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NEHelloWorld::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shripchello/src/private/NEHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
