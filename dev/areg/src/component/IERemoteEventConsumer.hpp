#ifndef AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
#define AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
/************************************************************************
 * \file        areg/src/component/IERemoteEventConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEStreamableEvent.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class CERemoteRequestEvent;
class CERemoteNotifyRequestEvent;
class CERemoteResponseEvent;

//////////////////////////////////////////////////////////////////////////
// IERemoteEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The interface of remote event consumer. Derived by Stub object
 *          with type of remote to trigger event processing.
 **/
class AREG_API IERemoteEventConsumer   : public    CEThreadEventConsumerBase
{
protected:
    /**
     * \brief   Default constructor. Protected
     **/
    IERemoteEventConsumer( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~IERemoteEventConsumer( void );

public:
/************************************************************************/
// IERemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteRequestEvent & requestEvent ) = 0;

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteNotifyRequestEvent & requestNotifyEvent ) = 0;

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   requestNotifyEvent  The remote response request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteResponseEvent & responseEvent ) = 0;

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
    virtual void StartEventProcessing( CEEvent & eventElem ) ;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteEventConsumer( const IERemoteEventConsumer & /*src*/ );
    const IERemoteEventConsumer & operator = ( const IERemoteEventConsumer & /*src*/ );
};

#endif  // AREG_COMPONENT_IEREMOTEEVENTCONSUMER_HPP
