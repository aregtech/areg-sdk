#ifndef AREG_IPC_IEREMOTESERVICECONSUMER_HPP
#define AREG_IPC_IEREMOTESERVICECONSUMER_HPP
/************************************************************************
 * \file        areg/src/ipc/IERemoteServiceConsumer.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote service consumer interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#include "areg/src/base/TEArrayList.hpp"
#include "areg/src/component/CEStubAddress.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStreamableEvent;
class CEChannel;

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
    virtual void GetRemoteServiceList( TEArrayList<CEStubAddress, const CEStubAddress &> & out_listStubs) const = 0;

    /**
     * \brief   Call to receive list of registered remote proxy services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_lisProxies
     *          contains list of remote proxy client services in module.
     * \param   out_lisProxies      On output, this contains list of remote proxies, created in current module.
     * \return  Returns size of remote client proxy services in the list.
     **/
    virtual void GetRemoteServiceList( TEArrayList<CEProxyAddress, const CEProxyAddress &> & out_lisProxies) const = 0;

    /**
     * \brief   Call to receive list of registered remote stub service, which connection cookie is equal to 
     *          specified cookie value. In output out_listStubs will contain list of remote stub addresses.
     * \param   cookie          The cookie value to check
     * \param   out_listStubs   On output this will contain list of remote stub addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void GetServiceList( ITEM_ID cookie, TEArrayList<CEStubAddress, const CEStubAddress &> out_listStubs ) const = 0;

    /**
     * \brief   Call to receive list of registered remote proxy service, which connection cookie is equal to 
     *          specified cookie value. In output out_lisProxies will contain list of remote proxy addresses.
     * \param   cookie          The cookie value to check
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void GetServiceList( ITEM_ID cookie, TEArrayList<CEProxyAddress, const CEProxyAddress &> out_lisProxies ) const = 0;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void RegisterRemoteStub( const CEStubAddress & stub ) = 0;

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void RegisterRemoteProxy( const CEProxyAddress & proxy ) = 0;

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     **/
    virtual void UnregisterRemoteStub( const CEStubAddress & stub ) = 0;

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     **/
    virtual void UnregisterRemoteProxy( const CEProxyAddress & proxy ) = 0;

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoteServiceStarted( const CEChannel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoteServiceStopped( const CEChannel & channel ) = 0;

    /**
     * \brief   Triggered when connection with remote service broker is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoveServiceLostConnection( const CEChannel & channel ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteServiceConsumer( const IERemoteServiceConsumer & /*src*/ );
    const IERemoteServiceConsumer & operator = ( const IERemoteServiceConsumer & /*src*/ );
};

#endif // AREG_IPC_IEREMOTESERVICECONSUMER_HPP
