#ifndef AREG_BASE_SOCKETMULTIPLEXER_HPP
#define AREG_BASE_SOCKETMULTIPLEXER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketMultiplexer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, persistent socket readiness monitor with
 *              graceful shutdown support on Linux, macOS, and Windows.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include <algorithm>
#include <atomic>
#include <vector>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// areg::SocketMultiplexer
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Persistent socket readiness monitor.
 *
 *          Sockets are registered once and monitored across multiple wait()
 *          calls without rebuilding a descriptor list on each call.
 *
 *          Platform-specific wait backend:
 *          - Linux   : \c epoll_wait() — O(1) regardless of socket count.
 *          - Windows : \c WSAPoll.
 *          - macOS / other POSIX : \c poll().
 *
 *          Shutdown-safe wakeup mechanism (all platforms):
 *          Calling reset() from any thread immediately unblocks a wait()
 *          call that is in progress on another thread — no timeouts needed.
 *          The wakeup is implemented with OS-native primitives:
 *          - Linux   : \c eventfd  (mWakeupReadFd == mWakeupWriteFd, same fd).
 *          - macOS / other POSIX : anonymous \c pipe.
 *          - Windows : loopback TCP socket pair.
 *          In all cases, reset() writes to mWakeupWriteFd and wait()
 *          monitors mWakeupReadFd for the signal.
 *
 *          The connection cap is configurable at construction time and
 *          enforced in register_socket().  Minimum and maximum values are
 *          clamped regardless of the caller-supplied argument.
 *
 *          Typical usage pattern for a server accept loop:
 *          \code
 *          // On server socket creation:
 *          mMultiplexer.register_socket( serverHandle );
 *
 *          // On each accepted client connection:
 *          mMultiplexer.register_socket( clientHandle );
 *
 *          // On connection close (before socket_close()):
 *          mMultiplexer.unregister_socket( clientHandle );
 *
 *          // In the accept loop:
 *          SOCKETHANDLE ready = mMultiplexer.wait( -1 );
 *          // ready == FailedSocketHandle  ->  reset() was called, stop.
 *          // ready == InvalidSocketHandle ->  timeout, re-check exit signal.
 *          // otherwise                    ->  socket is readable.
 *          \endcode
 *
 *          The legacy stateless wait() overload is retained for RTOS and
 *          embedded environments where epoll / kqueue are unavailable.
 **/
class AREG_API SocketMultiplexer
{
//////////////////////////////////////////////////////////////////////////
// Connection-count limits (same on every platform)
//////////////////////////////////////////////////////////////////////////
public:
    /// Floor applied to any caller-supplied max — prevents degenerate limits.
    static constexpr int32_t  MIN_CONNECTIONS     {    32 };

    /// Ceiling applied to any caller-supplied max — mtrouter is not a web server.
    static constexpr int32_t  MAX_CONNECTIONS     { 10000 };

    /// Default cap when no explicit value is supplied at construction.
    static constexpr int32_t  DEFAULT_CONNECTIONS {   128 };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the multiplexer and creates OS wakeup resources.
     *
     * \param   maxConnections  Maximum number of simultaneously monitored
     *                          sockets (server + clients).  Clamped to
     *                          [MIN_CONNECTIONS, MAX_CONNECTIONS].
     **/
    explicit SocketMultiplexer(int32_t maxConnections = DEFAULT_CONNECTIONS) noexcept;

    /**
     * \brief   Releases all OS resources (epoll fd, wakeup fd/pipe/sockets).
     **/
    ~SocketMultiplexer() noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Registers \a hSocket for readiness monitoring.
     *          Call when the server socket is created and for each accepted
     *          client connection.
     *
     * \param   hSocket     Valid socket descriptor to register.
     * \param   search      Flag indicating whether first should search the specified socket in the list or not.
     *                      If this flag is false, it will always register valid socket and return true.
     *                      Otherwise, it will first search the socket and add only if it is not registered.
     * \return  true on success; false if the socket is already registered,
     *          invalid, or the connection cap has been reached.
     **/
    bool register_socket(SOCKETHANDLE hSocket, bool search) noexcept;

    /**
     * \brief   Unregisters \a hSocket from monitoring.
     *          Call before the socket descriptor is closed.
     *
     * \param   hSocket     Previously registered socket descriptor.
     * \return  true on success; false if the socket was not registered.
     **/
    bool unregister_socket(SOCKETHANDLE hSocket) noexcept;

    /**
     * \brief   Removes all registered sockets and signals any thread
     *          blocked in wait() to return FailedSocketHandle immediately.
     *
     *          Safe to call from a different thread than the one currently
     *          blocking in wait().
     **/
    void reset() noexcept;

    /**
     * \brief   Blocks until one of the registered sockets becomes readable,
     *          \a timeoutMs elapses, or reset() is called from another thread.
     *
     * \param   timeoutMs   Milliseconds to wait; -1 blocks indefinitely until
     *                      a socket fires or reset() interrupts the call.
     * \return  Readable socket handle on success;
     *          \c InvalidSocketHandle on timeout;
     *          \c FailedSocketHandle when reset() was called or on error.
     **/
    SOCKETHANDLE wait(int32_t timeoutMs = static_cast<int32_t>(areg::WAIT_INFINITE)) const noexcept;

    /**
     * \brief   Stateless wait — builds a temporary poll list from the
     *          provided arguments without requiring prior registration.
     *          Provided for RTOS and embedded environments where epoll and
     *          kqueue are unavailable.
     *
     * \param   serverSocket    The listening server socket.  When this socket
     *                          fires, a new connection is ready to be accepted.
     * \param   clientSockets   Array of already-accepted connection handles
     *                          to monitor for incoming data.  May be nullptr
     *                          when \a count is zero.
     * \param   count           Number of entries in \a clientSockets.
     * \param   timeoutMs       Milliseconds to wait; -1 blocks indefinitely.
     * \return  The handle that became readable;
     *          \c InvalidSocketHandle on timeout;
     *          \c FailedSocketHandle on error or if \a serverSocket is invalid.
     **/
    SOCKETHANDLE wait( SOCKETHANDLE            serverSocket
                     , const SOCKETHANDLE *    clientSockets
                     , int32_t                 count
                     , int32_t                 timeoutMs = static_cast<int32_t>(areg::WAIT_INFINITE)) const;

    /**
     * \brief   Returns true if the specified socket already registered.
     * \param   hSocket     The socket handle to check.
     **/
    [[nodiscard]]
    inline bool is_registered(SOCKETHANDLE hSocket) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    std::vector<SOCKETHANDLE>   mSockets;       //!< Registered socket handles (growable).
    int32_t                     mMaxCount;       //!< Configurable connection cap.
    std::atomic<bool>           mIsReset;        //!< Set by reset(); cleared by register_socket(). Guards re-entry after wakeup is drained.
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

#if defined(__linux__)
    SOCKETHANDLE    mEpollFd;       //!< epoll instance fd (POSIX fd type, not a socket).
#endif

    /// Wakeup signal input: polled / added to epoll for readability.
    /// On Linux: eventfd handle (same value as mWakeupWriteFd).
    /// On macOS / POSIX: read end of an anonymous pipe.
    /// On Windows: accepted side of a loopback TCP socket pair.
    SOCKETHANDLE    mWakeupReadFd;

    /// Wakeup signal output: written to by reset() to interrupt wait().
    /// On Linux: same eventfd handle as mWakeupReadFd.
    /// On macOS / POSIX: write end of an anonymous pipe.
    /// On Windows: connected side of a loopback TCP socket pair.
    SOCKETHANDLE    mWakeupWriteFd;

//////////////////////////////////////////////////////////////////////////
// Forbidden
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketMultiplexer );
};

inline bool areg::SocketMultiplexer::is_registered(SOCKETHANDLE hSocket) const noexcept
{
    return std::find(mSockets.begin(), mSockets.end(), hSocket) != mSockets.end();
}

} // namespace areg

#endif  // AREG_BASE_SOCKETMULTIPLEXER_HPP
