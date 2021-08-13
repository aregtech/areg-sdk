//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 * Generated at     11.08.2021  13:17:34 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NESystemShutdown.cpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown Service Interface Namespace implementation.
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
    const unsigned int * _RequestList       = static_cast<const unsigned int *>(NULL); /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NESystemShutdown::MSG_ID_broadcastServiceUnavailable  ) // broadcastServiceUnavailable( void )
        , static_cast<unsigned int>( NESystemShutdown::MSG_ID_broadcastServiceShutdown     ) // broadcastServiceShutdown( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NESystemShutdown::MSG_ID_ServiceState                 ) // NESystemShutdown::eServiceState mServiceState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    const unsigned int * _RequestToResponseMap  = static_cast<const unsigned int *>(NULL); // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
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
        , NEService::ServiceRemote
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

NESystemShutdown::eMessageIDs NESystemShutdown::getResponseId( NESystemShutdown::eMessageIDs reqId )
{
    return NESystemShutdown::MSG_ID_INVALID;
}

NESystemShutdown::eMessageIDs NESystemShutdown::getRequestId( NESystemShutdown::eMessageIDs respId )
{
    return NESystemShutdown::MSG_ID_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////
