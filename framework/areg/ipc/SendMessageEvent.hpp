#ifndef AREG_IPC_SENDMESSAGEEVENT_HPP
#define AREG_IPC_SENDMESSAGEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/SendMessageEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Send message events
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventTemplate.hpp"
#include "areg/base/RemoteMessage.hpp"

#include <utility>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// SendMessageEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Wraps remote message data with command instruction for message sender thread.
 **/
class AREG_API SendMessageEventData
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The command to process when send message.
    enum class SendCommand  : uint8_t
    {
          ForwardMessage    //!< Forward message to target.
        , ExitThread        //!< Stop sending message and exit the thread.
        , InvalidMessage    //!< Ignore, the message is invalid
    };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    inline SendMessageEventData();

    /**
     * \brief   Initializes with remote message and forward instruction (copy).
     *
     * \param   remoteMessage       Remote message to initialize.
     **/
    inline explicit SendMessageEventData( const RemoteMessage & remoteMessage );

    /**
     * \brief   Initializes with remote message and forward instruction (move).
     *          Transfers ownership of the message payload without copying.
     *
     * \param   remoteMessage       Remote message to move from.
     **/
    inline explicit SendMessageEventData( RemoteMessage && remoteMessage ) noexcept;

    inline SendMessageEventData( const SendMessageEventData & source );

    inline SendMessageEventData( SendMessageEventData && source ) noexcept;

    ~SendMessageEventData()= default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:

    inline SendMessageEventData & operator = ( const SendMessageEventData & source );

    inline SendMessageEventData & operator = ( SendMessageEventData && source ) noexcept;

    /**
     * \brief   Returns the remote message instance.
     **/
    [[nodiscard]]
    inline const RemoteMessage & remote_message() const noexcept;

    [[nodiscard]]
    inline const ITEM_ID& message_target() const noexcept;

    [[nodiscard]]
    inline const ITEM_ID& message_source() const noexcept;

    /**
     * \brief   Returns the command instruction.
     **/
    [[nodiscard]]
    inline SendMessageEventData::SendCommand command() const noexcept;

    /**
     * \brief   Returns true if message has forward instruction.
     **/
    [[nodiscard]]
    inline bool is_forward_message() const noexcept;

    /**
     * \brief   Returns true if message has exit thread instruction.
     **/
    [[nodiscard]]
    inline bool is_exit_message() const noexcept;

    inline void set_remote_message(const RemoteMessage& message);

    inline void set_remote_message(RemoteMessage&& message) noexcept;

    inline void set_command(SendMessageEventData::SendCommand command) noexcept;

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
    SendCommand    mCmdSendMessage;
};

//////////////////////////////////////////////////////////////////////////
// SendMessageEvent and SendMessageEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration of SendMessageEvent event and SendMessageEventConsumer consumer classes
AREG_DECLARE_EVENT(SendMessageEventData, SendMessageEvent, SendMessageEventConsumer)

/**
 * \brief   Pending send entry : holds everything needed for deferred group-send.
 **/
struct PendingSend
{
    //!< Resolved socket handle for this message.
    SOCKETHANDLE        socket;
    //!< Owning event; destroyed after send.
    SendMessageEvent*   sendEvt;
};

//////////////////////////////////////////////////////////////////////////
// SendMessageEventData class inline functions
//////////////////////////////////////////////////////////////////////////

inline SendMessageEventData::SendMessageEventData(const RemoteMessage& remoteMessage)
    : mRemoteMessage    (remoteMessage)
    , mCmdSendMessage   ( SendMessageEventData::SendCommand::ForwardMessage )
{
}

inline SendMessageEventData::SendMessageEventData(RemoteMessage&& remoteMessage) noexcept
    : mRemoteMessage    ( std::move(remoteMessage) )
    , mCmdSendMessage   ( SendMessageEventData::SendCommand::ForwardMessage )
{
}

inline SendMessageEventData::SendMessageEventData()
    : mRemoteMessage    ( )
    , mCmdSendMessage   ( SendMessageEventData::SendCommand::ExitThread )
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
    source.mCmdSendMessage = SendCommand::InvalidMessage;
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
    source.mCmdSendMessage = SendCommand::InvalidMessage;
    return (*this);
}

inline const RemoteMessage & SendMessageEventData::remote_message() const noexcept
{
    return mRemoteMessage;
}

inline const ITEM_ID& SendMessageEventData::message_target() const noexcept
{
    return mRemoteMessage.target();
}

inline const ITEM_ID& SendMessageEventData::message_source() const noexcept
{
    return mRemoteMessage.source();
}

inline SendMessageEventData::SendCommand SendMessageEventData::command() const noexcept
{
    return mCmdSendMessage;
}

inline bool SendMessageEventData::is_forward_message() const noexcept
{
    return (mCmdSendMessage == SendCommand::ForwardMessage);
}

inline bool SendMessageEventData::is_exit_message() const noexcept
{
    return (mCmdSendMessage == SendCommand::ExitThread);
}

inline void SendMessageEventData::set_remote_message(const RemoteMessage& message)
{
    mRemoteMessage = message;
}

inline void SendMessageEventData::set_remote_message(RemoteMessage&& message) noexcept
{
    mRemoteMessage = std::move(message);
}

inline void SendMessageEventData::set_command(SendMessageEventData::SendCommand command) noexcept
{
    mCmdSendMessage = command;
}

} // namespace areg
#endif  // AREG_IPC_SENDMESSAGEEVENT_HPP
