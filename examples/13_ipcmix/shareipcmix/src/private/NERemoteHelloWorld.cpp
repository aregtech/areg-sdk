//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/NERemoteHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteHelloWorld.
 * Generated at     29.05.2021  12:43:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/NERemoteHelloWorld.cpp
 * \ingroup         RemoteHelloWorld Service Interface
 * \brief           This is an automatic generated code of RemoteHelloWorld Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shareipcmix/src/NERemoteHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NERemoteHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NERemoteHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_requestHelloWorld            ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_requestClientShutdown        ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_responseHelloWorld           ) // responseHelloWorld( const NERemoteHelloWorld::sConnectedClient & clientInfo )
        , static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_broadcastHelloClients        ) // broadcastHelloClients( const NERemoteHelloWorld::ConnectionList & clientList )
        , static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable  ) // broadcastServiceUnavailable( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_ConnectedClients             ) // NERemoteHelloWorld::ConnectionList mConnectedClients;
        , static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_RemainOutput                 ) // short mRemainOutput;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_responseHelloWorld           ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NERemoteHelloWorld::MSG_ID_NO_PROCEED                   ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseHelloWorld( const NERemoteHelloWorld::sConnectedClient & clientInfo )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        1, // void broadcastHelloClients( const NERemoteHelloWorld::ConnectionList & clientList )
        0, // void broadcastServiceUnavailable( void )

    };
    
    /************************************************************************
     * RemoteHelloWorld Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NERemoteHelloWorld::ServiceName
        , NERemoteHelloWorld::InterfaceVersion
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

NERemoteHelloWorld::eMessageIDs NERemoteHelloWorld::getResponseId( NERemoteHelloWorld::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NERemoteHelloWorld::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NERemoteHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NERemoteHelloWorld::MSG_ID_INVALID );
}

NERemoteHelloWorld::eMessageIDs NERemoteHelloWorld::getRequestId( NERemoteHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NERemoteHelloWorld::getInterfaceData();
    NERemoteHelloWorld::eMessageIDs result = NERemoteHelloWorld::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NERemoteHelloWorld::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NERemoteHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NERemoteHelloWorld::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/NERemoteHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
