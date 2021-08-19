//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPowerManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NEPowerManager.cpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEPowerManager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEPowerManager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEPowerManager::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NEPowerManager::MSG_ID_requestPowerOn             ) // requestPowerOn( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_requestPowerOff            ) // requestPowerOff( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_requestStartTrafficLight   ) // requestStartTrafficLight( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_requestStopTrafficLight     ) // requestStopTrafficLight( void )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NEPowerManager::MSG_ID_responseStartTrafficLight  ) // responseStartTrafficLight( bool Success )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_responseStopTrafficLight    ) // responseStopTrafficLight( bool Success )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEPowerManager::MSG_ID_LightsPowerState           ) // NEPowerManager::ePoweredState mLightsPowerState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NEPowerManager::MSG_ID_NO_PROCEED                 ) // requestPowerOn( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_NO_PROCEED                 ) // requestPowerOff( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_responseStartTrafficLight  ) // requestStartTrafficLight( void )
        , static_cast<unsigned int>( NEPowerManager::MSG_ID_responseStopTrafficLight    ) // requestStopTrafficLight( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseStartTrafficLight( bool Success )
        1, // void responseStopTrafficLight( bool Success )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };
    
    /************************************************************************
     * PowerManager Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEPowerManager::ServiceName
        , NEPowerManager::InterfaceVersion
        , NEService::ServiceLocal
        , 4
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

NEPowerManager::eMessageIDs NEPowerManager::getResponseId( NEPowerManager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NEPowerManager::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEPowerManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEPowerManager::MSG_ID_INVALID );
}

NEPowerManager::eMessageIDs NEPowerManager::getRequestId( NEPowerManager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEPowerManager::getInterfaceData();
    NEPowerManager::eMessageIDs result = NEPowerManager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEPowerManager::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NEPowerManager::eMessageIDs>(sid.idRequestList[i]) : NEPowerManager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPowerManager.cpp file
//////////////////////////////////////////////////////////////////////////
