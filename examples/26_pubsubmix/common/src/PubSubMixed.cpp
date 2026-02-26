/************************************************************************
 * \file        common/src/PubSubMixed.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Mixed Publisher and Subscriber component.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "common/src/PubSubMixed.hpp"

#include "areg/component/Model.hpp"

PubSubMixed::PubSubMixed(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component (entry, owner)

    , mPubSubMixedPublisher (static_cast<areg::Component &>(self()))
    , mControllerSubscriber (entry.mDependencyServices[0], static_cast<areg::Component &>(self()), std::any_cast<int32_t>(entry.getData()) * 2 + 0)
    , mMixedSubscriber      (entry.mDependencyServices[1], static_cast<areg::Component &>(self()), std::any_cast<int32_t>(entry.getData()) * 2 + 1)
{
}

void PubSubMixed::startupComponent(areg::ComponentThread & comThread)
{
    areg::Component::startupComponent(comThread);
    mPubSubMixedPublisher.start();
}

void PubSubMixed::shutdownComponent(areg::ComponentThread & comThread)
{
    mPubSubMixedPublisher.stop();
    areg::Component::shutdownComponent(comThread);
}

inline PubSubMixed & PubSubMixed::self()
{
    return (*this);
}
