//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NETrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/NETrafficLight.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "src/gen/NETrafficLight.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   TrafficLight Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NETrafficLight::ServiceName;
        _InterfaceData.idVersion        = NETrafficLight::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceLocal;
        _InterfaceData.idRequestCount   = NETrafficLight::NumberofRequests;
        _InterfaceData.idResponseCount  = NETrafficLight::NumberofResponses;
        _InterfaceData.idAttributeCount = NETrafficLight::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NETrafficLight::MSG_ID_responseChangeLight  ) // requestChangeLight( NETrafficLight::eLight lightColor, bool holdon )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int _ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            1, // void responseChangeLight( NETrafficLight::eLight lightColor )

        /************************************************************************
         * Broadcasts
         ************************************************************************/

        };

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NETrafficLight functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NETrafficLight::createInterfaceData( void )
{
    return _getInterfaceData();
}

NETrafficLight::eMessageIDs NETrafficLight::getResponseId( NETrafficLight::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NETrafficLight::eMessageIDs>(sid.idRequestToResponseMap[index]) : NETrafficLight::MSG_ID_INVALID );
}

NETrafficLight::eMessageIDs NETrafficLight::getRequestId( NETrafficLight::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NETrafficLight::eMessageIDs result = NETrafficLight::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NETrafficLight::MSG_ID_INVALID && i < NETrafficLight::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NETrafficLight::RequestIds[i] : NETrafficLight::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/NETrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////
