#ifndef AREG_IPC_REMOTESERVICEEVENT_HPP
#define AREG_IPC_REMOTESERVICEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/RemoteServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
    explicit SendMessageEventData( const RemoteMessage & remoteMessage );

    /**
     * \brief   Copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    SendMessageEventData( const SendMessageEventData & source ) = default;

    /**
     * \brief   Moves remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    SendMessageEventData( SendMessageEventData && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~SendMessageEventData( void )= default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empties existing message buffer and copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    SendMessageEventData & operator = ( const SendMessageEventData & source );

    /**
     * \brief   Empties existing message buffer and moves remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    SendMessageEventData & operator = ( SendMessageEventData && source ) noexcept;

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
    SendMessageEventData( void ) = delete;
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
