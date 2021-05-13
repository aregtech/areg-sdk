#ifndef AREG_COMPONENT_SERVICEADDRESS_HPP
#define AREG_COMPONENT_SERVICEADDRESS_HPP
/************************************************************************
 * \file        areg/component/ServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Service Address object
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceItem.hpp"

/************************************************************************
 * Child objects
 ************************************************************************/
class StubAddress;
class ProxyAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The service address is a base of Proxy and Stub addresses created in the system.
 *          Since Service are not stand-alone module, but part of components, besides of
 *          service information it contains role name, which is given to component.
 **/
class AREG_API ServiceAddress : public    ServiceItem
{
//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Service address path as a string.
     * \param   addService  The service address object to convert.
     * /return  Returns service address as a string.
     **/
    static String convAddressToPath( const ServiceAddress & addService );

    /**
     * \brief   Converts given service address path as a string to service address object.
     * \param   pathService     The path of service address as a string.
     * \param   out_nextPart    If not NULL, on output this parameter points to next part of part after service address.
     * \return  Returns generated service address object.
     **/
    static ServiceAddress convPathToAddress( const char * pathService, const char** out_nextPart = NULL );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty service address.
     **/
    ServiceAddress( void );

    /**
     * \brief   Creates service address. Sets given service name, version, service type and role name of owner component.
     * \param   servieName      The name of service.
     * \param   serviceVersion  The version of service.
     * \param   serviceType     The type of service.
     * \param   roleName        The role name of owner component.
     **/
    ServiceAddress( const char * serviceName
                  , const Version & serviceVersion
                  , NEService::eServiceType serviceType
                  , const char * roleName );

    /**
     * \brief   Creates service address. Sets given service item information and role name of owner component.
     * \param   serviceItem     The service item object, which contains name, version and type information.
     * \param   roleName        The role name of owner component.
     **/
    ServiceAddress( const ServiceItem  & serviceItem, const char * roleName );

    /**
     * \brief   Creates service address, which is contained in stub address object.
     * \param   addrStub        The address of stub object with contains information of service address.
     **/
    ServiceAddress( const StubAddress & addrStub );

    /**
     * \brief   Creates service address, which is contained in proxy address object.
     * \param   addrProxy       The address of proxy object with contains information of service address.
     **/
    ServiceAddress( const ProxyAddress & addrProxy );

    /**
     * \brief   Initializes service address data from given streaming object.
     * \param   stream      The streaming object, which contain service address information
     **/
    ServiceAddress( const IEInStream & stream );

    /**
     * \brief   Copies data from given source.
     * \param   source      The source of data to copy
     **/
    ServiceAddress( const ServiceAddress & source );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceAddress( void );


//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies service address from given source.
     * \param   source      The source of service address to copy data.
     **/
    const ServiceAddress & operator = ( const ServiceAddress & source );

    /**
     * \brief   Checks equality of 2 service addresses and returns true if they are equal.
     * \param   other       The service address to check
     **/
    bool operator == (const ServiceAddress & other ) const;

    /**
     * \brief   Checks inequality of 2 service addresses and returns true if they are not equal.
     * \param   other       The service address to check
     **/
    bool operator != (const ServiceAddress & other ) const;

    /**
     * \brief   Converts service address to 32-bit unsigned integer value.
     **/
    operator unsigned int ( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Streaming operator. Reads and initialize proxy address from stream.
     * \param   stream  The streaming object to read data.
     * \param   input   Service address object to initialize data.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, ServiceAddress & input );

    /**
     * \brief   Streaming operator. Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const ServiceAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns role name of service address.
     **/
    inline const String & getRoleName( void ) const;

    /**
     * \brief   Sets the role name of service address
     * \param   roleName        New role name to set.
     **/
    inline void setRoleName( const char * roleName );

    /**
     * \brief   Returns service item information, which contains service name, version and type.
     **/
    inline const ServiceItem & getService( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The role name of service address.
     **/
    String        mRoleName;
};

//////////////////////////////////////////////////////////////////////////
// ServiceAddress class inline methods
//////////////////////////////////////////////////////////////////////////

inline const String & ServiceAddress::getRoleName(void) const
{
    return mRoleName;
}

inline void ServiceAddress::setRoleName(const char * roleName)
{
    mRoleName = roleName;
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

inline const ServiceItem & ServiceAddress::getService( void ) const
{
    return static_cast<const ServiceItem &>(*this);
}

inline const IEInStream & operator >> ( const IEInStream & stream, ServiceAddress & input )
{
    stream >> static_cast<ServiceItem &>(input); 
    stream >> input.mRoleName; 
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const ServiceAddress & output)
{
    stream << static_cast<const ServiceItem &>(output);
    stream << output.mRoleName;
    return stream;
}

#endif  // AREG_COMPONENT_SERVICEADDRESS_HPP
