#ifndef AREG_BASE_CESOCKETTEMP_HPP
#define AREG_BASE_CESOCKETTEMP_HPP

/************************************************************************
 * \file        areg/src/base/CESocketTemp.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform temporary socket class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CESocket.hpp"

/**
 * \brief   Temporary Socket class. The socket in this class
 *          is passed only via constructor. The socket handle
 *          cannot be neither created, nor closed
 **/

//////////////////////////////////////////////////////////////////////////
// CESocketTemp class declaration
//////////////////////////////////////////////////////////////////////////
class CESocketTemp  : public CESocket
{
public:
    /**
     * \brief   Initializes temporary socket object by passed socket handle.
     * \param   hSocket     The handle of socket. Normally it is valid value.
     **/
    CESocketTemp( SOCKETHANDLE hSocket );

    /**
     * \brief   Initializes temporary socket object by passed socket handle.
     * \param   hSocket     The handle of socket. Normally it is valid value.
     * \param   address     The socket address to set
     **/
    CESocketTemp( SOCKETHANDLE hSocket, const NESocket::CEInterlockedValue & address );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CESocketTemp( const CESocketTemp & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CESocketTemp( void );
//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CESocket overrides
/************************************************************************/

    /**
     * \brief   For client sockets, this method is creating new socket descriptor
     *          and connects to specified remote host and port number.
     *          For server sockets, this method is creating new socket descriptor
     *          and bind socket to specified host name and port number.
     *          The method should not do anything for accepted sockets.
     *          If object had before valid socket descriptor, it will be first closed,
     *          then create new.
     *
     *          The method should be overwritten by child classes.
     *
     * \param   hostName    The name of host to connect or bind.
     * \param   portNr      The valid port number to connect or bind.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool CreateSocket( const char * hostName, unsigned short portNr );

    /**
     * \brief   For client sockets, this method is creating new socket descriptor
     *          and connects to host and port number. Both, remote host address
     *          and port number should be already set.
     *          For server sockets, this method is creating new socket descriptor
     *          and bind socket to specified host name and port number. 
     *          Both, socket IP-address and port number should be already set.
     *          The method should not do anything for accepted sockets.
     *          If object had before valid socket descriptor, it will be first closed,
     *          then create new.
     *
     *          The method should be overwritten by child classes.
     *
     * \return  Returns true if operation succeeded.
     **/
    virtual bool CreateSocket( void );

protected:
/************************************************************************/
// CESocket protected overrides
/************************************************************************/

    /**
     * \brief   Called when the lock counter reaches zero.
     *          By default, when lock counter is zero, the system automatically closes socket.
     *          Overwrite this method if other action should be taken.
     * \param   hSocket     The Socket Handle to take close action.
     *                      In the moment when it is called, the member socket handle is already invalidated.
     **/
    virtual void CloseSocketHandle( SOCKETHANDLE hSocket );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESocketTemp( void );
    const CESocketTemp & operator = ( const CESocketTemp & /*src*/ );
};

#endif // AREG_BASE_CESOCKETTEMP_HPP
