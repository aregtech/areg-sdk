//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NETrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
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
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NETrafficLight::ServiceName;
        InterfaceData.idVersion         = NETrafficLight::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceLocal;
        InterfaceData.idRequestCount    = NETrafficLight::NumberofRequests;
        InterfaceData.idResponseCount   = NETrafficLight::NumberofResponses;
        InterfaceData.idAttributeCount  = NETrafficLight::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NETrafficLight::MSG_ID_ResponseChangeLight  ) // RequestChangeLight( const NETrafficLight::eLight & lightColor, const bool & holdon )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            1, // void ResponseChangeLight( const NETrafficLight::eLight & lightColor )

        /************************************************************************
         * Broadcasts
         ************************************************************************/

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NETrafficLight functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NETrafficLight::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NETrafficLight::eMessageIDs NETrafficLight::GetResponseId( NETrafficLight::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NETrafficLight::eMessageIDs>(sid.idRequestToResponseMap[index]) : NETrafficLight::MSG_ID_INVALID );
}

NETrafficLight::eMessageIDs NETrafficLight::GetRequestId( NETrafficLight::eMessageIDs respId )
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
