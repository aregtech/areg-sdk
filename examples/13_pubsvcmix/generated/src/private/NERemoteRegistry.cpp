//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NERemoteRegistry.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.08.2021  13:17:32 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NERemoteRegistry.cpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NERemoteRegistry.hpp"

//////////////////////////////////////////////////////////////////////////
// NERemoteRegistry functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NERemoteRegistry::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NERemoteRegistry::MSG_ID_requestRegister     ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_requestUnregister   ) // requestUnregister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_requestHelloWorld   ) // requestHelloWorld( unsigned int clientID, const String & addMessage )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NERemoteRegistry::MSG_ID_responseRegister    ) // responseRegister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_responseHelloWorld  ) // responseHelloWorld( unsigned int clientID )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NERemoteRegistry::MSG_ID_RegistryList        ) // NERemoteRegistry::ListRegistry mRegistryList;
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_RemainOutputs       ) // unsigned int mRemainOutputs;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NERemoteRegistry::MSG_ID_responseRegister    ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_NO_PROCEED          ) // requestUnregister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::MSG_ID_responseHelloWorld  ) // requestHelloWorld( unsigned int clientID, const String & addMessage )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseRegister( const NERemoteRegistry::sClientRegister & client )
        1, // void responseHelloWorld( unsigned int clientID )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };
    
    /************************************************************************
     * RemoteRegistry Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NERemoteRegistry::ServiceName
        , NERemoteRegistry::InterfaceVersion
        , NEService::ServiceRemote
        , 3
        , 2
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NERemoteRegistry::eMessageIDs NERemoteRegistry::getResponseId( NERemoteRegistry::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NERemoteRegistry::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NERemoteRegistry::eMessageIDs>(sid.idRequestToResponseMap[index]) : NERemoteRegistry::MSG_ID_INVALID );
}

NERemoteRegistry::eMessageIDs NERemoteRegistry::getRequestId( NERemoteRegistry::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NERemoteRegistry::getInterfaceData();
    NERemoteRegistry::eMessageIDs result = NERemoteRegistry::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NERemoteRegistry::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NERemoteRegistry::eMessageIDs>(sid.idRequestList[i]) : NERemoteRegistry::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NERemoteRegistry.cpp file
//////////////////////////////////////////////////////////////////////////
