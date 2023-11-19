//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPubSub.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:18 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/NEPubSub.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEPubSub.hpp"

//////////////////////////////////////////////////////////////////////////
// NEPubSub functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEPubSub::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseList  { nullptr }; /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEPubSub::eMessageIDs::MsgId_StringOnChange        ) // String mStringOnChange;
        , static_cast<unsigned int>( NEPubSub::eMessageIDs::MsgId_IntegerAlways         ) // unsigned int mIntegerAlways;
        , static_cast<unsigned int>( NEPubSub::eMessageIDs::MsgId_ServiceProviderState  ) // NEPubSub::eServiceState mServiceProviderState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseParamStateMap { nullptr }; // EMPTY RESPONSE AND BROADCAST LIST

    /************************************************************************
     * PubSub Service Interface data
     ************************************************************************/
    static const NEService::SInterfaceData _InterfaceData
    {
          NEPubSub::ServiceName
        , NEPubSub::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 0
        , 3
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEPubSub::eMessageIDs NEPubSub::getResponseId( NEPubSub::eMessageIDs /* reqId */ )
{
    return NEPubSub::eMessageIDs::MsgId_Invalid;
}

NEPubSub::eMessageIDs NEPubSub::getRequestId( NEPubSub::eMessageIDs /* respId */ )
{
    return NEPubSub::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPubSub.cpp file
//////////////////////////////////////////////////////////////////////////
