//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPowerManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/NEPowerManager.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
 *                  Service Interface namespace implementation.
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
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_requestPowerOn             ) // requestPowerOn( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_requestPowerOff            ) // requestPowerOff( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight   ) // requestStartTrafficLight( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight    ) // requestStopTrafficLight( void )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight  ) // responseStartTrafficLight( bool Success )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight   ) // responseStopTrafficLight( bool Success )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_LightsPowerState           ) // NEPowerManager::ePoweredState mLightsPowerState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_NotProcessed               ) // requestPowerOn( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_NotProcessed               ) // requestPowerOff( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight  ) // requestStartTrafficLight( void )
        , static_cast<unsigned int>( NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight   ) // requestStopTrafficLight( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
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
        , NEService::eServiceType::ServiceLocal
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

NEPowerManager::eMessageIDs NEPowerManager::getResponseId( NEPowerManager::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEPowerManager::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEPowerManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEPowerManager::eMessageIDs::MsgId_Invalid );
}

NEPowerManager::eMessageIDs NEPowerManager::getRequestId( NEPowerManager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEPowerManager::getInterfaceData();
    NEPowerManager::eMessageIDs result = NEPowerManager::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEPowerManager::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEPowerManager::eMessageIDs>(sid.idRequestList[i]) : NEPowerManager::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPowerManager.cpp file
//////////////////////////////////////////////////////////////////////////
