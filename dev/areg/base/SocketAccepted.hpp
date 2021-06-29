#ifndef AREG_BASE_SOCKETACPTED_HPP
#define AREG_BASE_SOCKETACPTED_HPP

/************************************************************************
 * \file        areg/base/SocketAccepted.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    SocketAccepted( void );

    /**
     * \brief   Initialization constructor. Initializes socket descriptor and address
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Address of accepted client socket
     **/
    SocketAccepted( const SOCKETHANDLE hSocket, const NESocket::InterlockedValue & sockAddress );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    SocketAccepted( const SocketAccepted & source );

    /**
     * \brief   Destructor
     **/
    virtual ~SocketAccepted( void );

public:
    /**
     * \brief   Assigning operator. Assigns socket descriptor and address from given source
     **/
    const SocketAccepted & operator = ( const SocketAccepted & source );

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
    virtual bool createSocket( const char * /*hostName*/, unsigned short /*portNr*/ );

    /**
     * \brief   Plays no role for accepted sockets. Always returns true;
     * \return  Returns true.
     **/
    virtual bool createSocket( void );

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
    void setAddress( const NESocket::InterlockedValue & newAddress );
};

#endif  // AREG_BASE_SOCKETACPTED_HPP
