#pragma once

/************************************************************************
 * \file        locsvcmesh/src/MainComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              Main servicing component.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locsvcmesh/src/ServicingComponent.hpp"

//////////////////////////////////////////////////////////////////////////
// MainComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main component that controls application shutdown.
 **/
class MainComponent : public ServicingComponent
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

protected:
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    MainComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~MainComponent(void) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MainComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( MainComponent );
};
