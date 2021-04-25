//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NESystem.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/NESystem.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "src/gen/NESystem.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   System Service Interface data
     **/
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NESystem::ServiceName;
        InterfaceData.idVersion         = NESystem::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceLocal;
        InterfaceData.idRequestCount    = NESystem::NumberofRequests;
        InterfaceData.idResponseCount   = NESystem::NumberofResponses;
        InterfaceData.idAttributeCount  = NESystem::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // RequestPowerOff( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // RequestPowerOn( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // RequestShutdown( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // RequestStart( void )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int * ResponseParamStateMap = static_cast<const unsigned int *>(NULL); // EMPTY RESPONSE AND BROADCAST LIST

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NESystem functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESystem::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NESystem::eMessageIDs NESystem::GetResponseId( NESystem::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NESystem::eMessageIDs>(sid.idRequestToResponseMap[index]) : NESystem::MSG_ID_INVALID );
}

NESystem::eMessageIDs NESystem::GetRequestId( NESystem::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NESystem::eMessageIDs result = NESystem::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NESystem::MSG_ID_INVALID && i < NESystem::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NESystem::RequestIds[i] : NESystem::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/NESystem.cpp file
//////////////////////////////////////////////////////////////////////////
