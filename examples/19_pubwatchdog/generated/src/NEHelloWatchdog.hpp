//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloWatchdog.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEHELLOWATCHDOG_HPP
#define  GENERATED_SRC_NEHELLOWATCHDOG_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/NEHelloWatchdog.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////

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
// NEHelloWatchdog namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   HelloWatchdog Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          This service will demonstrate how the system watchdog works in case of public service.
 *
 **/
namespace   NEHelloWatchdog
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEHelloWatchdog::ServiceName
     **/
    constexpr   char    ServiceName[]   { "HelloWatchdog" };
    /**
     * \brief   Implemented Version.
     *          NEHelloWatchdog::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The timeout of the watchdog
     *          NEHelloWatchdog::TimeoutWatchdog
     **/
    constexpr unsigned int    TimeoutWatchdog     = 1000;

    /**
     * \brief   The step to increase sleep timeout.
     *          NEHelloWatchdog::TimeoutStep
     **/
    constexpr unsigned int    TimeoutStep         = 300;

    /**
     * \brief   The initial timeout to sleep a thread.
     *          NEHelloWatchdog::InitialSleepTimeout
     **/
    constexpr unsigned int    InitialSleepTimeout = 300;

    /**
     * \brief   The maximum restarts of service.
     *          NEHelloWatchdog::MaximumRestarts
     **/
    constexpr unsigned int    MaximumRestarts     = 4;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The state of component to start a timer and output "Hello Watchdog" message.
     *          NEHelloWatchdog::eState
     **/
    enum class eState
    {
        /**
         * \brief   The service just initialized and ready to execute.
         **/
          Initialized
        /**
         * \brief   The timer is started to output messages.
         **/
        , Started
        /**
         * \brief   The timer is requested to stop.
         **/
        , Stopped
    };
    /**
     * \brief   Returns string value of NEHelloWatchdog::eState
     **/
    inline const char * getString( NEHelloWatchdog::eState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEHelloWatchdog::eMessageIDs
     *          HelloWatchdog Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestStartSleep
         *          The response triggered when the thread resumed from suspended mode.
         **/
          MsgId_requestStartSleep         = NEService::REQUEST_ID_FIRST   // void requestStartSleep( unsigned int timeoutSleep );
        /**
         * \brief   Request ID: MsgId_requestStopService
         *          Called to stop the service.
         **/
        , MsgId_requestStopService                                          // void requestStopService( void );
        /**
         * \brief   Request ID: MsgId_requestShutdownService
         *          Called to shutdown service and exit application.
         **/
        , MsgId_requestShutdownService                                      // void requestShutdownService( void );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseStartSleep
         *          Triggered to stop the timer.
         **/
        , MsgId_responseStartSleep        = NEService::RESPONSE_ID_FIRST  // void responseStartSleep( unsigned int timeoutSleep );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ServiceState
         *          The state of the service.
         **/
        , MsgId_ServiceState              = NEService::ATTRIBUTE_ID_FIRST // NEHelloWatchdog::eState mServiceState;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed              = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                   = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEHelloWatchdog::eMessageIDs
     **/
    inline const char * getString( NEHelloWatchdog::eMessageIDs msgId );

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
    NEHelloWatchdog::eMessageIDs getResponseId( NEHelloWatchdog::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEHelloWatchdog::eMessageIDs getRequestId( NEHelloWatchdog::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWatchdog inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEHelloWatchdog methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEHelloWatchdog::eMessageIDs );

/************************************************************************
 * NEHelloWatchdog::eState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEHelloWatchdog::eState )

/**
 * Return string value of NEHelloWatchdog::eState
 **/
inline const char * NEHelloWatchdog::getString( NEHelloWatchdog::eState enumValue )
{
    switch ( enumValue )
    {
    case    NEHelloWatchdog::eState::Initialized:
        return "NEHelloWatchdog::eState::Initialized";
    case    NEHelloWatchdog::eState::Started:
        return "NEHelloWatchdog::eState::Started";
    case    NEHelloWatchdog::eState::Stopped:
        return "NEHelloWatchdog::eState::Stopped";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEHelloWatchdog::eState value!";
    }
}

/**
 * Returns string value of NEHelloWatchdog::eMessageIDs
 **/
inline const char * NEHelloWatchdog::getString( NEHelloWatchdog::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep:
        return "NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep";
    case    NEHelloWatchdog::eMessageIDs::MsgId_requestStopService:
        return "NEHelloWatchdog::eMessageIDs::MsgId_requestStopService";
    case    NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService:
        return "NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService";
    case    NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep:
        return "NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep";
    case    NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
        return "NEHelloWatchdog::eMessageIDs::MsgId_ServiceState";

    case NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed:
        return "NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed";
    case NEHelloWatchdog::eMessageIDs::MsgId_Invalid:
        return "NEHelloWatchdog::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEHelloWatchdog::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWatchdog namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEHELLOWATCHDOG_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloWatchdog.hpp file
//////////////////////////////////////////////////////////////////////////
