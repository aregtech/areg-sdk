#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SOCKETHANDOVER_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SOCKETHANDOVER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/SocketHandover.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, hands accepted sockets to the receive thread that serves them.
 *              Only the serving thread registers a socket in its multiplexer, unregisters it
 *              and drops its receive state; other threads queue the socket here and wake it.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <vector>

namespace areg {
    class SocketMultiplexer;
}

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// SocketRelease interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The owner of accepted sockets: the receive thread, or the connection pair, that
 *          serves them. Takes back a socket that left the connection maps.
 **/
class SocketRelease
{
protected:
    SocketRelease() = default;
    virtual ~SocketRelease() = default;

public:
    /**
     * \brief   Takes back the socket of a closed connection. Called by any thread, with no lock
     *          of the connection held. The socket is shut down already.
     *
     * \param   cookie  The cookie the connection had.
     * \param   socket  The socket of the connection.
     **/
    virtual void release_socket(ITEM_ID cookie, areg::SocketAccepted && socket) = 0;

private:
    AREG_NOCOPY_NOMOVE(SocketRelease);
};

//////////////////////////////////////////////////////////////////////////
// SocketHandover class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The queue of sockets a serving thread has to register or to release. Any thread
 *          adds to it and wakes the multiplexer of the serving thread; the serving thread takes
 *          the queue at the top of its loop.
 **/
class SocketHandover
{
public:
    /**
     * \brief   Creates the queue of the thread that waits on the given multiplexer.
     *
     * \param   mux     The multiplexer of the serving thread, woken on every add.
     **/
    explicit SocketHandover(areg::SocketMultiplexer & mux) noexcept;

    ~SocketHandover() = default;

    /**
     * \brief   Queues a socket the serving thread has to register.
     **/
    void add(const areg::SocketAccepted & socket);

    /**
     * \brief   Queues a socket the serving thread has to unregister and drop.
     **/
    void release(areg::SocketAccepted && socket);

    /**
     * \brief   Returns true when sockets are queued.
     **/
    [[nodiscard]]
    inline bool has_pending() const noexcept;

    /**
     * \brief   Moves the queued sockets out. Called by the serving thread only.
     *
     * \param[out]  added       The sockets to register.
     * \param[out]  released    The sockets to unregister and drop.
     **/
    void take(std::vector<areg::SocketAccepted> & added, std::vector<areg::SocketAccepted> & released);

private:
    areg::SocketMultiplexer &           mMux;       //!< The multiplexer woken on every add.
    std::atomic<bool>                   mPending;   //!< True while a queue is not empty.
    ResourceLock                        mLock;      //!< Guards both queues.
    std::vector<areg::SocketAccepted>   mAdded;     //!< Sockets queued for registration.
    std::vector<areg::SocketAccepted>   mReleased;  //!< Sockets queued for release.

private:
    SocketHandover() = delete;
    AREG_NOCOPY_NOMOVE(SocketHandover);
};

inline bool SocketHandover::has_pending() const noexcept
{
    return mPending.load(std::memory_order_acquire);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SOCKETHANDOVER_HPP
