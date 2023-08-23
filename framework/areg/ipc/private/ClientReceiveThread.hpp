#ifndef AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
#define AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientReceiveThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Receive Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteMessageHandler;
class ClientConnection;

//////////////////////////////////////////////////////////////////////////
// ClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message receiver thread from remote routing service.
 *          All received messages are passed to receiver thread for further dispatching
 *          and distribution between components and services.
 **/
class ClientReceiveThread    : public    DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Service handler and client connection objects.
     * \param   remoteService   The instance of remote service to process messages.
     * \param   connection      The instance of client connection object to read messages.
     **/
    ClientReceiveThread(IERemoteMessageHandler& remoteService, ClientConnection & connection );

    /**
     * \brief   Destructor.
     **/
    virtual ~ClientReceiveThread( void ) = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of received data size and rests the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    inline uint32_t extractDataReceive( void );

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running.
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    IERemoteMessageHandler& mRemoteService;
    /**
     * \brief   The instance of connection to receive messages from remote routing service.
     **/
    ClientConnection &      mConnection;

    /**
     * \brief   Accumulative value of received data size.
     */
    std::atomic_uint        mBytesReceive;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientReceiveThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ClientReceiveThread );
};

inline uint32_t ClientReceiveThread::extractDataReceive( void )
{
    return static_cast<uint32_t>(mBytesReceive.exchange(0));
}

#endif  // AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
