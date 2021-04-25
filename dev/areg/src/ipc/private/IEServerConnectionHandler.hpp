#ifndef AREG_IPC_PRIVATE_IESERVERCONNECTIONHANDLER_HPP
#define AREG_IPC_PRIVATE_IESERVERCONNECTIONHANDLER_HPP

/************************************************************************
 * \file        areg/src/ipc/private/IEServerConnectionHandler.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Interface of server connection handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CESocketAccepted;

//////////////////////////////////////////////////////////////////////////
// IEServerConnectionHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler server side connection.
 *          It contains callbacks when needs to check client connection
 *          or when connection is lost.
 **/
class AREG_API IEServerConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IEServerConnectionHandler( void );
    /**
     * \brief   Destructor
     **/
    virtual ~IEServerConnectionHandler( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServerConnectionHandler interface overrides
/************************************************************************/

    /**
     * \brief   Call to check whether new client connection should be accepted
     *          or rejected. Once client is accepted, server will start to receive
     *          messages from client. Otherwise, connection with client is immediately
     *          closed and communication is stopped.
     * \param   clientSocket    Accepted client socket object to check.
     * \return  Returns true if client connection can be accepted. To reject and close
     *          connection with client, the method should return false.
     **/
    virtual bool CanAcceptConnection( const CESocketAccepted & clientSocket ) = 0;

    /**
     * \brief   Triggered, when lost connection with client.
     *          Passed clientSocket parameter specifies client socket, which lost connection.
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void ConnectionLost( CESocketAccepted & clientSocket ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IEServerConnectionHandler( const IEServerConnectionHandler & /*src*/ );
    const IEServerConnectionHandler & operator = ( const IEServerConnectionHandler & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_IESERVERCONNECTIONHANDLER_HPP
