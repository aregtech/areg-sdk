#ifndef AREG_IPC_PRIVATE_CEREMOTESERVICEEVENT_HPP
#define AREG_IPC_PRIVATE_CEREMOTESERVICEEVENT_HPP

/************************************************************************
 * \file        areg/src/ipc/private/CERemoteServiceEvent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Send message events
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/TEEvent.hpp"
#include "areg/src/base/CERemoteMessage.hpp"

//////////////////////////////////////////////////////////////////////////
// CESendMessageEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The data to set when sending remote message.
 *          Is used when sending message to remote process to dispatch and handle message.
 *          The remote message with data is created and set before sending message.
 *          The object is used when passing event to message sending threat for further processing.
 **/
class CESendMessageEventData
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
    CESendMessageEventData( const CERemoteMessage & remoteMessage );

    /**
     * \brief   Copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    CESendMessageEventData( const CESendMessageEventData & source );

    /**
     * \brief   Destructor
     **/
    ~CESendMessageEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empties existing message buffer and copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    const CESendMessageEventData & operator = ( const CESendMessageEventData & source );

    /**
     * \brief   Returns instance of remote message.
     **/
    inline const CERemoteMessage & GetRemoteMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote message
     **/
    CERemoteMessage mRemoteMessage;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESendMessageEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// CESendMessageEvent and IESendMessageEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of CESendMessageEvent event and IESendMessageEventConsumer consumer classes
DECLARE_EVENT(CESendMessageEventData, CESendMessageEvent, IESendMessageEventConsumer)

//////////////////////////////////////////////////////////////////////////
// CESendMessageEventData class inline functions
//////////////////////////////////////////////////////////////////////////

inline const CERemoteMessage & CESendMessageEventData::GetRemoteMessage( void ) const
{   return mRemoteMessage;      }

#endif  // AREG_IPC_PRIVATE_CEREMOTESERVICEEVENT_HPP
