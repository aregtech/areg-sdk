#ifndef AREG_IPC_PRIVATE_CESERVERRECEIVETHREAD_HPP
#define AREG_IPC_PRIVATE_CESERVERRECEIVETHREAD_HPP

/************************************************************************
 * \file        areg/src/ipc/private/CEServerReceiveThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Receive Message Thread of Server socket.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEDispatcherThread.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServerConnectionHandler;
class IERemoteServiceHandler;
class CEServerConnection;

//////////////////////////////////////////////////////////////////////////
// CEServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message receiving thread of server socket.
 **/
class AREG_API CEServerReceiveThread    : public    CEDispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection, connection servicing and connection handling objects
     * \param   connectHandler  The instance of server socket connect / disconnect handling interface
     * \param   remoteService   The instance of remote servicing handler
     * \param   connection      The instance of server connection object.
     **/
    CEServerReceiveThread( IEServerConnectionHandler & connectHandler, IERemoteServiceHandler & remoteService, CEServerConnection & connection );
    /**
     * \brief   Destructor
     **/
    virtual ~CEServerReceiveThread( void );

protected:
/************************************************************************/
// CEDispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    IERemoteServiceHandler &   mRemoteService;
    /**
     * \brief   The instance of connection handler interface object
     **/
    IEServerConnectionHandler& mConnectHandler;
    /**
     * \brief   The instance of server connection object
     **/
    CEServerConnection &        mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServerReceiveThread( void );
    CEServerReceiveThread( const CEServerReceiveThread & /*src*/ );
    const CEServerReceiveThread & operator = ( const CEServerReceiveThread & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CESERVERRECEIVETHREAD_HPP
