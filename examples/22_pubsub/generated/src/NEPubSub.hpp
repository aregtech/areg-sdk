//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPubSub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPUBSUB_HPP
#define  GENERATED_SRC_NEPUBSUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     31.08.2023  12:27:59 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/NEPubSub.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/String.hpp"

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
// NEPubSub namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   PubSub Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This is an interface to test the PubSub features (data-driven communication) of the
 *          AREG framework, where service provider is a Publisher and service consumer is a Subscriber that dynamically
 *          subscribes on data update and get notified.
 *
 **/
namespace   NEPubSub
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEPubSub::ServiceName
     **/
    constexpr   char    ServiceName[]   { "PubSub" };
    /**
     * \brief   Implemented Version.
     *          NEPubSub::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The timeout to make updates on change
     *          NEPubSub::TimeoutOnChange
     **/
    constexpr unsigned int    TimeoutOnChange     = 150;

    /**
     * \brief   The timeout to update on always.
     *          NEPubSub::TimeoutAlways
     **/
    constexpr unsigned int    TimeoutAlways       = 300;

    /**
     * \brief   The number of cycles to change the value when updating values.
     *          NEPubSub::CycleAlways
     **/
    constexpr unsigned short  CycleAlways         = 3;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The state of the service provider
     *          NEPubSub::eServiceState
     **/
    enum class eServiceState
    {
        /**
         * \brief   The service provider state is initialized. The values are in invalid states.
         **/
          Uninitialized
        /**
         * \brief   The service provider state is stopped, values are initialized and valid.
         **/
        , Stopped
        /**
         * \brief   The service provider state is running, the values should change values.
         **/
        , Running
        /**
         * \brief   The service provider shuts down and quits.
         **/
        , Shutdown
    };
    /**
     * \brief   Returns string value of NEPubSub::eServiceState
     **/
    inline const char * getString( NEPubSub::eServiceState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEPubSub::eMessageIDs
     *          PubSub Service interface defined message IDs
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

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_StringOnChange
         *          This simple string should send update notification only on value change.
         **/
          MsgId_StringOnChange        = NEService::ATTRIBUTE_ID_FIRST // String mStringOnChange;
        /**
         * \brief   Attribute ID: MsgId_IntegerAlways
         *          This simple integer should send update notification always when value is set.
         **/
        , MsgId_IntegerAlways                                           // unsigned int mIntegerAlways;
        /**
         * \brief   Attribute ID: MsgId_ServiceProviderState
         *          The state of service provider. The value is always valid, updated on change.
         **/
        , MsgId_ServiceProviderState                                    // NEPubSub::eServiceState mServiceProviderState;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed          = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid               = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEPubSub::eMessageIDs
     **/
    inline const char * getString( NEPubSub::eMessageIDs msgId );

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
    NEPubSub::eMessageIDs getResponseId( NEPubSub::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEPubSub::eMessageIDs getRequestId( NEPubSub::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEPubSub inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEPubSub methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEPubSub::eMessageIDs );

/************************************************************************
 * NEPubSub::eServiceState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEPubSub::eServiceState )

/**
 * Return string value of NEPubSub::eServiceState
 **/
inline const char * NEPubSub::getString( NEPubSub::eServiceState enumValue )
{
    switch ( enumValue )
    {
    case    NEPubSub::eServiceState::Uninitialized:
        return "NEPubSub::eServiceState::Uninitialized";
    case    NEPubSub::eServiceState::Stopped:
        return "NEPubSub::eServiceState::Stopped";
    case    NEPubSub::eServiceState::Running:
        return "NEPubSub::eServiceState::Running";
    case    NEPubSub::eServiceState::Shutdown:
        return "NEPubSub::eServiceState::Shutdown";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEPubSub::eServiceState value!";
    }
}

/**
 * Returns string value of NEPubSub::eMessageIDs
 **/
inline const char * NEPubSub::getString( NEPubSub::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEPubSub::eMessageIDs::MsgId_StringOnChange:
        return "NEPubSub::eMessageIDs::MsgId_StringOnChange";
    case    NEPubSub::eMessageIDs::MsgId_IntegerAlways:
        return "NEPubSub::eMessageIDs::MsgId_IntegerAlways";
    case    NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
        return "NEPubSub::eMessageIDs::MsgId_ServiceProviderState";

    case NEPubSub::eMessageIDs::MsgId_NotProcessed:
        return "NEPubSub::eMessageIDs::MsgId_NotProcessed";
    case NEPubSub::eMessageIDs::MsgId_Invalid:
        return "NEPubSub::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPubSub::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPubSub namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEPUBSUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPubSub.hpp file
//////////////////////////////////////////////////////////////////////////
