//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:13 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/NESystemShutdown.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NESystemShutdown.hpp"

//////////////////////////////////////////////////////////////////////////
// NESystemShutdown functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESystemShutdown::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_broadcastServiceUnavailable  ) // broadcastServiceUnavailable( void )
        , static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_broadcastServiceShutdown     ) // broadcastServiceShutdown( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_ServiceState                 ) // NESystemShutdown::eServiceState mServiceState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        0, // void broadcastServiceUnavailable( void )
        0, // void broadcastServiceShutdown( void )

    };

    /************************************************************************
     * SystemShutdown Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESystemShutdown::ServiceName
        , NESystemShutdown::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 2
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NESystemShutdown::eMessageIDs NESystemShutdown::getResponseId( NESystemShutdown::eMessageIDs /* reqId */ )
{
    return NESystemShutdown::eMessageIDs::MsgId_Invalid;
}

NESystemShutdown::eMessageIDs NESystemShutdown::getRequestId( NESystemShutdown::eMessageIDs /* respId */ )
{
    return NESystemShutdown::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////
