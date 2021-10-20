/**
 * \file    common/src/ServiceComponent.hpp
 * \brief   Declaration of the service component.
 **/
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloWorldStub.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceComponent declaration
//////////////////////////////////////////////////////////////////////////
class ServiceComponent  : public    Component
                        , protected HelloWorldStub
{
//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called to instantiate the service component when loading the model.
     * \param   entry   Indicates the component description entry from Registry.
     * \param   owner   The component owning thread.
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called when unloading model to delete service component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    ServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ServiceComponent(void) = default;

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The request to output a greeting.
     * \param   client  The name of client to output the greeting.
     **/
    virtual void requestHelloWorld( const String & client ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< The wrapper of this pointer.
    inline ServiceComponent & self( void )
    {   return (*this); }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceComponent );
};
