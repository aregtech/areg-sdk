#ifndef AREG_IPC_PRIVATE_CLIENTSERVICEEVENT_HPP
#define AREG_IPC_PRIVATE_CLIENTSERVICEEVENT_HPP

/************************************************************************
 * \file        areg/ipc/private/ClientServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The client service event data. Used for service broker and
 *          connection service communication
 **/
class ClientServiceEventData
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The list of commands for client connection service
     **/
    typedef enum E_ClientServiceCommands
    {
          CMD_StartService      //!< Sent to start client connection service.
        , CMD_StopService       //!< Sent to stop client connection service.
        , CMD_ServiceStarted    //!< Sent to notify client service connection is started.
        , CMD_ServiceStopped    //!< Sent to notify client service connection is stopped.
        , CMD_ServiceLost       //!< Sent to notify client service that connection is lost.
    } eClientServiceCommands;

    /**
     * \brief   Converts ClientServiceEventData:eClientServiceCommands values to string.
     **/
    static inline const char * getString( ClientServiceEventData::eClientServiceCommands cmdService );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client connection service event data, sets command.
     * \param   cmdService  The command to set in event data.
     **/
    ClientServiceEventData( ClientServiceEventData::eClientServiceCommands cmdService );
    /**
     * \brief   Copies the event data from given source.
     * \param   source  The source to copy data.
     **/
    ClientServiceEventData( const ClientServiceEventData & source );
    /**
     * \brief   Destructor.
     **/
    ~ClientServiceEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    const ClientServiceEventData & operator = ( const ClientServiceEventData & source );

    /**
     * \brief   Returns command saved in event data.
     **/
    inline ClientServiceEventData::eClientServiceCommands getCommand( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of client service event.
     **/
    ClientServiceEventData::eClientServiceCommands    mServiceCommand;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientServiceEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// ClientServiceEvent and IEClientServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of ClientServiceEvent event and IEClientServiceEventConsumer consumer classes
DECLARE_EVENT(ClientServiceEventData, ClientServiceEvent, IEClientServiceEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// ClientServiceEventData class inline methods
//////////////////////////////////////////////////////////////////////////////

inline ClientServiceEventData::eClientServiceCommands ClientServiceEventData::getCommand( void ) const
{
    return mServiceCommand;
}

inline const char * ClientServiceEventData::getString( ClientServiceEventData::eClientServiceCommands cmdService )
{
    switch ( cmdService )
    {
    case ClientServiceEventData::CMD_StartService:
        return "ClientServiceEventData::CMD_StartService";
    case ClientServiceEventData::CMD_StopService:
        return "ClientServiceEventData::CMD_StopService";
    case ClientServiceEventData::CMD_ServiceStarted:
        return "ClientServiceEventData::CMD_ServiceStarted";
    case ClientServiceEventData::CMD_ServiceStopped:
        return "ClientServiceEventData::CMD_ServiceStopped";
    case ClientServiceEventData::CMD_ServiceLost:
        return "ClientServiceEventData::CMD_ServiceLost";
    default:
        return "ERR: Undefined ClientServiceEventData::eClientServiceCommands value!!!";
    }
}

#endif  // AREG_IPC_PRIVATE_CLIENTSERVICEEVENT_HPP
