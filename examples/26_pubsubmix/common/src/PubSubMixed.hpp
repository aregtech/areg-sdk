#ifndef PUBSUBMIX_COMMON_PUBSUBMIXED_HPP
#define PUBSUBMIX_COMMON_PUBSUBMIXED_HPP

/************************************************************************
 * \file        common/src/PubSubMixed.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              Mixed Publisher and Subscriber component.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/Component.hpp"
#include "common/src/Publisher.hpp"
#include "common/src/Subscriber.hpp"

/**
 * \brief   A component that contains a publisher (publishes data) and 2 subscribers, where
 *          one of it is connected to main Publisher to receive service state information.
 *          As soon as main Publisher changes the state to the shutdown, this component triggers
 *          application exit to complete the job and exit the process.
 *          In this example, this component is instantiated twice and run in 2 different threads.
 **/
class PubSubMixed   : public Component
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    PubSubMixed(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up.
     *          Here it starts the timers to change the values.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent(ComponentThread & comThread) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down.
     *          Here it stops the timers.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent(ComponentThread & comThread) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    inline PubSubMixed & self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    Publisher   mPubSubMixedPublisher;  //!< Another publisher to update data
    Subscriber  mControllerSubscriber;  //!< A subscriber of a primary Publisher, which runs in the same process.
    Subscriber  mMixedSubscriber;       //!< A subscriber of the publisher that belong to the same component.
};

#endif // PUBSUBMIX_COMMON_PUBSUBMIXED_HPP
