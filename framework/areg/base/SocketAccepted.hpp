#ifndef AREG_BASE_SOCKETACCEPTED_HPP
#define AREG_BASE_SOCKETACCEPTED_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/SocketAccepted.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Accepted Socket class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Socket.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SocketAccepted class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Accepted socket created by the system when a server socket receives a connection
 *          request. Handle is automatically closed when no other object uses the socket.
 **/
class AREG_API SocketAccepted : public    Socket
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SocketAccepted() = default;

    /**
     * \brief   Initializes socket with a handle and client address.
     *
     * \param   hSocket         Socket descriptor.
     * \param   sockAddress     Address of the accepted client socket.
     **/
    explicit SocketAccepted( const SOCKETHANDLE hSocket, const areg::SocketAddress & sockAddress );

    explicit SocketAccepted(const SOCKETHANDLE hSocket, areg::SocketAddress && sockAddress) noexcept;

    SocketAccepted( const SocketAccepted & source ) = default;

    SocketAccepted( SocketAccepted && source ) noexcept = default;

    virtual ~SocketAccepted() = default;

public:

    SocketAccepted & operator = ( const SocketAccepted & src ) = default;

    SocketAccepted & operator = ( SocketAccepted && src ) noexcept = default;

    [[nodiscard]]
    bool operator == ( const SocketAccepted & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   No-op for accepted sockets. Always returns true.
     **/
    bool create(const String& /*hostName*/, uint16_t /*portNr*/) final;
    bool create(const char * /*hostName*/, uint16_t /*portNr*/ ) final;

    /**
     * \brief   No-op for accepted sockets. Always returns true.
     **/
    bool create() final;

    /**
     * \brief   The address of an accepted socket is the peer's, set when the connection is accepted.
     *          It cannot be set.
     **/
    bool set_address( const char * hostName, uint16_t portNr, bool isServer ) = delete;
    bool set_address( const String & hostName, uint16_t portNr, bool isServer ) = delete;
    void set_address( const areg::SocketAddress & newAddress ) = delete;
};

} // namespace areg
#endif  // AREG_BASE_SOCKETACCEPTED_HPP
