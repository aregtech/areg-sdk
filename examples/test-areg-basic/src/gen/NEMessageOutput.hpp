//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/NEMessageOutput.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_NEMESSAGEOUTPUT_HPP
#define  SRC_GEN_NEMESSAGEOUTPUT_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/NEMessageOutput.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NEMessageOutput namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   MessageOutput Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          The message output service to interact with user.
 *
 **/
namespace   NEMessageOutput
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NEMessageOutput::ServiceName
     **/
    const char * const    ServiceName         = "MessageOutput";
    /**
     * \brief   Implemented Version
     *          NEMessageOutput::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "0.1.0";

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The actions requested by user.
     *          NEMessageOutput::eRequestedActons
     **/
    enum eRequestedActons
    {
        /**
         * \brief   No action is requested.
         **/
          NoAction    
        /**
         * \brief   Requested action to start, console commands:
         *          start, -s
         **/
        , ActionStart 
        /**
         * \brief   Requested action to stop. Console commands:
         *          stop, -p
         **/
        , ActionStop  
        /**
         * \brief   Requested to quit applicaton. Console commands:
         *          quit, exit, -q
         **/
        , ActionQuit  
    };
    /**
     * \brief   Returns string value of NEMessageOutput::eRequestedActons
     **/
    inline const char * getString( NEMessageOutput::eRequestedActons enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEMessageOutput::eMessageIDs
     *          MessageOutput Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestOutputMessage
         *          Sent to service to output message on console.
         **/
          MSG_ID_requestOutputMessage     = NEService::REQUEST_ID_FIRST     // void requestOutputMessage( const String & msgOutput, const DateTime & dateTime );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastActionRequest
         *          Informs the action requested by a user
         **/
        , MSG_ID_broadcastActionRequest   = NEService::RESPONSE_ID_FIRST    // void broadcastActionRequest( NEMessageOutput::eRequestedActons userAction );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastInputMessage
         *          The input message from the user.
         **/
        , MSG_ID_broadcastInputMessage                                      // void broadcastInputMessage( const String & msgInput );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_LastMessage
         *          The last message to output on console.
         **/
        , MSG_ID_LastMessage              = NEService::ATTRIBUTE_ID_FIRST   // String mLastMessage;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED               = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID                  = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
    /**
     * \brief   Returns string value of NEMessageOutput::eMessageIDs
     **/
    inline const char * getString( NEMessageOutput::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & createInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEMessageOutput::eMessageIDs getResponseId( NEMessageOutput::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEMessageOutput::eMessageIDs getRequestId( NEMessageOutput::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_requestOutputMessage
    };
    /**
     * \brief   Number of supported Request entries
     **/
    const unsigned int NumberofRequests     = MACRO_ARRAYLEN(RequestIds);

    /**
     * \brief   Array of supported Response IDs
     **/
    const eMessageIDs    ResponseIds[] = 
    {

          MSG_ID_broadcastActionRequest
        , MSG_ID_broadcastInputMessage
    };
    /**
     * \brief   Number of supported Response entries
     **/
    const unsigned int  NumberofResponses   = MACRO_ARRAYLEN(ResponseIds);

    /**
     * \brief   Array of supported Attribute IDs
     **/
    const eMessageIDs    AttributeIds[] =
    {
          MSG_ID_LastMessage
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

}
//////////////////////////////////////////////////////////////////////////
// NEMessageOutput inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEMessageOutput methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEMessageOutput::eMessageIDs );

/************************************************************************
 * NEMessageOutput::eRequestedActons enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEMessageOutput::eRequestedActons )

/**
 * Return string value of NEMessageOutput::eRequestedActons
 **/
inline const char * NEMessageOutput::getString( NEMessageOutput::eRequestedActons enumValue )
{
    switch ( enumValue )
    {
    case    NEMessageOutput::NoAction:
        return "NEMessageOutput::NoAction";
    case    NEMessageOutput::ActionStart:
        return "NEMessageOutput::ActionStart";
    case    NEMessageOutput::ActionStop:
        return "NEMessageOutput::ActionStop";
    case    NEMessageOutput::ActionQuit:
        return "NEMessageOutput::ActionQuit";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEMessageOutput::eRequestedActons value!";
    }
}

/**
 * Returns string value of NEMessageOutput::eMessageIDs
 **/
inline const char * NEMessageOutput::getString( NEMessageOutput::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEMessageOutput::MSG_ID_requestOutputMessage:
        return "NEMessageOutput::MSG_ID_requestOutputMessage";
    case    NEMessageOutput::MSG_ID_broadcastActionRequest:
        return "NEMessageOutput::MSG_ID_broadcastActionRequest";
    case    NEMessageOutput::MSG_ID_broadcastInputMessage:
        return "NEMessageOutput::MSG_ID_broadcastInputMessage";
    case    NEMessageOutput::MSG_ID_LastMessage:
        return "NEMessageOutput::MSG_ID_LastMessage";

    case NEMessageOutput::MSG_ID_NO_PROCEED:
        return "NEMessageOutput::MSG_ID_NO_PROCEED";
        
    case NEMessageOutput::MSG_ID_INVALID:
        return "NEMessageOutput::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEMessageOutput::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEMessageOutput namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_NEMESSAGEOUTPUT_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/NEMessageOutput.hpp file
//////////////////////////////////////////////////////////////////////////
