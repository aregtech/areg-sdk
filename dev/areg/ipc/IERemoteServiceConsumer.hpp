#ifndef AREG_IPC_IEREMOTESERVICECONSUMER_HPP
#define AREG_IPC_IEREMOTESERVICECONSUMER_HPP
/************************************************************************
 * \file        areg/ipc/IERemoteServiceConsumer.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote service consumer interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TEArrayList.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class StreamableEvent;
class Channel;

//////////////////////////////////////////////////////////////////////////
// IERemoteServiceConsumer interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing consumer interface with callbacks, which are
 *          triggered when servicing state update is requested.
 **/
class AREG_API IERemoteServiceConsumer
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default destructor
     **/
    IERemoteServiceConsumer( void );
    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteServiceConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteServiceConsumer
/************************************************************************/

    /**
     * \brief   Call to receive list of registered remote stub server services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_listStubs
     *          contains list of remote stub server services in module.
     * \param   out_listStubs       On output, this contains list of remote stubs, created in current module.
     * \return  Returns size of remote stub server services in the list.
     **/
    virtual void getRemoteServiceList( TEArrayList<StubAddress, const StubAddress &> & out_listStubs) const = 0;

    /**
     * \brief   Call to receive list of registered remote proxy services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_lisProxies
     *          contains list of remote proxy client services in module.
     * \param   out_lisProxies      On output, this contains list of remote proxies, created in current module.
     * \return  Returns size of remote client proxy services in the list.
     **/
    virtual void getRemoteServiceList( TEArrayList<ProxyAddress, const ProxyAddress &> & out_lisProxies) const = 0;

    /**
     * \brief   Call to receive list of registered remote stub service, which connection cookie is equal to 
     *          specified cookie value. In output out_listStubs will contain list of remote stub addresses.
     * \param   cookie          The cookie value to check
     * \param   out_listStubs   On output this will contain list of remote stub addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> out_listStubs ) const = 0;

    /**
     * \brief   Call to receive list of registered remote proxy service, which connection cookie is equal to 
     *          specified cookie value. In output out_lisProxies will contain list of remote proxy addresses.
     * \param   cookie          The cookie value to check
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<ProxyAddress, const ProxyAddress &> out_lisProxies ) const = 0;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void registerRemoteStub( const StubAddress & stub ) = 0;

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void registerRemoteProxy( const ProxyAddress & proxy ) = 0;

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     **/
    virtual void unregisterRemoteStub( const StubAddress & stub ) = 0;

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     **/
    virtual void unregisterRemoteProxy( const ProxyAddress & proxy ) = 0;

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void remoteServiceStarted( const Channel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void remoteServiceStopped( const Channel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote service broker is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void removeServiceLostConnection( const Channel & channel ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteServiceConsumer( const IERemoteServiceConsumer & /*src*/ );
    const IERemoteServiceConsumer & operator = ( const IERemoteServiceConsumer & /*src*/ );
};

#endif // AREG_IPC_IEREMOTESERVICECONSUMER_HPP
