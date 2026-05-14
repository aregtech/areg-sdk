#ifndef AREG_BASE_SOCKETDEFS_HPP
#define AREG_BASE_SOCKETDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS-independent socket wrapper: address management,
 *              connection helpers, and data transfer utilities for TCP/IP (IPv4).
 *
 * \note        Currently only TCP/IP IPv4 connections are supported.
 *              All other connection types are out of scope.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"

#include <memory>
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
struct sockaddr_in;

namespace areg {
    class Socket;
    class SocketMultiplexer;
} // namespace areg

namespace areg {
//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Stores an IPv4 address and port number for a socket endpoint.
 *          Resolves host names to IP addresses and extracts peer addresses
 *          from connected sockets.
 **/
class AREG_API SocketAddress
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SocketAddress();

    SocketAddress(const String& address, uint16_t portNr);

    SocketAddress(const SocketAddress& source);

    SocketAddress(SocketAddress&& source) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    SocketAddress& operator = (const SocketAddress& source);

    SocketAddress& operator = (SocketAddress&& source) noexcept;

    /**
     * \brief   Returns true if both address and port are identical.
     **/
    [[nodiscard]] bool operator == (const SocketAddress& other) const;

    /**
     * \brief   Returns true if either address or port differs.
     **/
    [[nodiscard]] bool operator != (const SocketAddress& other) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Fills \a sockAddr with the IPv4 address and port stored in this object.
     *
     * \param[out]  sockAddr    Output sockaddr_in structure to populate.
     * \return  true on success; false if the stored address is invalid.
     **/
    bool to_sockaddr(struct sockaddr_in& sockAddr) const;

    /**
     * \brief   Sets the IP address and port from a sockaddr_in structure.
     *
     * \param[in]   sockAddr    Source sockaddr_in structure.
     **/
    void from_sockaddr(const struct sockaddr_in& sockAddr);

    /**
     * \brief   Resolves \a hostName to an IPv4 address and stores it together
     *          with \a portNr.  If \a hostName is nullptr or empty, the
     *          loopback address is used for server sockets; the wildcard
     *          address is used for client sockets.
     *
     * \param   hostName    Numeric IP or human-readable host name to resolve.
     * \param   portNr      Port number to store.
     * \param   isServer    Pass true when binding a server socket, false for
     *                      a client connection.
     * \return  true if resolution succeeded; false otherwise.
     **/
    bool resolve_address(const String& hostName, uint16_t portNr, bool isServer);

    /**
     * \brief   Reads the peer address of the connected socket \a hSocket
     *          and stores the result in this object.
     *
     * \param   hSocket     Descriptor of an already-connected socket.
     * \return  true if the address was retrieved successfully; false otherwise.
     **/
    bool resolve_socket(SOCKETHANDLE hSocket);

    /**
     * \brief   Returns true if the stored address equals \a host and \a port.
     **/
    [[nodiscard]]
    bool is_equal(const String& host, uint16_t port) const noexcept;

    /**
     * \brief   Returns the numeric IPv4 address string (e.g. "192.168.1.1").
     **/
    [[nodiscard]]
    inline const String& host_address() const noexcept;

    /**
     * \brief   Returns the resolved host name string.
     **/
    [[nodiscard]]
    inline const String& host_name() const noexcept;

    /**
     * \brief   Returns the port number.
     **/
    [[nodiscard]]
    inline uint16_t host_port() const noexcept;

    /**
     * \brief   Clears the stored address and resets the port to InvalidPort.
     **/
    inline void reset() noexcept;

    /**
     * \brief   Returns true if the IP address is non-empty and the port is
     *          not InvalidPort.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    String   mIpAddr;   //!< Numeric IPv4 address string.
    String   mHostName; //!< Resolved host name string.
    uint16_t mPortNr;   //!< Port number; InvalidPort when not set.
};

//////////////////////////////////////////////////////////////////////////
// areg::UserData
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Stores a user name and password pair for authenticated connections.
 **/
class AREG_API UserData
{
//////////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////////
public:
    UserData();

    UserData(const String& user, const String& password);

    UserData(const UserData& src);

    UserData(UserData&& src) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    UserData& operator = (const UserData& source);

    UserData& operator = (UserData&& source) noexcept;

    /**
     * \brief   Returns true if both user name and password are identical.
     **/
    [[nodiscard]]
    bool operator == (const UserData& other) const noexcept;

    /**
     * \brief   Returns true if user name or password differs.
     **/
    [[nodiscard]]
    bool operator != (const UserData& other) const noexcept;

    /**
     * \brief   Returns the user name.
     **/
    [[nodiscard]]
    const String& user() const noexcept;

    /**
     * \brief   Sets the user name.
     **/
    void set_user(const String& user);

    /**
     * \brief   Returns the password.
     **/
    [[nodiscard]]
    const String& password() const noexcept;

    /**
     * \brief   Sets the password.
     **/
    void set_password(const String& password);

    /**
     * \brief   Returns true if the user name is not empty.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    String mUser;       //!< User name.
    String mPassword;   //!< User password.
};

//////////////////////////////////////////////////////////////////////////
// areg namespace constants
//////////////////////////////////////////////////////////////////////////

//!< Invalid socket descriptor.
constexpr SOCKETHANDLE  InvalidSocketHandle     { static_cast<SOCKETHANDLE>(~0) };

//!< Sentinel returned by server-accept failures (distinct from InvalidSocketHandle).
constexpr SOCKETHANDLE  FailedSocketHandle      { static_cast<SOCKETHANDLE>(~1) };

//!< Sentinel for an unset or invalid port number.
constexpr uint16_t      InvalidPort             { 0 };

//!< Loopback host name.
constexpr std::string_view LocalHost            { "localhost" };

//!< Loopback IPv4 address.
constexpr std::string_view LocalAddress         { "127.0.0.1" };

//!< Separator character between IPv4 octets.
constexpr char          IP_SEPARATOR            { '.' };

//!< Default IP-Address of routing service.
constexpr std::string_view  DEFAULT_ROUTER_HOST { LocalAddress };

//!< Default connection port number of routing service.
constexpr uint16_t          DEFAULT_ROUTER_PORT { 8181 };

//!< Default IP-Address of routing service.
constexpr std::string_view  DEFAULT_LOGGER_HOST { areg::LocalAddress };

//!< Default connection port number of routing service.
constexpr uint16_t      DEFAULT_LOGGER_PORT     { 8282 };

//!< Buffer size required to hold any IPv4 address string (e.g. "255.255.255.255\0").
constexpr uint32_t      IP_ADDRESS_SIZE         { 16u };

//!< Maximum payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_MAX_SIZE         { 65536u };

//!< Minimum payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_MIN_SIZE         { 4096u };

//!< Default payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_DEFAULT_SIZE     { PACKET_MAX_SIZE };

//!< Sentinel indicating an invalid or uninitialized packet size.
constexpr uint32_t      PACKET_INVALID_SIZE     { 0u };

//!< Compile-time default for SO_SNDBUF applied to every new socket.
//!< 16 MB (Linux kernel doubles to ~32 MB) gives ~10–15 large frames in-flight,
//!< which prevents TCP send stalls on a high-throughput image pipeline (800+ blocks/sec).
//!< Applied on all platforms including Windows (SO_SNDBUF does NOT disable autotuning;
//!< only SO_RCVBUF does that on Windows Vista+).
//!< Override at runtime via net::SERVICE::TRANSPORT::sndbuf in areg.init (value in KB).
constexpr uint32_t      SOCKET_SEND_BUFFER_SIZE { 4u * areg::ONE_MEGABYTE };

//!< Compile-time default for SO_RCVBUF applied on Linux and macOS.
//!< Override at runtime via net::SERVICE::TRANSPORT::rcvbuf in areg.init (value in KB).
constexpr uint32_t      SOCKET_RECV_BUFFER_SIZE { 4u * areg::ONE_MEGABYTE };

//!< Maximum milliseconds a single send() call may block waiting for TCP send-window space.
//!< When the peer's receive window is zero (e.g. OOM pressure or a slow/stalled receiver),
//!< a blocking send() can hang for tens of seconds.  This timeout caps that wait so the send
//!< thread detects the dead/slow peer within SOCKET_SEND_TIMEOUT_MS and tears down the
//!< connection through the normal failed_send_message path.
//!<
//!< Choosing the right value is a trade-off:
//!<   - Too large: high-rate pipelines (e.g. 380 fps × 3 MB) accumulate
//!<     timeout_ms × rate × msg_size bytes in the ServerSendThread queue before the
//!<     connection is closed.  At 5000 ms × 380 fps × 3 MB ≈ 5.7 GB -- OOM.
//!<   - Too small: transient OS scheduling jitter (GC, CPU starvation) may cause
//!<     spurious disconnects on otherwise healthy LANs.
//!<
//!< 500 ms is a safe default for loopback and LAN:
//!<   - Limits worst-case queue growth to ~570 MB at 1.14 GB/s inflow (380 fps × 3 MB).
//!<   - Tolerates typical OS scheduling pauses (< 100 ms) without false disconnects.
//!< Increase for very slow WAN links or when large receiver-side processing spikes are expected.
constexpr uint32_t      SOCKET_SEND_TIMEOUT_MS  { 2500u };

//!< Floor applied to any caller-supplied max -- prevents degenerate limits.
constexpr uint32_t      MIN_CONNECTIONS         { 32u };

//!< Ceiling applied to any caller-supplied max -- mtrouter is not a web server.
constexpr uint32_t      MAX_CONNECTIONS         { 10000u };

//!< Default socket capacity: initial mSockets.reserve() size and the stack-allocation
//!< threshold for the WSAPOLLFD / pollfd array in SocketMultiplexer::wait().
//!< 128 covers the common mtrouter deployment (up to ~100 simultaneous service channels)
//!< while keeping the stack frame under 4 KB (128 × WSAPOLLFD = ~1 KB).
//!< Must be >= BATCH_SIZE.
constexpr uint32_t      DEFAULT_CONNECTIONS     { 128u };

//!< Number of socket events fetched from the OS in a single epoll_wait / kevent / WSAPoll
//!< syscall.  Raising this reduces per-client syscall overhead under burst load.
//!< Optimal when BATCH_SIZE == THREAD_DRAIN_LIMIT: one OS call covers exactly one drain pass.
constexpr uint32_t      BATCH_SIZE              { 128u };

//!< Number of additional sockets (receive) or events (send) drained per dispatcher wake-up
//!< before returning to the blocking wait.  One drain pass processes exactly one OS-level
//!< batch, so setting THREAD_DRAIN_LIMIT == BATCH_SIZE eliminates stale-batch residuals.
//!< Tested optimal on both native Linux and WSL2: higher values increase CPU load without
//!< proportional throughput gain; lower values under-utilise the OS batch.
constexpr uint32_t      THREAD_DRAIN_LIMIT      { BATCH_SIZE };

//!< Size of the IoBuffer / iovec stack array used for scatter-gather send (writev / WSASend).
//!< Equals THREAD_DRAIN_LIMIT: slot 0 holds the triggering message (owned by the dispatch
//!< chain), slots 1..THREAD_DRAIN_LIMIT-1 hold at most THREAD_DRAIN_LIMIT-1 drained events.
//!< Total batch is exactly THREAD_DRAIN_LIMIT entries.  128 × sizeof(iovec) = 2 KB -- L1-friendly.
constexpr uint32_t      THREAD_BATCH_LIMIT      { THREAD_DRAIN_LIMIT };

//!< Maximum number of events queued in any send thread (ServerSendThread, PoolSendThread, areg::ClientSendThread).
//!< 0 = unlimited: the queue grows without bound and never drops messages.
//!< A non-zero cap protects against OOM under sustained TCP back-pressure
//!< (e.g. WSL2 vCPU throttling) at the cost of silently dropping the newest messages.
//!< 1024 provides ~30 ms of pipeline depth at 32K msg/s per send thread while
//!< preventing unbounded RAM growth when the receiver is slower than the sender.
constexpr uint32_t      SEND_THREAD_QUEUE_LIMIT { 0u };

//!< Default send/drain batch size; configurable via net::*::tcpip::batch in areg.init.
//!< Must equal THREAD_DRAIN_LIMIT to cover exactly one drain pass per OS wake-up.
constexpr uint32_t      DEFAULT_BATCH_SIZE          { BATCH_SIZE };

//!< Default thread-pool pair count; configurable via net::*::tcpip::pairs in areg.init.
//!< 0 = no pool (shared send/receive threads only).
constexpr uint32_t      DEFAULT_POOL_PAIRS          { 0u };

//!< Default thread-local receive-cache size in KB.
//!< Used by the client-side receive path (Cached mode, ClientReceiveThread).
//!<
//!< Client receive read-ahead (Cached mode):
//!<   Each Phase-2 greedy recv() captures up to DEFAULT_THREAD_CACHE_KB of data in one
//!<   syscall, then serves subsequent header and body reads from the in-memory cache.
//!<   Larger values reduce recv() syscall frequency proportionally.
//!<   At 3 KB messages: 512 KB / 3 KB ≈ 170 messages served per single recv() call.
//!<
//!< Configurable at runtime via net::*::tcpip::cache in areg.init (value in KB).
//!< This constant is the compile-time default returned when no config entry is present.
constexpr const uint32_t    DEFAULT_THREAD_CACHE_KB{ 256 };

//!< Maximum biUsed value accepted in any received MessageHeader.
//!< Requests larger than this are treated as protocol violations and rejected without
//!< allocation.  Prevents unbounded heap growth from corrupt or malicious packets.
//!< 64 MB covers the largest legitimate areg-sdk message payload; raise if needed.
constexpr uint32_t          MAX_MESSAGE_DATA_SIZE   { 64u * areg::ONE_MEGABYTE };

/**
 * \brief   Maximum number of pending connections the OS will queue on a
 *          listening server socket (SOMAXCONN).  Initialized at runtime.
 **/
extern AREG_API const int32_t MAXIMUM_LISTEN_QUEUE_SIZE /*= SOMAXCONN*/;

AREG_API uint32_t   thread_cache_size() noexcept;

struct ThreadCache
{
    static constexpr uint32_t THREAD_CACHE_SIZE{ areg::DEFAULT_THREAD_CACHE_KB * areg::ONE_KILOBYTE };

    SOCKETHANDLE socket{ areg::InvalidSocketHandle };   //!< Socket whose data occupies [head, head+unread)
    uint32_t     head   { 0u };                         //!< Read cursor: index of next byte to serve to caller
    uint32_t     unread { 0u };                         //!< Unread cached bytes: valid bytes at [head, head+unread)
    uint32_t     space  { 0u };                         //!< total space in buffer
    std::unique_ptr<uint8_t[]> buffer{ nullptr };       //!< initialize by need

    inline uint8_t* cache() noexcept
    {
        if (buffer == nullptr)
        {
            space = areg::thread_cache_size();
            head  = 0u;
            unread= 0u;
            buffer.reset(new (std::nothrow) uint8_t[space]);
        }

        return buffer.get();
    }
};

/**
 * \brief   Selects the receive strategy for the current thread.
 *          Exact mode never speculatively reads beyond the caller request.
 *          Cached mode enables thread-local read-ahead for single-socket
 *          receive loops to reduce recv() syscall frequency.
 **/
enum class ReceiveMode : uint8_t
{
      NoCache       = 0
    , MonoCache     = 1
    , MultiCache    = 2
};

//////////////////////////////////////////////////////////////////////////
// areg::IoBuffer -- scatter/gather I/O descriptor
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Describes one contiguous region in a scatter/gather write list.
 *          Used by send_data_v() to combine multiple buffers into a single
 *          socket syscall (writev on POSIX, WSASend on Windows).
 **/
struct IoBuffer
{
    const uint8_t* data;   //!< Pointer to the start of the data region.
    std::size_t     size;   //!< Number of bytes to send from this region.
};

//////////////////////////////////////////////////////////////////////////
// areg namespace free functions
//////////////////////////////////////////////////////////////////////////

// areg::SocketMultiplexer is defined in areg/base/SocketMultiplexer.hpp
// (included above).

//////////////////////////////////////////////////////////////////////////
// areg namespace free functions
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Returns true if \a hSocket is a usable descriptor (not
 *          InvalidSocketHandle and not FailedSocketHandle).
 **/
[[nodiscard]]
inline bool is_valid_socket(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Returns the thread-local TX staging cache for the calling thread.
 *          Socket-independent: within a single thread only one socket sends at a time.
 *          Used exclusively by the Windows coalesced-send path (_os_send_data_v).
 *          Controller threads that send single messages bypass the staging buffer
 *          entirely (count == 1 fast-path in _os_send_data_v).
 **/
[[nodiscard]]
AREG_API ThreadCache& thread_tx_cache() noexcept;

/**
 * \brief   Returns the thread-local RX cache for \a hSocket.
 *          One ThreadCache per (thread, socket) pair held in a thread-local map.
 *          The cache buffer is lazily allocated on first use via ThreadCache::cache().
 *          Threads that operate in ReceiveMode::Exact never allocate the buffer.
 *
 * \param   hSocket     Socket whose receive cache is requested.
 **/
[[nodiscard]]
AREG_API ThreadCache& thread_rx_cache(SOCKETHANDLE hSocket) noexcept;

AREG_API void thread_rx_cache_release(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Sets receive strategy of the calling thread.
 **/
AREG_API void set_receive_mode(ReceiveMode mode) noexcept;

/**
 * \brief   Returns receive strategy of the calling thread.
 **/
[[nodiscard]]
AREG_API ReceiveMode receive_mode() noexcept;

/**
 * \brief   Initializes the socket subsystem for the current process.
 *          Subsequent calls are reference-counted until socket_release()
 *          is called an equal number of times.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool socket_initialize() noexcept;

/**
 * \brief   Decrements the socket subsystem reference count; releases
 *          resources when it reaches zero.
 **/
AREG_API void socket_release() noexcept;

/**
 * \brief   Creates a streaming TCP/IP socket.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE socket_create() noexcept;

/**
 * \brief   Closes \a hSocket.  No further I/O is possible afterward.
 **/
AREG_API void socket_close(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Applies the default socket-buffer policy to \a hSocket immediately
 *          after creation or acceptance.
 *
 *          Sets SO_SNDBUF to SOCKET_SEND_BUFFER_SIZE on all platforms.
 *          Linux and macOS also set SO_RCVBUF to SOCKET_RECV_BUFFER_SIZE.
 *          Windows intentionally leaves SO_RCVBUF unchanged: setting it on a connected
 *          or accepted socket disables SIO_LOOPBACK_FAST_PATH and drops loopback
 *          throughput from 2+ GB/s to ~300 MB/s.
 *
 * \param   hSocket     Valid socket descriptor to configure.
 **/
AREG_API void socket_configure(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Disables the Nagle algorithm (TCP_NODELAY) on a connected socket.
 *          Also applies platform-specific keepalive and broken-pipe handling.
 *          Call this only on client or accepted sockets, never on listening sockets.
 *
 * \param   hSocket     Valid connected socket descriptor.
 **/
AREG_API void socket_set_no_delay(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Creates a TCP/IP client socket and connects to \a peerAddr.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE client_connect(const SocketAddress& peerAddr);

/**
 * \brief   Creates a TCP/IP client socket and connects to \a hostName:\a portNr.
 *
 * \param   hostName    Host name or numeric IPv4 address of the remote server.
 * \param   portNr      Remote port number.
 * \param   socketAddr  If not nullptr, receives the resolved remote address.
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE client_connect(const String& hostName, uint16_t portNr, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Creates a TCP/IP server socket and binds to \a peerAddr.
 *          Call server_listen() before accepting connections.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE server_connect(const SocketAddress& peerAddr);

/**
 * \brief   Creates a TCP/IP server socket and binds to \a hostName:\a portNr.
 *          Call server_listen() before accepting connections.
 *
 * \param   hostName    Host name or numeric IPv4 address to bind to.
 * \param   portNr      Port number to bind to.
 * \param   socketAddr  If not nullptr, receives the bound local address.
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE server_connect(const String& hostName, uint16_t portNr, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Places \a serverSocket into listening mode.
 *
 * \param   serverSocket    Bound server socket descriptor.
 * \param   maxQueueSize    Maximum number of pending connections the OS will
 *                          queue (default: MAXIMUM_LISTEN_QUEUE_SIZE).
 * \return  true if the socket is now listening; false otherwise.
 **/
AREG_API bool server_listen(SOCKETHANDLE serverSocket, int32_t maxQueueSize = areg::MAXIMUM_LISTEN_QUEUE_SIZE) noexcept;

/**
 * \brief   Waits via the persistent \a multiplexer, then accepts a pending
 *          client connection if the server socket fired.
 *
 *          All sockets must have been registered with \a multiplexer before
 *          calling this overload.  On Linux this uses epoll_wait() instead
 *          of poll(), giving O(1) readiness notification.
 *
 * \param   multiplexer     Persistent multiplexer with the server and client
 *                          sockets already registered.
 * \param   serverSocket    The listening server socket descriptor.
 * \param   socketAddr      If not nullptr, receives the new client's address
 *                          when a new connection is accepted.
 * \return  Valid descriptor for the new connection or an existing readable
 *          client; FailedSocketHandle if \a serverSocket is invalid;
 *          InvalidSocketHandle on timeout or other failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE server_accept(SocketMultiplexer& multiplexer, SOCKETHANDLE serverSocket, SocketAddress* socketAddr = nullptr, int32_t timeoutMs = static_cast<int32_t>(areg::WAIT_INFINITE));

/**
 * \brief   Accepts one pending client connection on \a serverSocket.
 *          Legacy stateless overload -- builds a temporary poll list from
 *          \a masterList.  Prefer the SocketMultiplexer overload for
 *          persistent server accept loops.
 *
 * \param   serverSocket    Listening server socket descriptor.
 * \param   masterList      Array of already-accepted socket descriptors, or
 *                          nullptr if none.
 * \param   entriesCount    Number of entries in \a masterList; zero if nullptr.
 * \param   socketAddr      If not nullptr, receives the new client's address.
 * \return  Valid descriptor for the new connection; FailedSocketHandle if
 *          \a serverSocket is invalid; InvalidSocketHandle on other failure.
 **/
[[nodiscard]]
AREG_API SOCKETHANDLE server_accept(SOCKETHANDLE serverSocket, const SOCKETHANDLE* masterList, int32_t entriesCount, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Returns the OS send-buffer size in bytes for \a hSocket.
 **/
[[nodiscard]]
AREG_API uint32_t max_send_size(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Sets the send-buffer size for \a hSocket to \a sendSize bytes.
 *          \a sendSize is clamped to [PACKET_MIN_SIZE, PACKET_MAX_SIZE].
 *
 * \return  The previously configured send size.
 **/
AREG_API uint32_t set_send_size(SOCKETHANDLE hSocket, uint32_t sendSize) noexcept;

/**
 * \brief   Returns the OS receive-buffer size in bytes for \a hSocket.
 **/
[[nodiscard]]
AREG_API uint32_t max_receive_size(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Sets the receive-buffer size for \a hSocket to \a recvSize bytes.
 *          \a recvSize is clamped to [PACKET_MIN_SIZE, PACKET_MAX_SIZE].
 *
 * \return  The previously configured receive size.
 **/
AREG_API uint32_t set_recv_size(SOCKETHANDLE hSocket, uint32_t recvSize) noexcept;

/**
 * \brief   Sends \a dataLength bytes from \a dataBuffer through \a hSocket.
 *          Loops internally on partial sends; returns when all bytes are sent
 *          or a connection error occurs.
 *
 * \param   hSocket         Valid connected socket descriptor.
 * \param   dataBuffer      Buffer holding the bytes to send.
 * \param   dataLength      Number of bytes to send.
 * \return  Total bytes sent (== \a dataLength on success); negative on error;
 *          0 if buffer is empty.
 **/
AREG_API int32_t send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, uint32_t dataLength) noexcept;

/**
 * \brief   Scatter/gather send: sends \a count buffers through \a hSocket
 *          in a single syscall (writev on POSIX, WSASend on Windows).
 *          Reduces per-message syscall overhead when the same socket receives
 *          multiple queued messages in a single drain pass.
 *
 * \param   hSocket     Valid connected socket descriptor.
 * \param   buffers     Array of IoBuffer descriptors; must contain \a count entries.
 * \param   count       Number of buffers in the array.
 * \return  Total bytes sent on success; negative on error; 0 on invalid socket
 *          or empty list.
 **/
AREG_API int32_t send_data_v(SOCKETHANDLE hSocket, const IoBuffer* buffers, uint32_t count, uint32_t totalSize = 0) noexcept;

/**
 * \brief   Receives up to \a dataLength bytes from \a hSocket into \a dataBuffer.
 *          Loops internally on partial receives; returns when the buffer is full,
 *          the peer closes the connection, or an error occurs.
 *
 * \param   hSocket         Valid connected socket descriptor.
 * \param   dataBuffer      Buffer to fill with received bytes.
 * \param   dataLength      Capacity of \a dataBuffer in bytes.
 * \return  Bytes received (> 0); 0 if the peer closed the connection;
 *          negative on error or empty buffer.
 **/
AREG_API int32_t receive_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength) noexcept;

/**
 * \brief   Receives exactly \a dataLength bytes from \a hSocket, reading at most
 *          DEFAULT_THREAD_CACHE_KB * ONE_KILOBYTE bytes per recv() call.
 *          Use for large message bodies to keep per-call latency bounded.
 *          Does not interact with the thread-local receive cache.
 *
 * \param   hSocket         Valid connected socket descriptor.
 * \param   dataBuffer      Buffer to fill with received bytes.
 * \param   dataLength      Exact number of bytes to receive.
 * \return  Bytes received (> 0); negative on error or if the peer closed the connection.
 **/
AREG_API int32_t receive_data_window(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength) noexcept;

/**
 * \brief   Returns the number of bytes of received data that are cached in
 *          the calling thread's read-ahead buffer for \a hSocket.
 *
 *          A non-zero return means additional complete messages are available
 *          in the application cache and will NOT trigger an epoll/select
 *          wakeup (the kernel socket buffer is empty).  Callers that use a
 *          multiplexer-based receive loop (ServerReceiveThread, PoolReceiveThread)
 *          must drain this cache before blocking on the multiplexer again, or
 *          those messages will never be processed.
 *          In Exact receive mode this function always returns zero.
 *
 * \param   hSocket     Socket whose cache is queried.
 * \return  Bytes available in the thread-local cache for this socket (>= 0).
 **/
AREG_API uint32_t recv_data_available(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Sets SO_SNDTIMEO on \a hSocket so that a blocking send() cannot
 *          hang longer than \a timeoutMs milliseconds.  This is a kernel-level
 *          safety net: even if the application-level deadline in _os_send_data
 *          is somehow bypassed, the kernel will return EAGAIN/EWOULDBLOCK after
 *          the configured timeout.
 *
 * \param   hSocket     Valid connected socket descriptor.
 * \param   timeoutMs   Maximum milliseconds a send() may block (0 = infinite).
 * \return  true on success; false otherwise.
 **/
AREG_API bool set_send_timeout(SOCKETHANDLE hSocket, uint32_t timeoutMs) noexcept;

/**
 * \brief   Shuts down the send direction of \a hSocket (half-close).
 *          The socket becomes receive-only.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool disable_send(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Shuts down the receive direction of \a hSocket (half-close).
 *          The socket becomes send-only.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool disable_receive(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Shuts down both directions of hSocket without closing the
 *          file descriptor.  Any thread currently blocked in send() or
 *          recv() on this socket will return immediately with an error.
 *
 * \details Unlike socket_close(), this call is thread-safe by POSIX: it
 *          is defined to interrupt blocked I/O on the same descriptor from
 *          another thread.  Use this to unblock a send/receive thread before
 *          waiting for it to exit, then call socket_close() afterward from
 *          the owning thread.
 **/
AREG_API void socket_interrupt(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Returns true if \a hSocket is still connected (not closed by
 *          the peer or the OS).
 **/
[[nodiscard]]
AREG_API bool is_socket_alive(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Returns the number of bytes queued in the receive buffer of
 *          \a hSocket and ready to read without blocking.
 **/
AREG_API uint32_t pending_read(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Returns the local machine host name, or an empty string on failure.
 **/
[[nodiscard]]
AREG_API const String& hostname() noexcept;

/**
 * \brief   Returns true if \a address is a loopback address
 *          ("localhost" or "127.0.0.1").
 **/
[[nodiscard]]
inline bool is_local_address(const String& address) noexcept;

/**
 * \brief   Returns true if \a address is a well-formed IPv4 dotted-decimal
 *          string (e.g. "192.168.0.1").
 **/
[[nodiscard]]
AREG_API bool is_ip_address(const String& address) noexcept;

/**
 * \brief   Resolves a human-readable host name to an IPv4 address string.
 *          Returns \a hostName unchanged if resolution fails.
 **/
[[nodiscard]]
AREG_API String host_to_ip(const String& hostName);

/**
 * \brief   Resolves an IPv4 address to a host name.
 *          Returns \a ipAddress unchanged if resolution fails.
 **/
[[nodiscard]]
AREG_API String ip_to_host(const String& ipAddress);

/**
 * \brief   Extracts the dotted-decimal IPv4 address from \a sockAddr.
 **/
[[nodiscard]]
AREG_API String extract_ip_address(const struct sockaddr_in& sockAddr);

/**
 * \brief   Extracts the port number from \a sockAddr.
 **/
[[nodiscard]]
AREG_API uint16_t extract_port_number(const struct sockaddr_in& sockAddr) noexcept;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress -- inline implementations
//////////////////////////////////////////////////////////////////////////

inline const areg::String& areg::SocketAddress::host_address() const noexcept
{
    return mIpAddr;
}

inline const areg::String& areg::SocketAddress::host_name() const noexcept
{
    return mHostName;
}

inline uint16_t areg::SocketAddress::host_port() const noexcept
{
    return mPortNr;
}

inline void areg::SocketAddress::reset() noexcept
{
    mIpAddr.clear();
    mHostName.clear();
    mPortNr = areg::InvalidPort;
}

inline bool areg::SocketAddress::is_valid() const noexcept
{
    return (!mIpAddr.is_empty()) && (mPortNr != areg::InvalidPort);
}

//////////////////////////////////////////////////////////////////////////
// areg free function -- inline implementations
//////////////////////////////////////////////////////////////////////////

inline bool areg::is_valid_socket(SOCKETHANDLE hSocket) noexcept
{
    return (hSocket != areg::InvalidSocketHandle) && (hSocket != areg::FailedSocketHandle);
}

inline bool areg::is_local_address(const areg::String& address) noexcept
{
    return (address == areg::LocalHost) || (address == areg::LocalAddress);
}

#endif  // AREG_BASE_SOCKETDEFS_HPP
