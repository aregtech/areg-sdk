//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NELargeData.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     22.09.2022  23:42:15 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/NELargeData.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NELargeData.hpp"

//////////////////////////////////////////////////////////////////////////
// NELargeData functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NELargeData::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NELargeData::eMessageIDs::MsgId_broadcastWriteImageBlock  ) // broadcastWriteImageBlock( void )
        , static_cast<unsigned int>( NELargeData::eMessageIDs::MsgId_broadcastServiceStopping  ) // broadcastServiceStopping( void )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    constexpr unsigned int * _AttributeList { nullptr };    /* There are no attributes. Set Invalid message ID */

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        0, // void broadcastWriteImageBlock( void )
        0, // void broadcastServiceStopping( void )

    };

    /************************************************************************
     * LargeData Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NELargeData::ServiceName
        , NELargeData::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 2
        , 0
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NELargeData::eMessageIDs NELargeData::getResponseId( NELargeData::eMessageIDs /* reqId */ )
{
    return NELargeData::eMessageIDs::MsgId_Invalid;
}

NELargeData::eMessageIDs NELargeData::getRequestId( NELargeData::eMessageIDs /* respId */ )
{
    return NELargeData::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NELargeData.cpp file
//////////////////////////////////////////////////////////////////////////
