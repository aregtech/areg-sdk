#ifndef AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
#define AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/SocketConnectionBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
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
class RemoteMessage;
class Socket;

//////////////////////////////////////////////////////////////////////////
// SocketConnectionBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The client and server connection base class to extend
 **/
class AREG_API SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    SocketConnectionBase( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~SocketConnectionBase( void ) = default;

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
    int sendMessage( const RemoteMessage & in_message, const Socket & clientSocket ) const;

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
    int receiveMessage( RemoteMessage & out_message, const Socket & clientSocket ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( SocketConnectionBase );
};

#endif  // AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
