//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NEMessageOutput.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/NEMessageOutput.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "src/gen/NEMessageOutput.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   MessageOutput Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NEMessageOutput::ServiceName;
        _InterfaceData.idVersion        = NEMessageOutput::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceLocal;
        _InterfaceData.idRequestCount   = NEMessageOutput::NumberofRequests;
        _InterfaceData.idResponseCount  = NEMessageOutput::NumberofResponses;
        _InterfaceData.idAttributeCount = NEMessageOutput::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEMessageOutput::MSG_ID_NO_PROCEED              ) // requestOutputMessage( const String & msgOutput, const DateTime & dateTime )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int _ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/

        /************************************************************************
         * Broadcasts
         ************************************************************************/
            1, // void broadcastActionRequest( NEMessageOutput::eRequestedActons userAction )
            1, // void broadcastInputMessage( const String & msgInput )

        };

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEMessageOutput functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEMessageOutput::createInterfaceData( void )
{
    return _getInterfaceData();
}

NEMessageOutput::eMessageIDs NEMessageOutput::getResponseId( NEMessageOutput::eMessageIDs reqId )
{
    return NEMessageOutput::MSG_ID_INVALID;
}

NEMessageOutput::eMessageIDs NEMessageOutput::getRequestId( NEMessageOutput::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NEMessageOutput::eMessageIDs result = NEMessageOutput::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEMessageOutput::MSG_ID_INVALID && i < NEMessageOutput::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NEMessageOutput::RequestIds[i] : NEMessageOutput::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/NEMessageOutput.cpp file
//////////////////////////////////////////////////////////////////////////
