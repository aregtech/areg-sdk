#ifndef AREG_IPC_PRIVATE_SERVERSERVICEEVENT_HPP
#define AREG_IPC_PRIVATE_SERVERSERVICEEVENT_HPP

/************************************************************************
 * \file        areg/ipc/private/ServerServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/RemoteMessage.hpp"

//////////////////////////////////////////////////////////////////////////
// ServerServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The broker side service event data. Used for message broker and
 *          connection service communication
 **/
class ServerServiceEventData
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
     * \brief   Converts ServerServiceEventData:eServerServiceCommands values to string.
     **/
    static inline const char * getString( ServerServiceEventData::eServerServiceCommands cmdService );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes server connection service event data, sets command.
     * \param   cmdService  The command to set in event data.
     **/
    ServerServiceEventData( ServerServiceEventData::eServerServiceCommands cmdService );
    /**
     * \brief   Initializes server connection service event data, sets command and message data buffer.
     * \param   cmdService  The command to set in event data.
     * \param   msgService  The message data buffer to initialize.
     **/
    ServerServiceEventData( ServerServiceEventData::eServerServiceCommands cmdService, const RemoteMessage & msgService );
    /**
     * \brief   Copies the event data from given source.
     * \param   source  The source to copy data.
     **/
    ServerServiceEventData( const ServerServiceEventData & source );
    /**
     * \brief   Destructor.
     **/
    ~ServerServiceEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    const ServerServiceEventData & operator = ( const ServerServiceEventData & source );

    /**
     * \brief   Returns command saved in event data.
     **/
    inline ServerServiceEventData::eServerServiceCommands getCommand( void ) const;

    /**
     * \brief   Returns message data buffer object saved in event data.
     **/
    inline const RemoteMessage & getMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of server service event.
     **/
    ServerServiceEventData::eServerServiceCommands    mServiceCommand;
    /**
     * \brief   The message data buffer saved in service event.
     **/
    mutable RemoteMessage                             mMessageData;

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////////
private:
    ServerServiceEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// ServerServiceEvent and IEServerServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of ServerServiceEvent event and IEServerServiceEventConsumer consumer classes
DECLARE_EVENT(ServerServiceEventData, ServerServiceEvent, IEServerServiceEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// ServerServiceEventData class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline ServerServiceEventData::eServerServiceCommands ServerServiceEventData::getCommand( void ) const
{
    return mServiceCommand;
}

inline const RemoteMessage & ServerServiceEventData::getMessage( void ) const
{
    return mMessageData;
}

inline const char * ServerServiceEventData::getString( ServerServiceEventData::eServerServiceCommands cmdService )
{
    switch ( cmdService )
    {
    case ServerServiceEventData::CMD_StartService:
        return "ServerServiceEventData::CMD_StartService";
    case ServerServiceEventData::CMD_StopService:
        return "ServerServiceEventData::CMD_StopService";
    case ServerServiceEventData::CMD_ServiceSendMsg:
        return "ServerServiceEventData::CMD_ServiceSendMsg";
    case ServerServiceEventData::CMD_ServiceReceivedMsg:
        return "ServerServiceEventData::CMD_ServiceReceivedMsg";
    default:
        return "ERR: Unexpected ServerServiceEventData::eServerServiceCommands value!!!";
    }
}

#endif  // AREG_IPC_PRIVATE_SERVERSERVICEEVENT_HPP
