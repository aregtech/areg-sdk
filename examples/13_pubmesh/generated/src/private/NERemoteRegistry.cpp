//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NERemoteRegistry.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:11 GMT+02:00
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/NERemoteRegistry.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface namespace implementation.
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
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_requestRegister     ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_requestUnregister   ) // requestUnregister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld   ) // requestHelloWorld( unsigned int clientID, const String & addMessage )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_responseRegister    ) // responseRegister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld  ) // responseHelloWorld( unsigned int clientID )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_RegistryList        ) // NERemoteRegistry::ListRegistry mRegistryList;
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs       ) // unsigned int mRemainOutputs;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_responseRegister    ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_NotProcessed        ) // requestUnregister( const NERemoteRegistry::sClientRegister & client )
        , static_cast<unsigned int>( NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld  ) // requestHelloWorld( unsigned int clientID, const String & addMessage )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
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
        , NEService::eServiceType::ServicePublic
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

NERemoteRegistry::eMessageIDs NERemoteRegistry::getResponseId( NERemoteRegistry::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NERemoteRegistry::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NERemoteRegistry::eMessageIDs>(sid.idRequestToResponseMap[index]) : NERemoteRegistry::eMessageIDs::MsgId_Invalid );
}

NERemoteRegistry::eMessageIDs NERemoteRegistry::getRequestId( NERemoteRegistry::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NERemoteRegistry::getInterfaceData();
    NERemoteRegistry::eMessageIDs result = NERemoteRegistry::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NERemoteRegistry::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NERemoteRegistry::eMessageIDs>(sid.idRequestList[i]) : NERemoteRegistry::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NERemoteRegistry.cpp file
//////////////////////////////////////////////////////////////////////////
