#ifndef AREG_COMPONENT_CEPROXYADDRESS_HPP
#define AREG_COMPONENT_CEPROXYADDRESS_HPP
/************************************************************************
 * \file        areg/src/component/CEProxyAddress.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Address class.
 *              Every Proxy is reached by unique address, which is a 
 *              part of Proxy object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/NEService.hpp"
#include "areg/src/component/CEServiceAddress.hpp"
#include "areg/src/component/CEChannel.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class CEStubAddress;
class CEEvent;
class CEServiceRequestEvent;
class CEServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// CEProxyAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Every Proxy contains Proxy Address. The Proxy Address is
 *              needed to identify Proxy. The proxy address is unique and
 *              contains thread address, connected component role name and
 *              the name of service interface.
 * 
 * \details     Proxies are requiring unique identification mechanism.
 *              The Proxy Address provides this identification.
 *              Since one thread can have only one instance of certain
 *              Proxy object, this unique identification ensures
 *              having several instances of same proxy class in different
 *              threads. The proxy address is needed when Stub / Server is
 *              sending response messages.
 *
 **/
class AREG_API CEProxyAddress   : public    CEServiceAddress
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEProxyAddress::INVALID_PROXY_NAME
     *          The name of invalid proxy.
     *          Do not use as Proxy / Service name.
     **/
    static const char * const   INVALID_PROXY_NAME      /*= "INVALID_PROXY"*/;

    /**
     * \brief   CEProxyAddress::EXTENTION_PROXY
     *          Extension, used in the path to convert Proxy Address to string object.
     **/
    static const char * const   EXTENTION_PROXY         /*= "proxy"*/;

public:
    /**
     * \brief   CEProxyAddress::INVALID_PROXY_ADDRESS
     *          Invalid proxy address.
     *          Need to verify Proxy Address validation.
     **/
    static const CEProxyAddress INVALID_PROXY_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates proxy address path from given proxy address.
     *          Every part of proxy address has a special path separator
     *          and path contains information of process ID, thread name,
     *          connected component role name and supported service name,
     *          and special extension identifying proxy.
     * \param	addrProxy   The proxy address to generate path
     * \return  Returns converted path of Proxy as string, containing Proxy address information
     **/
    static CEString ConvertAddressToPath( const CEProxyAddress & addrProxy );

    /**
     * \brief	Pars proxy path string and retrieves proxy address data from path.
     * \param	pathProxy	    The proxy path as a string.
     * \param	out_nextPart	If not a NULL, on output this will contain remaining
     *                          part after getting proxy path. On output usually
     *                          should be NULL.
     * \return	Proxy address object.
     **/
    static CEProxyAddress ConvertPathToAddress(const char * pathProxy, const char** out_nextPart = NULL);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor. By default created Invalid Proxy Address
     **/
    CEProxyAddress( void );

    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   serviceName     The name of service for Proxy
     * \param   serviceVersion  The version of service
     * \param   serviceType     The type of service (local, remote, etc.)
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    CEProxyAddress( const char * serviceName, const CEVersion & serviceVersion, NEService::eServiceType serviceType, const char * roleName, const char * threadName = NULL );
    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   service         The service item object, which contains service name, version and type
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    CEProxyAddress( const CEServiceItem & service, const char * roleName, const char * threadName = NULL );
    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   siData          The service interface data to assign to Proxy.
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    CEProxyAddress( const NEService::SInterfaceData & siData, const char * roleName, const char * threadName = NULL );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    CEProxyAddress( const CEProxyAddress & source );

    /**
     * \brief   Initialize proxy address from streaming object.
     **/
    CEProxyAddress(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~CEProxyAddress( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies proxy address from given source.
     * \param   source  The source of Proxy address to copy
     **/
    const CEProxyAddress & operator = ( const CEProxyAddress & source );

    /**
     * \brief   Checks equality of 2 proxy address objects. Returns true if 2 proxy addresses are equal.
     * \param   other   The Proxy address object to compare
     **/
    bool operator == ( const CEProxyAddress & other ) const;

    /**
     * \brief   Returns true if passed stub address is compatible with existing proxy address
     * \param   addrStub    Address of stub to compare.
     **/
    inline bool operator == (const CEStubAddress & addrStub ) const;

    /**
     * \brief   Checks inequality of 2 proxy addresses. Returns true if 2 proxy addresses are not equal.
     * \param   other   The Proxy address object to compare
     **/
    bool operator != ( const CEProxyAddress & other ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads and initialize proxy address from stream.
     * \param   stream  The streaming object to read data.
     * \param   input   Service address object to initialize data.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEProxyAddress & input );

    /**
     * \brief   Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEProxyAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if existing Proxy address is local
     **/
    inline bool IsLocalAddress( void ) const;

    /**
     * \brief   Returns true if existing Proxy address is local
     **/
    inline bool IsRemoteAddress( void ) const;

    /**
     * \brief   Returns thread name of processed Proxy
     **/
    inline const CEString & GetThread( void ) const;
    /**
     * \brief   Sets the thread name of processed Proxy
     **/
    void SetThread( const char * threadName );
    /**
     * \brief   Returns Proxy communication channel object
     **/
    inline const CEChannel & GetChannel( void ) const;
    /**
     * \brief   Sets proxy communication channel
     **/
    inline void SetChannel( const CEChannel & channel );
    /**
     * \brief   Returns Proxy cookie value
     **/
    inline ITEM_ID GetCookie( void ) const;
    /**
     * \brief   Sets Proxy cookie value
     **/
    inline void SetCookie( ITEM_ID cookie );
    /**
     * \brief   Returns Proxy source ID
     **/
    inline ITEM_ID GetSource( void ) const;
    /**
     * \brief   Sets Proxy source ID
     **/
    inline void SetSource( ITEM_ID source );
    /**
     * \brief   Returns Proxy target ID
     **/
    inline ITEM_ID GetTarget( void ) const;
    /**
     * \brief   Sets Proxy target ID
     **/
    inline void SetTarget( ITEM_ID target);

    /**
     * \brief   Returns true if proxy address is valid.
     *          The validation verified according of data in Invalid Proxy address.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Invalidates communication channel
     **/
    void InvalidateChannel( void );

    /**
     * \brief   Returns true if specified Stub address is compatible with Proxy
     * \param   addStub     The Stub address to check
     **/
    bool IsStubCompatible( const CEStubAddress & addrStub ) const;

    /**
     * \brief   Delivers specified service request event to target Stub
     * \param   stubEvent   The service request event to deliver
     * \return  Returns true if event was successfully delivered. If the specified Event is
     *          for remote target, the returned boolean value indicates whether event was successfully
     *          queued in the communication channel to deliver.
     **/
    bool DeliverServiceEvent( CEServiceRequestEvent & stubEvent ) const;

    /**
     * \brief   Delivers specified service response event to target Stub
     * \param   proxyEvent   The service response event to deliver
     * \return  Returns true if event was successfully delivered. If the specified Event is
     *          for remote target, the returned boolean value indicates whether event was successfully
     *          queued in the communication channel to deliver.
     **/
    bool DeliverServiceEvent( CEServiceResponseEvent & proxyEvent ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Delivers specified service event to target.
     * \param   serviceEvent    The service event to deliver
     * \param   idTarget        The ID of target service to deliver event
     **/
    static bool _deliverEvent( CEEvent & serviceEvent, ITEM_ID idTarget );


//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Thread name of Proxy
     **/
    CEString    mThreadName;
    /**
     * \brief   Communication channel of Proxy.
     **/
    CEChannel   mChannel;
};

//////////////////////////////////////////////////////////////////////////
// CEProxyAddress class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool CEProxyAddress::operator == ( const CEStubAddress & addrStub ) const
{   return IsStubCompatible(addrStub);                      }

inline bool CEProxyAddress::IsLocalAddress(void) const
{   return mChannel.GetCookie() == NEService::COOKIE_LOCAL;    }

inline bool CEProxyAddress::IsRemoteAddress(void) const
{   return (mChannel.GetCookie() > NEService::COOKIE_ROUTER );   }

inline const CEString & CEProxyAddress::GetThread(void) const
{   return mThreadName;                                     }

inline const CEChannel & CEProxyAddress::GetChannel( void ) const
{   return mChannel;                                        }

inline void CEProxyAddress::SetChannel( const CEChannel & channel )
{   mChannel = channel;                                     }

inline ITEM_ID CEProxyAddress::GetCookie( void ) const
{   return mChannel.GetCookie();                            }

inline void CEProxyAddress::SetCookie( ITEM_ID cookie )
{   mChannel.SetCookie(cookie);                             }

inline ITEM_ID CEProxyAddress::GetSource( void ) const
{   return mChannel.GetSource();                            }

inline void CEProxyAddress::SetSource( ITEM_ID source )
{   return mChannel.SetSource(source);                      }

inline ITEM_ID CEProxyAddress::GetTarget( void ) const
{   return mChannel.GetTarget();                            }

inline void CEProxyAddress::SetTarget( ITEM_ID target )
{   return mChannel.SetTarget(target);                      }

#endif  // AREG_COMPONENT_CEPROXYADDRESS_HPP
