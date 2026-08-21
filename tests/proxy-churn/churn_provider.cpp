/**
 * \file    churn_provider.cpp
 * \brief   Service provider of the proxy churn test, in its own model so that it can be
 *          unloaded while the consumer keeps running.
 */

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceProviderBase.hpp"

//!< Service provider, does nothing but exist.
class ChurnProvider final : public    areg::Component
                          , protected HelloServiceProviderBase
{
public:
    ChurnProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(self()) )
    {
    }

    void request_hello_service() final
    {
    }

private:
    inline ChurnProvider & self()
    {   return (*this); }
};

BEGIN_MODEL("ProviderModel")
    BEGIN_REGISTER_THREAD( "ProviderThread" )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", ChurnProvider )
            REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceProvider" )
    END_REGISTER_THREAD( "ProviderThread" )
END_MODEL("ProviderModel")
