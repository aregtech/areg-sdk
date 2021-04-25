//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/NEMessageOutput.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
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
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NEMessageOutput::ServiceName;
        InterfaceData.idVersion         = NEMessageOutput::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceLocal;
        InterfaceData.idRequestCount    = NEMessageOutput::NumberofRequests;
        InterfaceData.idResponseCount   = NEMessageOutput::NumberofResponses;
        InterfaceData.idAttributeCount  = NEMessageOutput::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEMessageOutput::MSG_ID_NO_PROCEED              ) // RequestOutputMessage( const CEString & msgOutput, const CEDateTime & dateTime )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/

        /************************************************************************
         * Broadcasts
         ************************************************************************/
            1, // void BroadcastActionRequest( const NEMessageOutput::eRequestedActons & userAction )
            1, // void BroadcastInputMessage( const CEString & msgInput )

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEMessageOutput functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEMessageOutput::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NEMessageOutput::eMessageIDs NEMessageOutput::GetResponseId( NEMessageOutput::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEMessageOutput::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEMessageOutput::MSG_ID_INVALID );
}

NEMessageOutput::eMessageIDs NEMessageOutput::GetRequestId( NEMessageOutput::eMessageIDs respId )
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
