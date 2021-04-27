#ifndef AREG_IPC_PRIVATE_CESERVERSERVICEEVENT_HPP
#define AREG_IPC_PRIVATE_CESERVERSERVICEEVENT_HPP

/************************************************************************
 * \file        areg/ipc/private/CEServerServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/CERemoteMessage.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServerServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The broker side service event data. Used for message broker and
 *          connection service communication
 **/
class CEServerServiceEventData
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The list of commands for server connection service
     **/
    typedef enum E_ServerServiceCommands
    {
          CMD_StartService          //!< Sent to start message broker server connection service
        , CMD_StopService           //!< Sent to stop message broker server connection service
        , CMD_ServiceSendMsg        //!< Sent to notify to sent messages to connected client
        , CMD_ServiceReceivedMsg    //!< Sent to notify received message from connected client
    } eServerServiceCommands;

    /**
     * \brief   Converts CEServerServiceEventData:eServerServiceCommands values to string.
     **/
    static inline const char * GetString( CEServerServiceEventData::eServerServiceCommands cmdService );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes server connection service event data, sets command.
     * \param   cmdService  The command to set in event data.
     **/
    CEServerServiceEventData( CEServerServiceEventData::eServerServiceCommands cmdService );
    /**
     * \brief   Initializes server connection service event data, sets command and message data buffer.
     * \param   cmdService  The command to set in event data.
     * \param   msgService  The message data buffer to initialize.
     **/
    CEServerServiceEventData( CEServerServiceEventData::eServerServiceCommands cmdService, const CERemoteMessage & msgService );
    /**
     * \brief   Copies the event data from given source.
     * \param   source  The source to copy data.
     **/
    CEServerServiceEventData( const CEServerServiceEventData & source );
    /**
     * \brief   Destructor.
     **/
    ~CEServerServiceEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    const CEServerServiceEventData & operator = ( const CEServerServiceEventData & source );

    /**
     * \brief   Returns command saved in event data.
     **/
    inline CEServerServiceEventData::eServerServiceCommands GetCommand( void ) const;

    /**
     * \brief   Returns message data buffer object saved in event data.
     **/
    inline const CERemoteMessage & GetMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of server service event.
     **/
    CEServerServiceEventData::eServerServiceCommands    mServiceCommand;
    /**
     * \brief   The message data buffer saved in service event.
     **/
    mutable CERemoteMessage                             mMessageData;

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////////
private:
    CEServerServiceEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// CEServerServiceEvent and IEServerServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of CEServerServiceEvent event and IEServerServiceEventConsumer consumer classes
DECLARE_EVENT(CEServerServiceEventData, CEServerServiceEvent, IEServerServiceEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// CEServerServiceEventData class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline CEServerServiceEventData::eServerServiceCommands CEServerServiceEventData::GetCommand( void ) const
{   return mServiceCommand; }

inline const CERemoteMessage & CEServerServiceEventData::GetMessage( void ) const
{   return mMessageData;    }

inline const char * CEServerServiceEventData::GetString( CEServerServiceEventData::eServerServiceCommands cmdService )
{
    switch ( cmdService )
    {
    case CEServerServiceEventData::CMD_StartService:
        return "CEServerServiceEventData::CMD_StartService";
    case CEServerServiceEventData::CMD_StopService:
        return "CEServerServiceEventData::CMD_StopService";
    case CEServerServiceEventData::CMD_ServiceSendMsg:
        return "CEServerServiceEventData::CMD_ServiceSendMsg";
    case CEServerServiceEventData::CMD_ServiceReceivedMsg:
        return "CEServerServiceEventData::CMD_ServiceReceivedMsg";
    default:
        return "ERR: Unexpected CEServerServiceEventData::eServerServiceCommands value!!!";
    }
}
#endif  // AREG_IPC_PRIVATE_CESERVERSERVICEEVENT_HPP
