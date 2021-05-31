//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/NELocalHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     29.05.2021  12:42:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/NELocalHelloWorld.cpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shareipcmix/src/NELocalHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NELocalHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NELocalHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_requestHelloWorld            ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_requestClientShutdown        ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_responseHelloWorld           ) // responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )
        , static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_broadcastHelloClients        ) // broadcastHelloClients( const NELocalHelloWorld::ConnectionList & clientList )
        , static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_broadcastServiceUnavailable  ) // broadcastServiceUnavailable( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_ConnectedClients             ) // NELocalHelloWorld::ConnectionList mConnectedClients;
        , static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_RemainOutput                 ) // short mRemainOutput;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_responseHelloWorld           ) // requestHelloWorld( const String & roleName, const String & addMessage = "" )
        , static_cast<unsigned int>( NELocalHelloWorld::MSG_ID_NO_PROCEED                   ) // requestClientShutdown( unsigned int clientID, const String & roleName )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        1, // void broadcastHelloClients( const NELocalHelloWorld::ConnectionList & clientList )
        0, // void broadcastServiceUnavailable( void )

    };
    
    /************************************************************************
     * LocalHelloWorld Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NELocalHelloWorld::ServiceName
        , NELocalHelloWorld::InterfaceVersion
        , NEService::ServiceLocal
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

NELocalHelloWorld::eMessageIDs NELocalHelloWorld::getResponseId( NELocalHelloWorld::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NELocalHelloWorld::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NELocalHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NELocalHelloWorld::MSG_ID_INVALID );
}

NELocalHelloWorld::eMessageIDs NELocalHelloWorld::getRequestId( NELocalHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NELocalHelloWorld::getInterfaceData();
    NELocalHelloWorld::eMessageIDs result = NELocalHelloWorld::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NELocalHelloWorld::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NELocalHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NELocalHelloWorld::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/NELocalHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
