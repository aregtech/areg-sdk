#ifndef AREG_EXAMPLES_11_SVCMIX_SECONDARYCOMPONENT_HPP
#define AREG_EXAMPLES_11_SVCMIX_SECONDARYCOMPONENT_HPP
/************************************************************************
 * \file        SecondaryComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              Secondary servicing component.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ServicingComponent.hpp"

#include "ServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main component that controls application shutdown.
 **/
class SecondaryComponent : public ServicingComponent
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
    SecondaryComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~SecondaryComponent(void);

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////

    inline SecondaryComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    ServiceClient   mClientMain;        //!< Service client of main component
    ServiceClient   mClientSecondary;   //!< Service client of secondary component.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SecondaryComponent( void );
    SecondaryComponent( const SecondaryComponent & /*src*/ );
    const SecondaryComponent & operator = ( const SecondaryComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline SecondaryComponent & SecondaryComponent::self( void )
{
    return (*this);
}

#endif  // AREG_EXAMPLES_11_SVCMIX_SECONDARYCOMPONENT_HPP
