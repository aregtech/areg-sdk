//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPublicHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:27 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/NEPublicHelloWorld.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEPublicHelloWorld.hpp"

//////////////////////////////////////////////////////////////////////////
// NEPublicHelloWorld functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEPublicHelloWorld::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister     ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister   ) // requestUnregister( const NEPublicHelloWorld::sClientRegister & client )
        , static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld   ) // requestHelloWorld( unsigned int clientID )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister    ) // responseRegister( const NEPublicHelloWorld::sClientRegister & client )
        , static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld  ) // responseHelloWorld( unsigned int clientID )
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
          static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister    ) // requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
        , static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_NotProcessed        ) // requestUnregister( const NEPublicHelloWorld::sClientRegister & client )
        , static_cast<unsigned int>( NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld  ) // requestHelloWorld( unsigned int clientID )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseRegister( const NEPublicHelloWorld::sClientRegister & client )
        1, // void responseHelloWorld( unsigned int clientID )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * PublicHelloWorld Service Interface data
     ************************************************************************/
    static const NEService::SInterfaceData _InterfaceData
    {
          NEPublicHelloWorld::ServiceName
        , NEPublicHelloWorld::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 3
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

NEPublicHelloWorld::eMessageIDs NEPublicHelloWorld::getResponseId( NEPublicHelloWorld::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid { NEPublicHelloWorld::getInterfaceData() };
    msg_id index { static_cast<msg_id>(GET_REQ_INDEX(reqId)) };

    return  ( (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEPublicHelloWorld::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEPublicHelloWorld::eMessageIDs::MsgId_Invalid );
}

NEPublicHelloWorld::eMessageIDs NEPublicHelloWorld::getRequestId( NEPublicHelloWorld::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid { NEPublicHelloWorld::getInterfaceData() };
    NEPublicHelloWorld::eMessageIDs result { NEPublicHelloWorld::eMessageIDs::MsgId_Invalid };
    for ( unsigned int i = 0; (result == NEPublicHelloWorld::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEPublicHelloWorld::eMessageIDs>(sid.idRequestList[i]) : NEPublicHelloWorld::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPublicHelloWorld.cpp file
//////////////////////////////////////////////////////////////////////////
