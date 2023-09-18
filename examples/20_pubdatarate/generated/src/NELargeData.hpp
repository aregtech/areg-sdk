//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NELargeData.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NELARGEDATA_HPP
#define  GENERATED_SRC_NELARGEDATA_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:01 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/NELargeData.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "common/NELargeData.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg//component/NERegistry.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NELargeData namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   LargeData Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          This service will demonstrate the TCP/IP data rate by sending 
 *          image data to connected clients.
 *
 **/
namespace   NELargeData
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NELargeData::ServiceName
     **/
    constexpr   char    ServiceName[]   { "LargeData" };
    /**
     * \brief   Implemented Version.
     *          NELargeData::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NELargeData::eMessageIDs
     *          LargeData Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/

    /************************************************************************
     * Response IDs
     ************************************************************************/
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastImageBlockAcquired
         *          Called to notify all connected clients that new image data is available.
         **/
          MsgId_broadcastImageBlockAcquired   = NEService::RESPONSE_ID_FIRST  // void broadcastImageBlockAcquired( const NELargeData::ImageBlock & imageBlock );
        /**
         * \brief   Broadcast ID: MsgId_broadcastServiceStopping
         *          Called to notify all clients that service is shutting down.
         **/
        , MsgId_broadcastServiceStopping                                        // void broadcastServiceStopping( void );

    /************************************************************************
     * Attributes IDs
     ************************************************************************/

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed                  = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                       = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NELargeData::eMessageIDs
     **/
    inline const char * getString( NELargeData::eMessageIDs msgId );

/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/

    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & getInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NELargeData::eMessageIDs getResponseId( NELargeData::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NELargeData::eMessageIDs getRequestId( NELargeData::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NELargeData inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NELargeData methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NELargeData::eMessageIDs );

/**
 * Returns string value of NELargeData::eMessageIDs
 **/
inline const char * NELargeData::getString( NELargeData::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NELargeData::eMessageIDs::MsgId_broadcastImageBlockAcquired:
        return "NELargeData::eMessageIDs::MsgId_broadcastImageBlockAcquired";
    case    NELargeData::eMessageIDs::MsgId_broadcastServiceStopping:
        return "NELargeData::eMessageIDs::MsgId_broadcastServiceStopping";

    case NELargeData::eMessageIDs::MsgId_NotProcessed:
        return "NELargeData::eMessageIDs::MsgId_NotProcessed";
    case NELargeData::eMessageIDs::MsgId_Invalid:
        return "NELargeData::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NELargeData::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NELargeData namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NELARGEDATA_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NELargeData.hpp file
//////////////////////////////////////////////////////////////////////////
