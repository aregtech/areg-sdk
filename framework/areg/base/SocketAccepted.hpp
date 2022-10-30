#ifndef AREG_BASE_SOCKETACPTED_HPP
#define AREG_BASE_SOCKETACPTED_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketAccepted.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Accepted Socket class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Socket.hpp"

//////////////////////////////////////////////////////////////////////////
// SocketAccepted class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Accepted socket, which is used by server when accepting connection.
 *          The socket handle is created and automatically set by system when connection
 *          request is received by server socket and closed when socket is not used
 *          by any other object.
 **/
class AREG_API SocketAccepted : public    Socket
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates instance of accepted socket
     **/
    SocketAccepted( void ) = default;

    /**
     * \brief   Initialization constructor. Initializes socket descriptor and address
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Address of accepted client socket
     **/
    explicit SocketAccepted( const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    SocketAccepted( const SocketAccepted & source ) = default;

    /**
     * \brief	Move constructor
     * \source	The source to move data.
     **/
    SocketAccepted( SocketAccepted && source ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    virtual ~SocketAccepted( void ) = default;

public:

    /**
     * \brief	Assigns socket data from given source.
     * \param	src		The source of socket to assign.
     **/
    SocketAccepted & operator = ( const SocketAccepted & src ) = default;

    /**
     * \brief	Moves socket data from given source.
     * \param	src		The source of socket to move.
     **/
    SocketAccepted & operator = ( SocketAccepted && src ) noexcept = default;

    /**
     * \brief   Compares sockets. 2 sockets are equal if socket handles are equal.
     * \param   other   The accepted socket object to compare
     * \return  Returns true if sockets handles are equal.
     **/
    bool operator == ( const SocketAccepted & other ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Plays no role for accepted sockets. Always returns true;
     * \return  Returns true.
     **/
    virtual bool createSocket( const char * /*hostName*/, unsigned short /*portNr*/ ) override;

    /**
     * \brief   Plays no role for accepted sockets. Always returns true;
     * \return  Returns true.
     **/
    virtual bool createSocket( void ) override;

    /**
     * \brief   Sets Socket Address. If hostName is not IP-address, it will 
     *          try to resolve first then set. The isServer parameter is needed
     *          to resolve address either for server or for client.
     *          For accepted sockets this call plays no role, because the
     *          the address automatically is resolved when accepting connection.
     * \param   hostName    Host name or IP-address to set. If name is specified,
     *                      first it will be resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \param   isServer    Flag, indicating whether name should be resolve for
     *                      server or for client.
     * \return  Returns true if succeeded to resolve and set Socket Address.
     **/
    bool setAddress( const char * hostName, unsigned short portNr, bool isServer );

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    void setAddress( const NESocket::SocketAddress & newAddress );
};

#endif  // AREG_BASE_SOCKETACPTED_HPP
