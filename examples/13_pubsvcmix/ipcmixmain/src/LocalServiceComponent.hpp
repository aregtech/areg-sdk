#ifndef AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXMAIN_SRC_LOCALSERVICECOMPONENT_HPP
#define AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXMAIN_SRC_LOCALSERVICECOMPONENT_HPP
/************************************************************************
 * \file        ipcmixmain/src/LocalServiceComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of local servicing interface.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/LocalServicingComponent.hpp"

//////////////////////////////////////////////////////////////////////////
// LocalServiceComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Local servicing interface. It is "invisible" 
 *          (inaccessible) outside of process.
 **/
class LocalServiceComponent  : public LocalServicingComponent
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalServiceComponent(void);

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    LocalServiceComponent( void );
    LocalServiceComponent( const LocalServiceComponent & /*src*/);
    const LocalServiceComponent & operator = ( const LocalServiceComponent & /*src*/ );
};

#endif // AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXMAIN_SRC_LOCALSERVICECOMPONENT_HPP

