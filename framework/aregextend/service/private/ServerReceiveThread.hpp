#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerReceiveThread.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service connectivity server receive message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServiceConnectionHandler;
class IERemoteMessageHandler;
class ServerConnection;

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message receiving thread of server socket.
 **/
class ServerReceiveThread    : public    DispatcherThread
{
    //!< Number of retries to accept socket connection
    static constexpr uint32_t RETRY_COUNT   { 5 };

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
    ServerReceiveThread( IEServiceConnectionHandler & connectHandler, IERemoteMessageHandler& remoteService, ServerConnection & connection );
    /**
     * \brief   Destructor
     **/
    virtual ~ServerReceiveThread( void ) = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of received data size and rests the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    inline uint32_t extractDataReceive( void ) const;

    /**
     * \brief   Call to enable or disable the received data calculation.
     *          It as well resets the existing calculated data.
     * \param   enable  Flag, indicating whether data calculation is enabled or not.
     **/
    inline void setEnableCalculateData(bool enable);

    /**
     * \brief   Returns flag, indicating whether data calculation is enabled or not.
     **/
    inline bool isCalculateDataEnabled(void) const;

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
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Service connection handler.
     **/
    IEServiceConnectionHandler& mConnectHandler;
    /**
     * \brief   The instance of remote service message handler.
     **/
    IERemoteMessageHandler &    mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &          mConnection;
    /**
     * \brief   Accumulative value of received data size.
     */
    mutable std::atomic_uint    mBytesReceive;
    /**
     * \brief   Flag, indicating whether data calculation is enabled or disabled. By default, it is disabled.
     **/
    bool                        mSaveDataReceive;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerReceiveThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServerReceiveThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnection inline methods.
//////////////////////////////////////////////////////////////////////////

inline uint32_t ServerReceiveThread::extractDataReceive(void) const
{
    return mBytesReceive.exchange(0);
}

inline void ServerReceiveThread::setEnableCalculateData(bool enable)
{
    if (mSaveDataReceive != enable)
    {
        mBytesReceive.store(0u);
        mSaveDataReceive = enable;
    }
}

inline bool ServerReceiveThread::isCalculateDataEnabled(void) const
{
    return mSaveDataReceive;
}

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
