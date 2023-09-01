/************************************************************************
/************************************************************************
 * \file        common/src/PubSubMixed.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              Mixed Publisher and Subscriber component.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "common/src/PubSubMixed.hpp"

#include "areg/component/NERegistry.hpp"

Component * PubSubMixed::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW PubSubMixed(entry, owner);
}

void PubSubMixed::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /*entry*/)
{
    delete (&compObject);
}

PubSubMixed::PubSubMixed(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component (entry, owner)

    , mPubSubMixedPublisher (static_cast<Component &>(self()))
    , mControllerSubscriber (entry.mDependencyServices[0], static_cast<Component &>(self()), entry.getComponentData().alignInt.mElement * 2 + 0)
    , mMixedSubscriber      (entry.mDependencyServices[1], static_cast<Component &>(self()), entry.getComponentData().alignInt.mElement * 2 + 1)
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
