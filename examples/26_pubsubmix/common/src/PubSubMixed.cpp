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

#include "areg/component/NERegistry.hpp"

PubSubMixed::PubSubMixed(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component (entry, owner)

    , mPubSubMixedPublisher (static_cast<Component &>(self()))
    , mControllerSubscriber (entry.mDependencyServices[0], static_cast<Component &>(self()), std::any_cast<int>(entry.getComponentData()) * 2 + 0)
    , mMixedSubscriber      (entry.mDependencyServices[1], static_cast<Component &>(self()), std::any_cast<int>(entry.getComponentData()) * 2 + 1)
{
}

void PubSubMixed::startupComponent(ComponentThread & comThread)
{
    Component::startupComponent(comThread);
    mPubSubMixedPublisher.start();
}

void PubSubMixed::shutdownComponent(ComponentThread & comThread)
{
    mPubSubMixedPublisher.stop();
    Component::shutdownComponent(comThread);
}

inline PubSubMixed & PubSubMixed::self(void)
{
    return (*this);
}
