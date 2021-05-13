//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NESystem.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
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
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NESystem::ServiceName;
        _InterfaceData.idVersion        = NESystem::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceLocal;
        _InterfaceData.idRequestCount   = NESystem::NumberofRequests;
        _InterfaceData.idResponseCount  = NESystem::NumberofResponses;
        _InterfaceData.idAttributeCount = NESystem::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // requestPowerOff( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // requestPowerOn( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // requestShutdown( void )
            , static_cast<unsigned int>( NESystem::MSG_ID_NO_PROCEED       ) // requestStart( void )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int * _ResponseParamStateMap = static_cast<const unsigned int *>(NULL); // EMPTY RESPONSE AND BROADCAST LIST

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NESystem functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESystem::createInterfaceData( void )
{
    return _getInterfaceData();
}

NESystem::eMessageIDs NESystem::getResponseId( NESystem::eMessageIDs reqId )
{
    return NESystem::MSG_ID_INVALID;
}

NESystem::eMessageIDs NESystem::getRequestId( NESystem::eMessageIDs respId )
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
