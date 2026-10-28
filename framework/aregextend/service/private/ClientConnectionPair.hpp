#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "aregextend/service/private/PoolSendThread.hpp"
#include "aregextend/service/private/PoolReceiveThread.hpp"
#include "aregextend/service/private/SocketHandover.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ClientConnectionPair class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Aggregates one PoolSendThread and one PoolReceiveThread for a
 *          single pool slot.  Clients are routed to a slot via cookie % N.
 *          Managed by ServiceCommunicationBase via unique_ptr.
 *
 * OWNERSHIP NOTE: unique_ptr is used because the pool pair has a single, clear
 *          owner (ServiceCommunicationBase).  The pair is created at server start
 *          and destroyed at server stop.  Async cleanup paths (connection_lost,
 *          failed_receive_message) do not extend the pair's lifetime -- they only
 *          remove a single socket from the pair's multiplexer, not stop the pair.
 *          The pair serves the sockets of its slot: a closed connection is given back to it
 *          by release_socket(), and its receive thread drops it.
 **/
class ClientConnectionPair final  : public SocketRelease
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   An accepted socket of the slot, with its handle read without touching the
     *          reference counter of the socket.
     **/
    struct SlotSocket
    {
        SOCKETHANDLE    socket  { areg::InvalidSocketHandle };  //!< The socket handle.
        SocketAccepted  client  { };                            //!< The accepted socket.
    };

    /**
     * \brief   The container of the sockets of the slot where the keys are cookie values.
     **/
    using MapCookieToConnection = OrderedMap<ITEM_ID, SlotSocket>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates the send and receive threads for a pool slot.
     *          The threads are NOT started; call start() after construction.
     *
     * \param   remoteService   Remote message handler passed to both threads.
     * \param   connection      Server connection passed to both threads.
     * \param   globalSend      Global send thread for DataRateHelper stats.
     * \param   globalRecv      Global receive thread for DataRateHelper stats.
     * \param   sendName        Thread name for the PoolSendThread.
     * \param   recvName        Thread name for the PoolReceiveThread.
     **/
    ClientConnectionPair( areg::RemoteMessageHandler & remoteService
                        , ServerConnection & connection
                        , ServerSendThread & globalSend
                        , ServerReceiveThread & globalRecv
                        , std::string_view sendName
                        , std::string_view recvName );

    ~ClientConnectionPair() override = default;

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
     * \brief   Waits for the receive thread to exit, after stop_threads().
     **/
    inline void shutdown_receive(uint32_t timeout);

    /**
     * \brief   Queues an accepted socket for registration with the receive thread's
     *          multiplexer.  The receive thread picks it up at the next loop iteration.
     *
     * \param   clientSocket    Accepted client socket to start monitoring.
     **/
    inline void add_socket( const areg::SocketAccepted & clientSocket, const ITEM_ID cookie );

    /**
     * \brief   Removes the client of \a cookie from the connection map of the slot and queues
     *          its socket for the receive thread, which unregisters and drops it.
     *
     * \param   cookie  The cookie of the closed connection.
     * \param   socket  The socket of the closed connection.
     **/
    void release_socket(ITEM_ID cookie, areg::SocketAccepted && socket) override;

    /**
     * \brief   Returns a reference to the send thread for event posting.
     **/
    [[nodiscard]]
    inline PoolSendThread & send_thread() noexcept;

    [[nodiscard]]
    inline SOCKETHANDLE socket_by_cookie(const ITEM_ID& cookie) const noexcept;

    /**
     * \brief   Returns a reference to the socket of \a cookie, or an empty one if the slot has
     *          no such client. The socket stays open while the returned copy exists.
     **/
    [[nodiscard]]
    inline areg::Socket::SocketHandle hold_by_cookie(const ITEM_ID& cookie) const noexcept;

    /**
     * \brief   Resolves all \a count cookies to socket handles in one lock window, and keeps one
     *          reference to each socket found in \a holds.
     *          Mirrors ServerConnection::batch_handles_by_cookies for the per-slot map.
     *
     * \param   cookies     Array of \a count cookie values to look up.
     * \param   handles     Output array, same size as \a cookies.
     * \param   count       Number of entries to resolve.
     * \param   holds       Output array of the distinct sockets found; at least \a count elements.
     * \param   holdCount   On output, the number of entries of \a holds.
     **/
    inline void batch_sockets_by_cookies( const ITEM_ID * cookies
                                        , SOCKETHANDLE  * handles
                                        , uint32_t        count
                                        , HeldSocket * holds
                                        , uint32_t & holdCount ) const noexcept;

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
    PoolSendThread          mSendThread;    //!< Outbound message dispatcher for this pool slot.
    PoolReceiveThread       mReceiveThread; //!< Inbound message receiver for this pool slot.
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

inline PoolSendThread & ClientConnectionPair::send_thread() noexcept
{
    return mSendThread;
}

inline SOCKETHANDLE ClientConnectionPair::socket_by_cookie(const ITEM_ID& cookie) const noexcept
{
    Lock lock(mLock);
    MapCookieToConnection::MAPPOS pos{ mConnections.find(cookie) };
    return (mConnections.is_valid_position(pos) ? mConnections.value_at(pos).socket : areg::InvalidSocketHandle);
}

inline areg::Socket::SocketHandle ClientConnectionPair::hold_by_cookie(const ITEM_ID& cookie) const noexcept
{
    Lock lock(mLock);
    MapCookieToConnection::MAPPOS pos{ mConnections.find(cookie) };
    return (mConnections.is_valid_position(pos) ? mConnections.value_at(pos).client.shared_handle() : areg::Socket::SocketHandle());
}

inline void ClientConnectionPair::batch_sockets_by_cookies( const ITEM_ID * cookies
                                                           , SOCKETHANDLE  * handles
                                                           , uint32_t        count
                                                           , HeldSocket * holds
                                                           , uint32_t & holdCount ) const noexcept
{
    holdCount = 0u;
    Lock lock(mLock);
    for (uint32_t i = 0u; i < count; ++i)
    {
        MapCookieToConnection::MAPPOS pos{ mConnections.find(cookies[i]) };
        if (mConnections.is_valid_position(pos))
        {
            const SlotSocket & socket{ mConnections.value_at(pos) };
            handles[i] = socket.socket;
            areg::ext::hold_socket(socket.socket, socket.client.shared_handle(), holds, holdCount);
        }
        else
        {
            handles[i] = areg::InvalidSocketHandle;
        }
    }
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

inline void ClientConnectionPair::shutdown_receive(uint32_t timeout)
{
    mReceiveThread.shutdown(timeout);
}

inline void ClientConnectionPair::add_socket(const areg::SocketAccepted& clientSocket, const ITEM_ID cookie)
{
    Lock lock(mLock);
    mConnections[cookie] = SlotSocket{ clientSocket.handle(), clientSocket };
    mReceiveThread.add_socket(clientSocket);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_CLIENTCONNECTIONPAIR_HPP
