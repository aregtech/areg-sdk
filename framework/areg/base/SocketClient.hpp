#ifndef AREG_BASE_SOCKETCLIENT_HPP
#define AREG_BASE_SOCKETCLIENT_HPP

/************************************************************************
 * \file        areg/base/SocketClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Socket class declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Socket.hpp"

//////////////////////////////////////////////////////////////////////////
// SocketClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Client Socket is used to connect to remote host server,
 *          send and receive data. Before sending or receiving any data,
 *          the socket should be created and as soon as connection is not needed,
 *          it should be closed.
 *          Client socket is using only TCP/IP connection. All other types
 *          and protocols are out of scope of this class and are not considered.
 **/
class AREG_API SocketClient   : public    Socket
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created
     *          and connected to remote host.
     **/
    SocketClient( void );

    /**
     * \brief   Initialization constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created 
     *          and connected to remote host.
     *          When instantiated, it will resolved passed host
     *          name and port number. If succeeded to resolve,
     *          it will set resolved IP-address and port number
     *          as socket address. If passed hostName is NULL,
     *          it resolve connection for local host.
     * \param   hostName    Host name or IP-address of remote server to connect.
     * \param   portNr      Port number of remote server to connect.
     **/
    SocketClient( const char * hostName, unsigned short portNr );

    /**
     * \brief   Initialization constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created 
     *          and connected to remote host.
     *          Specified remoteAddress will be set as remote server address to connect.
     * \param   remoteAddress   Address of remote host to connect.
     **/
    SocketClient( const NESocket::InterlockedValue & remoteAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~SocketClient( void );

public:
/************************************************************************/
// Socket overrides
/************************************************************************/

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connect to specified remote host and port.
     * \param   hostName    The name of host to connect.
     * \param   portNr      The valid port number to connect.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool createSocket( const char * hostName, unsigned short portNr );

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connects to existing socket address. The remote host address
     *          and port number should be already set in socket address.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool createSocket( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SocketClient( const SocketClient & /*src*/ );
    const SocketClient & operator = ( const SocketClient & /*src*/ );
};

#endif  // AREG_BASE_SOCKETCLIENT_HPP
