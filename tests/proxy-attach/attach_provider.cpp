/**
 * \file    attach_provider.cpp
 * \brief   Service provider of the proxy attach test, in its own model so that it can be
 *          unloaded and loaded again while the consumer keeps running.
 */

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceProviderBase.hpp"

//!< Service provider, does nothing but exist.
class AttachProvider final : public    areg::Component
                           , protected HelloServiceProviderBase
{
public:
    AttachProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

    void request_hello_service() final
    {
    }
};

BEGIN_MODEL("AttachProviderModel")
    BEGIN_REGISTER_THREAD( "AttachProviderThread" )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", AttachProvider )
            REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceProvider" )
    END_REGISTER_THREAD( "AttachProviderThread" )
END_MODEL("AttachProviderModel")
