//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEOptions.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/NEOptions.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEOptions.hpp"

//////////////////////////////////////////////////////////////////////////
// NEOptions functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEOptions::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_requestDataRate         ) // requestDataRate( void )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_requestPacketRate       ) // requestPacketRate( void )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_requestChangeOptions    ) // requestChangeOptions( const NEOptions::sOptionData & Options )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_requestQuitApplication  ) // requestQuitApplication( void )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_responseDataRate        ) // responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_responsePacketRate      ) // responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_OptionData              ) // NEOptions::sOptionData mOptionData;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_responseDataRate        ) // requestDataRate( void )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_responsePacketRate      ) // requestPacketRate( void )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_NotProcessed            ) // requestChangeOptions( const NEOptions::sOptionData & Options )
        , static_cast<unsigned int>( NEOptions::eMessageIDs::MsgId_NotProcessed            ) // requestQuitApplication( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        2, // void responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData )
        2, // void responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * Options Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEOptions::ServiceName
        , NEOptions::InterfaceVersion
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

NEOptions::eMessageIDs NEOptions::getResponseId( NEOptions::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEOptions::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEOptions::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEOptions::eMessageIDs::MsgId_Invalid );
}

NEOptions::eMessageIDs NEOptions::getRequestId( NEOptions::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEOptions::getInterfaceData();
    NEOptions::eMessageIDs result = NEOptions::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEOptions::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEOptions::eMessageIDs>(sid.idRequestList[i]) : NEOptions::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEOptions.cpp file
//////////////////////////////////////////////////////////////////////////
