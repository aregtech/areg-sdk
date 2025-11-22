#ifndef AREG_IPC_SENDMESSAGEEVENT_HPP
#define AREG_IPC_SENDMESSAGEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/SendMessageEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Send message events
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/RemoteMessage.hpp"

#include <utility>

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
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The command to process when send message.
    enum eSendMessage
    {
          MessageForward    //!< Forward message to target.
        , ExitThread        //!< Stop sending message and exit the thread.
    };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a message with instruction to stop sending message and exit thread.
     **/
    inline SendMessageEventData( void );

    /**
     * \brief   Sets the remote message buffer with the instruction to forward message
     *          to the target for further processing.
     * \param   remoteMessage   The remote message object to initialize.
     *                          The message should already contain information and instructions
     *                          for remote process.
     **/
    inline explicit SendMessageEventData( const RemoteMessage & remoteMessage );

    /**
     * \brief   Copies remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    inline SendMessageEventData( const SendMessageEventData & source );

    /**
     * \brief   Moves remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    inline SendMessageEventData( SendMessageEventData && source ) noexcept;

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
    inline SendMessageEventData & operator = ( const SendMessageEventData & source );

    /**
     * \brief   Empties existing message buffer and moves remote message data from given source.
     * \param   source  The source, which contains remote message.
     **/
    inline SendMessageEventData & operator = ( SendMessageEventData && source ) noexcept;

    /**
     * \brief   Returns instance of remote message.
     **/
    inline const RemoteMessage & getRemoteMessage( void ) const;

    /**
     * \brief   Returns the command instruction to handle messages.
     **/
    inline SendMessageEventData::eSendMessage getCommand( void ) const;

    /**
     * \brief   Returns true if message is with instruction to forward the message.
     **/
    inline bool isForwardMessage( void ) const;

    /**
     * \brief   Returns true if message is with instruction to quit the thread.
     **/
    inline bool isExitThreadMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote message
     **/
    RemoteMessage   mRemoteMessage;

    /**
     * \brief   The action to perform on the message.
     **/
    eSendMessage    mCmdSendMessage;
};

//////////////////////////////////////////////////////////////////////////
// SendMessageEvent and IESendMessageEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of SendMessageEvent event and IESendMessageEventConsumer consumer classes
DECLARE_EVENT(SendMessageEventData, SendMessageEvent, IESendMessageEventConsumer)

//////////////////////////////////////////////////////////////////////////
// SendMessageEventData class inline functions
//////////////////////////////////////////////////////////////////////////

inline SendMessageEventData::SendMessageEventData(const RemoteMessage& remoteMessage)
    : mRemoteMessage    (remoteMessage)
    , mCmdSendMessage   ( SendMessageEventData::eSendMessage::MessageForward )
{
}

inline SendMessageEventData::SendMessageEventData(void)
    : mRemoteMessage    ( )
    , mCmdSendMessage   ( SendMessageEventData::eSendMessage::ExitThread )
{
}

inline SendMessageEventData::SendMessageEventData( const SendMessageEventData & source )
    : mRemoteMessage    ( source.mRemoteMessage )
    , mCmdSendMessage   ( source.mCmdSendMessage )
{
}

inline SendMessageEventData::SendMessageEventData(SendMessageEventData&& source) noexcept
    : mRemoteMessage    ( std::move(source.mRemoteMessage) )
    , mCmdSendMessage   ( source.mCmdSendMessage )
{
}

inline SendMessageEventData& SendMessageEventData::operator = (const SendMessageEventData& source)
{
    mRemoteMessage  = source.mRemoteMessage;
    mCmdSendMessage = source.mCmdSendMessage;
    return (*this);
}

inline SendMessageEventData& SendMessageEventData::operator = (SendMessageEventData&& source) noexcept
{
    mRemoteMessage  = std::move(source.mRemoteMessage);
    mCmdSendMessage = source.mCmdSendMessage;
    return (*this);
}

inline const RemoteMessage & SendMessageEventData::getRemoteMessage( void ) const
{
    return mRemoteMessage;
}

inline SendMessageEventData::eSendMessage SendMessageEventData::getCommand( void ) const
{
    return mCmdSendMessage;
}

inline bool SendMessageEventData::isForwardMessage( void ) const
{
    return (mCmdSendMessage == eSendMessage::MessageForward);
}

inline bool SendMessageEventData::isExitThreadMessage( void ) const
{
    return (mCmdSendMessage == eSendMessage::ExitThread);
}

#endif  // AREG_IPC_SENDMESSAGEEVENT_HPP
