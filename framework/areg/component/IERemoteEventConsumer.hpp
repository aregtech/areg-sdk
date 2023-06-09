#ifndef AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
#define AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/IERemoteEventConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StreamableEvent.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class RemoteRequestEvent;
class RemoteNotifyRequestEvent;
class RemoteResponseEvent;

//////////////////////////////////////////////////////////////////////////
// IERemoteEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The interface of remote event consumer. Derived by Stub object
 *          with type of remote to trigger event processing.
 **/
class AREG_API IERemoteEventConsumer   : public    ThreadEventConsumerBase
{
protected:
    /**
     * \brief   Default constructor. Protected
     **/
    IERemoteEventConsumer( void ) = default;
    /**
     * \brief   Destructor.
     **/
    virtual ~IERemoteEventConsumer( void ) = default;

public:
/************************************************************************/
// IERemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    virtual void processRemoteRequestEvent( RemoteRequestEvent & requestEvent ) = 0;

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    virtual void processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent ) = 0;

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   requestNotifyEvent  The remote response request event to be processed.
     **/
    virtual void processRemoteResponseEvent( RemoteResponseEvent & responseEvent ) = 0;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by Dispatcher object when starts dispatch event.
     *          This pure virtual method should be overwritten by child
     *          class to process event.
     * \param   eventElem   Event object, which currently dispatcher is
     *                      processing. As soon as event is finished processing
     *                      it will be destroyed.
     **/
    virtual void startEventProcessing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IERemoteEventConsumer );
};

#endif  // AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
