#ifndef AREG_COMPONENT_CESTUBADDRESS_HPP
#define AREG_COMPONENT_CESTUBADDRESS_HPP
/************************************************************************
 * \file        areg/component/CEStubAddress.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Address class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEServiceAddress.hpp"
#include "areg/component/CEChannel.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class CEProxyAddress;
class CEServiceRequestEvent;

//////////////////////////////////////////////////////////////////////////
// CEStubAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Stub address class.
 *              This class defines address of stub object. All events
 *              sent to Stub object should have as target defined
 *              this Stub Address.
 * 
 * \details     The Stub Address contains Component Address of Stub holder
 *              and name of implemented service interface. The events 
 *              sent to Stub object should contain this address as a target
 *              of event delivery and the address should not be equal to
 *              CEStubAddress::INVALID_STUB_ADDRESS. Check validation
 *              of Stub address before sending event.
 *
 **/
class AREG_API CEStubAddress    : public    CEServiceAddress
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Hidden constants
/************************************************************************/

    /**
     * \brief   CEStubAddress::INVALID_STUB_NAME
     *          The invalid service interface name (invalid stub name)
     *          None of service interfaces should contain this name.
     **/
    static const char * const   INVALID_STUB_NAME   /*= "INVALID_STUB"*/;

    /**
     * \brief   CEStubAddress::EXTENTION_STUB
     *          Extension to add to Stub path.
     **/
    static const char * const   EXTENTION_STUB      /*= "stub"*/;

public:
/************************************************************************/
// Public constants
/************************************************************************/

    /**
     * \brief   CEStubAddress::INVALID_STUB_ADDRESS
     *          Predefined Invalid Stub address.
     **/
    static const CEStubAddress  INVALID_STUB_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts Stub address to string as a Address Path, containing
     *          path separator.
     * \param   addrStub    The Stub address containing information to crate path
     * \return  Returns converted path of Stub as string, containing Stub address information
     **/
    static CEString ConvertAddressToPath( const CEStubAddress & addrStub );

    /**
     * \brief   Instantiate Stub Address from given address path.
     *          If out_nextPart is not NULL, on output this will contain remaining part
     *          from Stub path.
     * \param   pathStub        The path of Stub object, containing information for address.
     * \param   out_nextPart    If not NULL, on output this will contain remaining part of Stub path
     * \return  Returns initialized CEStubAddress object, containing information taken from path
     **/
    static CEStubAddress ConvertPathToAddress(const char* pathStub, const char** out_nextPart = NULL);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initially creates invalid Stub address
     **/
    CEStubAddress( void );

    /**
     * \brief   Initialize Stub address from given service interface name, version, type, role name and holder thread.
     * \param   serviceName     The implemented Service Interface name of Stub
     * \param   serviceVersion  The implemented version of Service Interface
     * \param   serviceType     The type of service
     * \param   roleName        The role name of holder component
     * \param   threadName      Optional thread name of Stub. If NULL, the current thread where Stub instantiated is set.
     **/
    CEStubAddress( const char * serviceName
                 , const CEVersion & serviceVersion
                 , NEService::eServiceType serviceType
                 , const char * roleName
                 , const char * threadName = NULL );

    /**
     * \brief   Initialize Stub address from given service item, role name and holder thread.
     * \param   serviceItem     Service item, which contains basic information of service
     * \param   roleName        The role name of holder component
     * \param   threadName      Optional thread name of Stub. If NULL, the current thread where Stub instantiated is set.
     **/
    CEStubAddress( const CEServiceItem & service, const char * roleName, const char * threadName = NULL );

    /**
     * \brief   Initialize Stub address from given service data, role name and holder thread.
     * \param   siData          Service data, which contains basic information of service
     * \param   roleName        The role name of holder component
     * \param   threadName      Optional thread name of Stub. If NULL, the current thread where Stub instantiated is set.
     **/
    CEStubAddress( const NEService::SInterfaceData & siData, const char * roleName, const char * threadName = NULL );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    CEStubAddress( const CEStubAddress & source );

    /**
     * \brief   Initialize Stub address from stream.
     * \param   stream  The streaming object to read data.
     **/
    CEStubAddress( const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~CEStubAddress( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies Stub Address data from given source.
     * \param   source  The source of stub address to copy
     **/
    const CEStubAddress & operator = ( const CEStubAddress & source );

    /**
     * \brief   Copies Stub Address data from given service address.
     * \param   addrService The service address as a source of basic information.
     **/
    const CEStubAddress & operator = ( const CEServiceAddress & addrService );

    /**
     * \brief   Checks equality of 2 stub address and returns true if objects are equal.
     * \param   other   The stub address to compare.
     **/
    bool operator == ( const CEStubAddress & other ) const;

    /**
     * \brief   Checks compatibility of stub and proxy addresses and returns true if addresses of objects are compatible.
     * \param   addrProxy   The proxy address to check compatibility.
     **/
    bool operator == ( const CEProxyAddress & addrProxy ) const;

    /**
     * \brief   Checks inequality of 2 stub address and returns true if objects are not equal.
     * \param   other   The stub address to compare.
     **/
    bool operator != ( const CEStubAddress & other ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Streaming operator. Reads and initialize proxy address from stream.
     * \param   stream  The streaming object to read data.
     * \param   input   Service address object to initialize data.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEStubAddress & input );

    /**
     * \brief   Streaming operator. Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEStubAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if stub address is local
     **/
    inline bool IsLocalAddress( void ) const;

    /**
     * \brief   Returns true if stub address is remote
     **/
    inline bool IsRemoteAddress( void ) const;

    /**
     * \brief   Returns stub communication channel
     **/
    inline const CEChannel & GetChannel( void ) const;

    /**
     * \brief   Sets stub communication channel.
     **/
    inline void SetChannel( const CEChannel & channel );

    /**
     * \brief   Returns stub cookies value
     **/
    inline ITEM_ID GetCookie( void ) const;

    /**
     * \brief   Sets stub cookie value
     **/
    inline void SetCookie( ITEM_ID cookie );

    /**
     * \brief   Returns the ID of source set in communication channel
     **/
    inline ITEM_ID GetSource( void ) const;

    /**
     * \brief   Sets the ID of source in communication channel.
     **/
    inline void SetSource( ITEM_ID source );

    /**
     * \brief   Returns the service owner thread name.
     **/
    inline const CEString & GetThread( void ) const;

    /**
     * \brief   Returns validity of stub address. Returns true if Stub Address is not a CEStubAddress::INVALID_STUB_ADDRESS
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Invalidates communication channel
     **/
    void InvalidateChannel( void );

    /**
     * \brief   Sets the service owner thread name.
     * \param   threadName  The thread name to set.
     **/
    void SetThread( const char * threadName );

    /**
     * \brief   Checks compatibility of given proxy address.
     * \param   proxyAddress    The address of proxy to check compatibility
     * \return  Returns true if proxy is compatible with proxy. Otherwise it returns false.
     **/
    bool IsProxyCompatible( const CEProxyAddress & proxyAddress ) const;

    /**
     * \brief   Triggered to deliver given service event to target. Returns true if event is delivered.
     * \param   serviceEvent    The servicing event to deliver to target.
     * \return  Returns true if event is delivered to target for further dispatching.
     * \note    For remote events, it returns true if event is queued to be delivered and returned boolean value does not
     *          indicate that the target component received event.
     **/
    bool DeliverServiceEvent( CEServiceRequestEvent & serviceEvent ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of owner thread.
     **/
    CEString    mThreadName;
    /**
     * \brief   The communication channel.
     **/
    CEChannel   mChannel;
};

//////////////////////////////////////////////////////////////////////////
// CEStubAddress class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool CEStubAddress::operator == ( const CEProxyAddress & addrProxy ) const
{   return IsProxyCompatible(addrProxy);                    }

inline bool CEStubAddress::IsLocalAddress(void) const
{   return mChannel.GetCookie() == NEService::COOKIE_LOCAL;    }

inline bool CEStubAddress::IsRemoteAddress(void) const
{   return (mChannel.GetCookie() > NEService::COOKIE_ROUTER );   }

inline const CEString & CEStubAddress::GetThread( void ) const
{   return mThreadName;                                     }

inline const CEChannel & CEStubAddress::GetChannel(void) const
{   return mChannel;                                        }

inline void CEStubAddress::SetChannel(const CEChannel & channel)
{   mChannel = channel;                                     }

inline ITEM_ID CEStubAddress::GetCookie( void ) const
{   return mChannel.GetCookie();                            }

inline void CEStubAddress::SetCookie( ITEM_ID cookie )
{   mChannel.SetCookie(cookie);                             }

inline ITEM_ID CEStubAddress::GetSource( void ) const
{   return mChannel.GetSource();                            }

inline void CEStubAddress::SetSource( ITEM_ID source )
{   return mChannel.SetSource(source);                      }

#endif  // AREG_COMPONENT_CESTUBADDRESS_HPP
