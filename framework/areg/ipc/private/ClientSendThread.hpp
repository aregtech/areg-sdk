#ifndef AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
#define AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Send Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/SendMessageEvent.hpp"

#include <atomic>
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class RemoteMessageHandler;
class ClientConnection;

//////////////////////////////////////////////////////////////////////////
// ClientSendThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message sender thread that queues and sends all messages to remote routing service.
 **/
class ClientSendThread  : public    DispatcherThread
                        , public    SendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes message sender with service handler and client connection.
     *
     * \param   remoteService       Remote service handler for processing messages.
     * \param   connection          Client connection object for sending messages.
     * \param   namePrefix          Prefix for thread name to ensure uniqueness.
     **/
    ClientSendThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix );
    virtual ~ClientSendThread() = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulated sent data size and resets counter atomically. Useful for
     *          displaying data rate.
     **/
    inline uint32_t extract_data_send() const;

    /**
     * \brief   Enables or disables sent data calculation and resets existing calculated data.
     *
     * \param   enable      Flag indicating whether data calculation should be enabled.
     **/
    inline void set_data_rate_enabled(bool enable);

    /**
     * \brief   Returns whether data calculation is enabled.
     **/
    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables event dispatching. Override to perform preparation work.
     *
     * \param   is_ready     Flag indicating whether dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) override;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts and delivers event to target. Override in derived classes to process events.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true.
     **/
    bool post_event( Event & eventElem ) override;

private:
/************************************************************************/
// SendMessageEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Processes send message events dispatched by worker or component thread.
     *
     * \param   data    Send message event data to process.
     **/
    void process_event( const SendMessageEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    RemoteMessageHandler&     mRemoteService;
    /**
     * \brief   The instance of connection to send messages from remote routing service.
     **/
    ClientConnection &          mConnection;

    /**
     * \brief   Accumulative value of sent data size.
     **/
    mutable std::atomic_uint    mBytesSend;

    /**
     * \brief   Flag, indicating whether data calculation is enabled or disabled. By default, it is disabled.
     **/
    bool                        mSaveDataSend;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientSendThread() = delete;
    AREG_NOCOPY_NOMOVE( ClientSendThread );
};

inline uint32_t ClientSendThread::extract_data_send() const
{
    return static_cast<uint32_t>(mBytesSend.exchange( 0 ));
}

inline void ClientSendThread::set_data_rate_enabled(bool enable)
{
    if (mSaveDataSend != enable)
    {
        mBytesSend.store(0u);
        mSaveDataSend = enable;
    }
}

inline bool ClientSendThread::is_data_rate_enabled() const noexcept
{
    return mSaveDataSend;
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
