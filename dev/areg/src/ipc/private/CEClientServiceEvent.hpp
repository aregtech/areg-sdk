#ifndef AREG_IPC_PRIVATE_CECLIENTSERVICEEVENT_HPP
#define AREG_IPC_PRIVATE_CECLIENTSERVICEEVENT_HPP

/************************************************************************
 * \file        areg/src/ipc/private/CEClientServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/TEEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CEClientServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The client service event data. Used for service broker and
 *          connection service communication
 **/
class CEClientServiceEventData
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
     * \brief   Converts CEClientServiceEventData:eClientServiceCommands values to string.
     **/
    static inline const char * GetString( CEClientServiceEventData::eClientServiceCommands cmdService );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client connection service event data, sets command.
     * \param   cmdService  The command to set in event data.
     **/
    CEClientServiceEventData( CEClientServiceEventData::eClientServiceCommands cmdService );
    /**
     * \brief   Copies the event data from given source.
     * \param   source  The source to copy data.
     **/
    CEClientServiceEventData( const CEClientServiceEventData & source );
    /**
     * \brief   Destructor.
     **/
    ~CEClientServiceEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    const CEClientServiceEventData & operator = ( const CEClientServiceEventData & source );

    /**
     * \brief   Returns command saved in event data.
     **/
    inline CEClientServiceEventData::eClientServiceCommands GetCommand( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of client service event.
     **/
    CEClientServiceEventData::eClientServiceCommands    mServiceCommand;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientServiceEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// CEClientServiceEvent and IEClientServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of CEClientServiceEvent event and IEClientServiceEventConsumer consumer classes
DECLARE_EVENT(CEClientServiceEventData, CEClientServiceEvent, IEClientServiceEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// CEClientServiceEventData class inline methods
//////////////////////////////////////////////////////////////////////////////

inline CEClientServiceEventData::eClientServiceCommands CEClientServiceEventData::GetCommand( void ) const
{   return mServiceCommand;                                 }

inline const char * CEClientServiceEventData::GetString( CEClientServiceEventData::eClientServiceCommands cmdService )
{
    switch ( cmdService )
    {
    case CEClientServiceEventData::CMD_StartService:
        return "CEClientServiceEventData::CMD_StartService";
    case CEClientServiceEventData::CMD_StopService:
        return "CEClientServiceEventData::CMD_StopService";
    case CEClientServiceEventData::CMD_ServiceStarted:
        return "CEClientServiceEventData::CMD_ServiceStarted";
    case CEClientServiceEventData::CMD_ServiceStopped:
        return "CEClientServiceEventData::CMD_ServiceStopped";
    case CEClientServiceEventData::CMD_ServiceLost:
        return "CEClientServiceEventData::CMD_ServiceLost";
    default:
        return "ERR: Undefined CEClientServiceEventData::eClientServiceCommands value!!!";
    }
}

#endif  // AREG_IPC_PRIVATE_CECLIENTSERVICEEVENT_HPP
