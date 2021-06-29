#ifndef AREG_IPC_PRIVATE_IEREMOTESERVICEHANDLER_HPP
#define AREG_IPC_PRIVATE_IEREMOTESERVICEHANDLER_HPP
/************************************************************************
 * \file        areg/ipc/private/IERemoteServiceHandler.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Interface of remote Service Handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace NESocket{
    class InterlockedValue;
}
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// IERemoteServiceHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler remote servicing.
 *          It contains callbacks, which are triggered either then succeeded to send data
 *          or an error occurred when running.
 **/
class AREG_API IERemoteServiceHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IERemoteServiceHandler( void );
    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteServiceHandler( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteServiceHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed ) = 0;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicated the source, which failed.
     **/
    virtual void failedReceiveMessage( SOCKETHANDLE whichSource ) = 0;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) = 0;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   addrHost    The address of remote host, which sent message.
     * \param   whichSource The socket handle, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, const NESocket::InterlockedValue & addrHost, SOCKETHANDLE whichSource ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteServiceHandler( const IERemoteServiceHandler & /*src*/ );
    const IERemoteServiceHandler & operator = ( const IERemoteServiceHandler & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_IEREMOTESERVICEHANDLER_HPP
