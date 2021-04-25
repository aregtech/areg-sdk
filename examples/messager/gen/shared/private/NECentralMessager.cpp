//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     24.08.2019  21:12:29 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/private/NECentralMessager.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "gen/shared/NECentralMessager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   CentralMessager Service Interface data
     **/
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NECentralMessager::ServiceName;
        InterfaceData.idVersion         = NECentralMessager::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceRemote;
        InterfaceData.idRequestCount    = NECentralMessager::NumberofRequests;
        InterfaceData.idResponseCount   = NECentralMessager::NumberofResponses;
        InterfaceData.idAttributeCount  = NECentralMessager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // RequestSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime )
            , static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // RequestKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage )
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
            4, // void BroadcastSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime )
            3, // void BroadcastKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage )
            2, // void BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime )

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NECentralMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NECentralMessager::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NECentralMessager::eMessageIDs NECentralMessager::GetResponseId( const NECentralMessager::eMessageIDs reqId )
{
    const NEService::SInterfaceData& sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NECentralMessager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NECentralMessager::MSG_ID_INVALID );
}

NECentralMessager::eMessageIDs NECentralMessager::GetRequestId( const NECentralMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData& sid = _getInterfaceData();
    NECentralMessager::eMessageIDs result = NECentralMessager::MSG_ID_INVALID;
    for (unsigned int i = 0; result == NECentralMessager::MSG_ID_INVALID && i < NECentralMessager::NumberofRequests; i ++)
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NECentralMessager::RequestIds[i] : NECentralMessager::MSG_ID_INVALID;
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////
