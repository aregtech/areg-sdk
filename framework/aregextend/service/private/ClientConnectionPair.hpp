#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ClientConnectionPair.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool thread pair: one send thread + one receive thread
 *              serving all clients routed to a single pool slot.
 *              Clients are added and removed dynamically without stopping the pair.
 *              Owned exclusively by ServiceCommunicationBase via unique_ptr.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/SocketAccepted.hpp"
#include "aregextend/service/private/ClientSendThread.hpp"
#include "aregextend/service/private/ClientReceiveThread.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ClientConnectionPair class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Aggregates one ClientSendThread and one ClientReceiveThread for a
 *          single pool slot.  Clients are routed to a slot via cookie % N.
 *          Managed by ServiceCommunicationBase via unique_ptr.
 *
 * OWNERSHIP NOTE: unique_ptr is used because the pool pair has a single, clear
 *          owner (ServiceCommunicationBase).  The pair is created at server start
 *          and destroyed at server stop.  Async cleanup paths (connection_lost,
 *          failed_receive_message) do not extend the pair's lifetime — they only
 *          remove a single socket from the pair's multiplexer, not stop the pair.
 **/
class ClientConnectionPair
{
    /**
     * \brief   The container of socket handles where the keys are cookie values.
     **/
    using MapCookieToConnection = OrderedMap<ITEM_ID, SocketAccepted>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates the send and receive threads for a pool slot.
     *          The threads are NOT started; call start() after construction.
     *
     * \param   connectHandler  Connection handler passed to receive thread.
     * \param   remoteService   Remote message handler passed to both threads.
     * \param   connection      Server connection passed to both threads.
     * \param   globalSend      Global send thread for DataRateHelper stats.
     * \param   globalRecv      Global receive thread for DataRateHelper stats.
     * \param   sendName        Thread name for the ClientSendThread.
     * \param   recvName        Thread name for the ClientReceiveThread.
     **/
    ClientConnectionPair( areg::ext::ConnectionHandler & connectHandler
                        , areg::RemoteMessageHandler & remoteService
                        , ServerConnection & connection
                        , ServerSendThread & globalSend
                        , ServerReceiveThread & globalRecv
                        , std::string_view sendName
                        , std::string_view recvName );

    ~ClientConnectionPair() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Starts both the send and the receive threads.
     *          Returns true only if both threads started successfully.
     **/
    bool start();

    /**
     * \brief   Signals both threads to stop and waits for them to finish.
     *          Safe to call from any thread other than the receive thread itself.
     **/
    void stop();

    /**
     * \brief   Signals the send thread to stop and waits for it to finish.
     **/
    void stop_send();

    /**
     * \brief   Signals the receive thread to stop (hard-resets the multiplexer)
     *          and waits for it to finish.
     *          Must NOT be called from inside the receive thread (deadlock risk).
     **/
    void stop_recv();

    inline void stop_threads();

    inline void shutdown_threads(uint32_t timeout);

    /**
     * \brief   Queues an accepted socket for registration with the receive thread's
     *          multiplexer.  The receive thread picks it up at the next loop iteration.
     *
     * \param   clientSocket    Accepted client socket to start monitoring.
     **/
    inline void add_socket( const areg::SocketAccepted & clientSocket, const ITEM_ID cookie );

    /**
     * \brief   Removes the client identified by \a cookie from the local connection
     *          map and queues its socket handle for removal from the receive thread's
     *          multiplexer.  The receive thread processes the removal at the next
     *          loop iteration.
     *
     * \param   cookie  The cookie assigned to the client being disconnected.
     *                  The cookie is already known at every call site (connection_lost),
     *                  so passing it directly avoids an O(N) reverse-lookup scan.
     **/
    inline void remove_socket(ITEM_ID cookie);

    /**
     * \brief   Enables or disables data rate tracking in both threads.
     *
     * \param   enable  True to enable; false to disable.
     **/
    inline void set_data_rate_enabled(bool enable) noexcept;

    /**
     * \brief   Returns a reference to the send thread for event posting.
     **/
    [[nodiscard]]
    inline ClientSendThread & send_thread() noexcept;

    [[nodiscard]]
    inline SOCKETHANDLE socket_by_cookie(const ITEM_ID& cookie) const noexcept;

    [[nodiscard]]
    inline SocketAccepted client_by_cookie(const ITEM_ID& cookie) const noexcept;

    inline uint64_t bytes_received() const noexcept;

    inline uint32_t messages_received() const noexcept;

    inline uint64_t bytes_sent() const noexcept;

    inline uint32_t messages_sent() const noexcept;

    inline void stat_received(uint64_t& bytesReceived, uint32_t& msgReceived) const noexcept;

    inline void stat_sent(uint64_t& bytesSend, uint32_t& msgSent) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden mehods
//////////////////////////////////////////////////////////////////////////
private:
    [[nodiscard]]
    inline ClientConnectionPair& self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientSendThread        mSendThread;    //!< Outbound message dispatcher for this pool slot.
    ClientReceiveThread     mReceiveThread; //!< Inbound message receiver for this pool slot.
    MapCookieToConnection   mConnections;   //!< Maps client cookies to accepted socket connections for this pool slot.
    mutable ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientConnectionPair() = delete;
    AREG_NOCOPY_NOMOVE( ClientConnectionPair );
};

//////////////////////////////////////////////////////////////////////////
// ClientConnectionPair inline methods
//////////////////////////////////////////////////////////////////////////

inline ClientConnectionPair& ClientConnectionPair::self() noexcept
{
    return (*this);
}

inline void ClientConnectionPair::set_data_rate_enabled(bool enable) noexcept
{
    mSendThread.set_data_rate_enabled(enable);
    mReceiveThread.set_data_rate_enabled(enable);
}

inline ClientSendThread & ClientConnectionPair::send_thread() noexcept
{
    return mSendThread;
}

inline SOCKETHANDLE ClientConnectionPair::socket_by_cookie(const ITEM_ID& cookie) const noexcept
{
    Lock lock(mLock);
    SocketAccepted client;
    return (mConnections.find(cookie, client) ? client.handle() : areg::InvalidSocketHandle);
}

inline SocketAccepted ClientConnectionPair::client_by_cookie(const ITEM_ID& cookie) const noexcept
{
    Lock lock(mLock);
    SocketAccepted client;
    mConnections.find(cookie, client);
    return client;
}

inline uint64_t ClientConnectionPair::bytes_received() const noexcept
{
    return mReceiveThread.bytes_received();
}

inline uint32_t ClientConnectionPair::messages_received() const noexcept
{
    return mReceiveThread.messages_received();
}

inline uint64_t ClientConnectionPair::bytes_sent() const noexcept
{
    return mSendThread.bytes_sent();
}

inline uint32_t ClientConnectionPair::messages_sent() const noexcept
{
    return mSendThread.messages_sent();
}

inline void ClientConnectionPair::stat_received(uint64_t& bytesReceived, uint32_t& msgReceived) const noexcept
{
    mReceiveThread.data_stat(bytesReceived, msgReceived);
}

inline void ClientConnectionPair::stat_sent(uint64_t& bytesSent, uint32_t& msgSent) const noexcept
{
    mSendThread.data_stat(bytesSent, msgSent);
}

inline void ClientConnectionPair::stop_threads()
{
    mReceiveThread.request_stop();
    mSendThread.trigger_exit();
}

inline void ClientConnectionPair::shutdown_threads(uint32_t timeout)
{
    mSendThread.shutdown(timeout);
    mReceiveThread.shutdown(timeout);
}

inline void ClientConnectionPair::add_socket(const areg::SocketAccepted& clientSocket, const ITEM_ID cookie)
{
    Lock lock(mLock);
    mConnections[cookie] = clientSocket;
    mReceiveThread.add_socket(clientSocket);
}

inline void ClientConnectionPair::remove_socket(ITEM_ID cookie)
{
    Lock lock(mLock);
    SocketAccepted client;
    if ( mConnections.find(cookie, client) )
    {
        mReceiveThread.remove_socket(client.handle());
        mConnections.remove_at(cookie);
    }
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
