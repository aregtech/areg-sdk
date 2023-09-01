//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPubSubMix.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     31.08.2023  12:34:15 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/NEPubSubMix.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEPubSubMix.hpp"

//////////////////////////////////////////////////////////////////////////
// NEPubSubMix functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEPubSubMix::getInterfaceData( void )
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
          static_cast<unsigned int>( NEPubSubMix::eMessageIDs::MsgId_StringOnChange        ) // NEPubSubMix::sString mStringOnChange;
        , static_cast<unsigned int>( NEPubSubMix::eMessageIDs::MsgId_IntegerAlways         ) // NEPubSubMix::sInteger mIntegerAlways;
        , static_cast<unsigned int>( NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState  ) // NEPubSubMix::eServiceState mServiceProviderState;
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
     * PubSubMix Service Interface data
     ************************************************************************/
    static const NEService::SInterfaceData _InterfaceData
    {
          NEPubSubMix::ServiceName
        , NEPubSubMix::InterfaceVersion
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

NEPubSubMix::eMessageIDs NEPubSubMix::getResponseId( NEPubSubMix::eMessageIDs /* reqId */ )
{
    return NEPubSubMix::eMessageIDs::MsgId_Invalid;
}

NEPubSubMix::eMessageIDs NEPubSubMix::getRequestId( NEPubSubMix::eMessageIDs /* respId */ )
{
    return NEPubSubMix::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPubSubMix.cpp file
//////////////////////////////////////////////////////////////////////////
