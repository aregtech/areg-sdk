#ifndef AREG_EXAMPLES_11_SVCMIX_ClientComponent_HPP
#define AREG_EXAMPLES_11_SVCMIX_ClientComponent_HPP
/************************************************************************
 * \file        ClientComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              Third servicing component.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ServicingComponent.hpp"

#include "ServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main component that controls application shutdown.
 **/
class ClientComponent : public Component
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

protected:
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~ClientComponent(void);

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////

    inline ClientComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    ServiceClient   mClientMain;        //!< Service client of main component
    ServiceClient   mClientSecondary;   //!< Service client of secondary component.
    ServiceClient   mClientThird;       //!< Third service

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientComponent( void );
    ClientComponent( const ClientComponent & /*src*/ );
    const ClientComponent & operator = ( const ClientComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ClientComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ClientComponent & ClientComponent::self( void )
{
    return (*this);
}

#endif  // AREG_EXAMPLES_11_SVCMIX_ClientComponent_HPP
