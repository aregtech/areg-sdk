#ifndef AREG_IPC_PRIVATE_CESOCKETCONNECTIONBASEE_HPP
#define AREG_IPC_PRIVATE_CESOCKETCONNECTIONBASEE_HPP
/************************************************************************
 * \file        areg/ipc/private/CESocketConnectionBase.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Connection Base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NESocket.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CERemoteMessage;
class CESocket;

//////////////////////////////////////////////////////////////////////////
// CESocketConnectionBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The client and server connection base class to extend
 **/
class AREG_API CESocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    CESocketConnectionBase( void );
    /**
     * \brief   Destructor
     **/
    virtual ~CESocketConnectionBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   If socket is valid, sends data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to send data to remote host. No data will be sent, if Remote Buffer is empty.
     *          Before sending data, the method will check and validate existing checksum in buffer
     *          structure. And if checksum is invalid, the data will not be sent to remote target.
     *          If checksum is invalid, the returned value is zero.
     *          Note:   The returned value of sent data (used data length) will be different of total buffer length.
     *          Note:   If Remote Buffer is empty, nothing will be sent.
     *          Note:   The call is blocking and method will not return until all data are not sent
     *                  or if data sending fails.
     *          Note:   Check and set checksum before sending data.
     * \param   in_message      The instance of buffer to send. The checksum number of Remote Buffer object
     *                          will be checked before sending. If checksum is invalid, the data will not be sent.
     * \param   clientSocket    The socket object, which can be either client connection socket or accepted socket on server side
     * \return  Returns length in bytes of data in Remote Buffer sent to remote host. 
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or Remote Buffer object is empty.
     **/
    int SendMessage( const CERemoteMessage & in_message, const CESocket & clientSocket ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to receive data from remote host. If Remote Buffer data is empty or checksum is,
     *          not matching, it will return zero.
     *          Note:   The returned value of received data (used data length) will be different of total buffer length.
     *          Note:   If received Remote Buffer was empty, on output out_message in invalid.
     *          Note:   The call is blocking and method will not return until all data are not received
     *                  or if data receiving fails.
     * \param   out_message     The instance of Remote Buffer to receive data. The checksum number of Remote Buffer object
     *                          will be checked after receiving data. If checksum is invalid, the data will invalidated and dropped.
     * \param   clientSocket    The socket object, which can be either client connection socket or accepted socket on server side
     * \return  Returns length in bytes of data in Remote Buffer received from remote host.
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or data in Remote Buffer object is empty.
     **/
    int ReceiveMessage( CERemoteMessage & out_message, const CESocket & clientSocket ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESocketConnectionBase( const CESocketConnectionBase & /*src*/ );
    const CESocketConnectionBase & operator = ( const CESocketConnectionBase & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CESOCKETCONNECTIONBASEE_HPP
