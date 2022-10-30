#ifndef AREG_MCROUTER_TCP_PRIVATE_SERVERSERVICEEVENT_HPP
#define AREG_MCROUTER_TCP_PRIVATE_SERVERSERVICEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServerServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
 * \brief   The routing service event data. Used for communication with
 *          message router.
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
    typedef enum class E_ServerServiceCommands
    {
          CMD_StartService          //!< Start message router connection service
        , CMD_StopService           //!< Stop message router connection service
        , CMD_RestartService        //!< Restart the message router connection service
        , CMD_ServiceSendMsg        //!< Notify to sent messages to connected client
        , CMD_ServiceReceivedMsg    //!< Notify received message from connected client
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
    explicit ServerServiceEventData( ServerServiceEventData::eServerServiceCommands cmdService );
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
     * \brief   Moves the event data from given source.
     * \param   source  The source to move data.
     **/
    ServerServiceEventData( ServerServiceEventData && source ) noexcept;
    /**
     * \brief   Destructor.
     **/
    ~ServerServiceEventData( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    ServerServiceEventData & operator = ( const ServerServiceEventData & source );

    /**
     * \brief   Moves event data from given source.
     * \param   source      The source to move data.
     **/
    ServerServiceEventData & operator = ( ServerServiceEventData && source ) noexcept;

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
    ServerServiceEventData( void ) = delete;
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
    case ServerServiceEventData::eServerServiceCommands::CMD_StartService:
        return "ServerServiceEventData::CMD_StartService";
    case ServerServiceEventData::eServerServiceCommands::CMD_StopService:
        return "ServerServiceEventData::CMD_StopService";
    case ServerServiceEventData::eServerServiceCommands::CMD_RestartService:
        return "ServerServiceEventData::CMD_RestartService";
    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceSendMsg:
        return "ServerServiceEventData::CMD_ServiceSendMsg";
    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg:
        return "ServerServiceEventData::CMD_ServiceReceivedMsg";
    default:
        return "ERR: Unexpected ServerServiceEventData::eServerServiceCommands value!!!";
    }
}

#endif  // AREG_MCROUTER_TCP_PRIVATE_SERVERSERVICEEVENT_HPP
