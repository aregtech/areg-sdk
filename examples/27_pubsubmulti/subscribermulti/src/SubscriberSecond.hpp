#pragma once

/************************************************************************
 * \file        subscribermulti/src/SubscriberSecond.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of PubSub feature service consumer.
 *              It dynamically subscribes and unsubscribes on data update.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "subscribermulti/src/SubscriberBase.hpp"

/**
 * \brief   In contrast with the 'Subscriber' object, this is a dummy
 *          subscriber object, which does not output any message on console.
 *          The purpose of this object is to test and demonstrate the service
 *          consumer behavior when run with other same consumer in the same thread.
 **/
class SubscriberSecond  : public SubscriberBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The dependency entry.
     * \param   owner   The owning component.
     **/
    SubscriberSecond(const areg::DependencyEntry & entry, Component & owner);

    virtual ~SubscriberSecond() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    bool serviceConnected( areg::ServiceConnectionState status, ProxyBase & proxy ) override;

/************************************************************************/
// PubSubClientBase Overrides
/************************************************************************/
    /**
     * \brief   Triggered, when ServiceProviderState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceProviderState description:
     *          The state of service provider. The value is always valid, updated on change.
     * \param   ServiceProviderState    The value of ServiceProviderState attribute.
     * \param   state                   The data validation flag.
     **/
    void onServiceProviderStateUpdate( PubSub::RunState ServiceProviderState, areg::DataState state ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SubscriberSecond();
    AREG_NOCOPY_NOMOVE(SubscriberSecond);
};
