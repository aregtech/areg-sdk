/************************************************************************
 * \file        areg/src/ipc/private/CEListServiceProxies.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "areg/src/ipc/private/CEListServiceProxies.hpp"
#include "areg/src/ipc/private/CEServiceStub.hpp"
#include "areg/src/component/CEStubAddress.hpp"

const CEServiceProxy     CEListServiceProxies::InvalidProxyService;

CEListServiceProxies::CEListServiceProxies( void )
    : TELinkedList<CEServiceProxy, const CEServiceProxy &> ( )
{
    ; // do nothing
}

CEListServiceProxies::CEListServiceProxies( const CEListServiceProxies & source )
    : TELinkedList<CEServiceProxy, const CEServiceProxy &>( static_cast<const TELinkedList<CEServiceProxy, const CEServiceProxy &> &>(source) )
{
    ; // do nothing
}

CEListServiceProxies::~CEListServiceProxies(void)
{
    ; // do nothing
}

const CEListServiceProxies & CEListServiceProxies::operator = ( const CEListServiceProxies & source )
{
    if ( static_cast<const CEListServiceProxies *>(this) != &source )
    {
        RemoveAll();
        static_cast<TELinkedList<CEServiceProxy, const CEServiceProxy &> &>(*this) = static_cast<const TELinkedList<CEServiceProxy, const CEServiceProxy &> &>(source);
    }
    return (*this);
}

const CEServiceProxy & CEListServiceProxies::GetService( const CEProxyAddress & addrProxy ) const
{
    LISTPOS pos = findProxy(addrProxy);
    return ( pos != NULL ? static_cast<const CEServiceProxy &>(GetAt(pos)) : CEListServiceProxies::InvalidProxyService );
}

CEServiceProxy & CEListServiceProxies::RegisterService( const CEProxyAddress & addrProxy )
{
    LISTPOS pos = findProxy(addrProxy);
    if ( pos == NULL )
        pos = AddTail( CEServiceProxy(addrProxy) );
    return static_cast<CEServiceProxy &>(GetAt(pos));
}

CEServiceProxy & CEListServiceProxies::RegisterService(const CEProxyAddress & addrProxy, const CEServiceStub & stubService)
{
    LISTPOS pos = findProxy(addrProxy);
    if ( pos == NULL )
        pos = AddTail( CEServiceProxy(addrProxy) );

    const CEStubAddress & addrStub = stubService.GetServiceAddress();
    CEServiceProxy & proxyService = GetAt(pos);
    if ( addrStub == addrProxy)
    {
        if ( stubService.GetServiceStatus() == NEService::ServiceConnected )
            proxyService.StubAvailable( addrStub );
        else
            proxyService.StubUnavailable();
    }
    return proxyService;
}

CEServiceProxy CEListServiceProxies::UnregisterService( const CEProxyAddress & addrProxy )
{
    CEServiceProxy result;
    for ( LISTPOS pos = GetHeadPosition( ); pos != NULL; pos = GetNextPosition(pos) )
    {
        const CEServiceProxy & proxyService = GetAt(pos);
        if ( proxyService == addrProxy )
        {
            result = proxyService;
            RemoveAt(pos);
            break;
        }
    }
    return result;
}

int CEListServiceProxies::StubServiceAvailable( const CEStubAddress & addrStub )
{
    int result = 0;
    for ( LISTPOS pos = GetHeadPosition(); pos != NULL; pos = GetNextPosition(pos) )
    {
        CEServiceProxy & proxyService = GetAt(pos);
        result += proxyService.StubAvailable(addrStub) ? 1 : 0;
    }
    return result;
}

int CEListServiceProxies::StubServiceUnavailable( void )
{
    int result = 0;
    for ( LISTPOS pos = GetHeadPosition(); pos != NULL; pos = GetNextPosition(pos) )
    {
        CEServiceProxy & proxyService = GetAt(pos);
        result += proxyService.StubUnavailable() ? 1 : 0;
    }
    return result;
}

int CEListServiceProxies::GetSpecificService(CEListServiceProxies & out_listProxies, ITEM_ID cookie)
{
    int result = 0;
    for ( LISTPOS pos = GetHeadPosition( ); pos != NULL; pos = GetNextPosition(pos) )
    {
        CEServiceProxy & proxyService = GetAt(pos);
        if ( proxyService.GetServiceAddress().GetCookie() == cookie )
        {
            result += 1;
            out_listProxies.AddTail(proxyService);
        }
    }
    return result;
}

LISTPOS CEListServiceProxies::findProxy(const CEProxyAddress & addrProxy) const
{
    LISTPOS pos = GetHeadPosition( );
    for ( ; pos != NULL; pos = GetNextPosition(pos) )
    {
        if ( GetAt(pos) == addrProxy )
            break;
    }
    return pos;
}
