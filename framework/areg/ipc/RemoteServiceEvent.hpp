#ifndef AREG_IPC_REMOTESERVICEEVENT_HPP
#define AREG_IPC_REMOTESERVICEEVENT_HPP

/************************************************************************
 * \file        areg/ipc/RemoteServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Send message events
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/RemoteMessage.hpp"

//////////////////////////////////////////////////////////////////////////
// SendMessageEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The data to set when sending remote message.
 *          Is used when sending message to remote process to dispatch and handle message.
 *          The remote message with data is created and set before sending message.
 *          The object is used when passing event to message sending threat for further processing.
 **/
class AREG_API SendMessageEventData
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the remote message buffer with the instruction for remote process.
     * \param   remoteMessage   The remote message object to initialize.
     *                          The message should already contain information and instructions
     *                          for remote process.
     **/
    SendMessageEventData( const RemoteMessage & remoteMessage );

    /**
     * \brief   Copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    SendMessageEventData( const SendMessageEventData & source );

    /**
     * \brief   Destructor
     **/
    ~SendMessageEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empties existing message buffer and copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    const SendMessageEventData & operator = ( const SendMessageEventData & source );

    /**
     * \brief   Returns instance of remote message.
     **/
    inline const RemoteMessage & getRemoteMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote message
     **/
    RemoteMessage mRemoteMessage;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SendMessageEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// SendMessageEvent and IESendMessageEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of SendMessageEvent event and IESendMessageEventConsumer consumer classes
DECLARE_EVENT(SendMessageEventData, SendMessageEvent, IESendMessageEventConsumer)

//////////////////////////////////////////////////////////////////////////
// SendMessageEventData class inline functions
//////////////////////////////////////////////////////////////////////////

inline const RemoteMessage & SendMessageEventData::getRemoteMessage( void ) const
{
    return mRemoteMessage;
}

#endif  // AREG_IPC_REMOTESERVICEEVENT_HPP
