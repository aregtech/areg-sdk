//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     12.05.2021  16:41:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/NECentralMessager.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shared/generated/NECentralMessager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   CentralMessager Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NECentralMessager::ServiceName;
        _InterfaceData.idVersion        = NECentralMessager::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceRemote;
        _InterfaceData.idRequestCount   = NECentralMessager::NumberofRequests;
        _InterfaceData.idResponseCount  = NECentralMessager::NumberofResponses;
        _InterfaceData.idAttributeCount = NECentralMessager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
            , static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
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
            4, // void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
            3, // void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
            2, // void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )

        };

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NECentralMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NECentralMessager::createInterfaceData( void )
{
    return _getInterfaceData();
}

NECentralMessager::eMessageIDs NECentralMessager::getResponseId( NECentralMessager::eMessageIDs reqId )
{
    return NECentralMessager::MSG_ID_INVALID;
}

NECentralMessager::eMessageIDs NECentralMessager::getRequestId( NECentralMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NECentralMessager::eMessageIDs result = NECentralMessager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NECentralMessager::MSG_ID_INVALID && i < NECentralMessager::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NECentralMessager::RequestIds[i] : NECentralMessager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////
