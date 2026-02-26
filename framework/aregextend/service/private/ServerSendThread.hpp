#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server send message thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/SendMessageEvent.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
class RemoteMessageHandler;
class ServerConnection;

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message sender thread
 **/
class ServerSendThread  : public    areg::DispatcherThread
                        , public    areg::SendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection servicing handler and server connection objects.
     * \param   remoteService   The instance of remote servicing handle to set.
     * \param   connection      The instance of server socket connection object.
     **/
    ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection );

    /**
     * \brief   Destructor
     **/
    virtual ~ServerSendThread() = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of sent data size and rests the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    inline uint32_t extractDataSend() const;

    /**
     * \brief   Call to enable or disable the received data calculation.
     *          It as well resets the existing calculated data.
     * \param   enable  Flag, indicating whether data calculation is enabled or not.
     **/
    inline void setEnableCalculateData(bool enable);

    /**
     * \brief   Returns flag, indicating whether data calculation is enabled or not.
     **/
    inline bool isCalculateDataEnabled() const;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    void readyForEvents( bool isReady ) override;

/************************************************************************/
// EventRouter class overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    bool postEvent( areg::Event & eventElem ) override;

private:
/************************************************************************/
// SendMessageEventConsumer class overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    void processEvent( const areg::SendMessageEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    RemoteMessageHandler&     mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &          mConnection;
    /**
     * \brief   Accumulative value of sent data size.
     **/
    mutable std::atomic_uint    mBytesSend;
    /**
     * \brief   Flag, indicating whether should calculate send data size or not. By default it does not compute.
     **/
    bool                        mSaveDataSend;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread() = delete;
    AREG_NOCOPY_NOMOVE( ServerSendThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class inline methods
//////////////////////////////////////////////////////////////////////////

inline uint32_t ServerSendThread::extractDataSend() const
{
    return static_cast<uint32_t>(mBytesSend.exchange(0));
}

inline void ServerSendThread::setEnableCalculateData(bool enable)
{
    if (mSaveDataSend != enable)
    {
        mBytesSend.store(0u);
        mSaveDataSend = enable;
    }
}

inline bool ServerSendThread::isCalculateDataEnabled() const
{
    return mSaveDataSend;
}

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
