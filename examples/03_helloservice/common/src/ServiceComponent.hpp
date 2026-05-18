/**
 * \file    common/src/ServiceComponent.hpp
 * \brief   Declaration of the service component.
 **/
#pragma once

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/03_helloservice/services/HelloServiceProviderBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceComponent declaration
//////////////////////////////////////////////////////////////////////////
class ServiceComponent final : public    areg::Component
                             , protected HelloServiceProviderBase
{
public:
    ServiceComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

//////////////////////////////////////////////////////////////////////////
// HelloService Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The request to output a greeting.
     * \param   client  The name of the client to output the greeting.
     **/
    void request_hello_service( const areg::String & client ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< The wrapper of this pointer.
    inline ServiceComponent & self()
    {   return (*this); }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceComponent() = delete;
    AREG_NOCOPY_NOMOVE( ServiceComponent );
};
