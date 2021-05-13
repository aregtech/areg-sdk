#ifndef AREG_COMPONENT_SERVICEITEM_HPP
#define AREG_COMPONENT_SERVICEITEM_HPP
/************************************************************************
 * \file        areg/component/ServiceItem.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Item object
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// ServiceItem class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Item is a base class of all service based address used in the system.
 *          It contains service name, version and the type of service. It is used by Proxy and Stub
 *          objects to identify service target and source.
 **/
class AREG_API ServiceItem
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    static const char * const       INVALID_SERVICE         /*= "INVALID_SERVICE"*/;

//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates Service Item path as a string.
     * \param   service     The service item to convert.
     * /return  Returns service item as a string.
     **/
    static String convAddressToPath( const ServiceItem & service );
   
    /**
     * \brief   Converts given service item path as a string to service item object.
     * \param   pathService     The path of service item as a string.
     * \param   out_nextPart    If not NULL, on output this parameter points to next part of part after service item.
     * \return  Returns generated service address object.
     **/
    static ServiceItem convPathToAddress(  const char* pathService, const char** out_nextPart = NULL );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates empty service item.
     **/
    ServiceItem( void );

    /**
     * \brief   Creates service item, sets service name.
     * \param   serviceName     The service name to set.
     **/
    ServiceItem( const char * serviceName );

    /**
     * \brief   Creates service item, sets service name, version and type.
     * \param   serviceName     The service name to set.
     * \param   serviceVersion  The service version to set.
     * \param   serviceType     The type of service
     **/
    ServiceItem( const char * serviceName, const Version & serviceVersion, NEService::eServiceType serviceType );

    /**
     * \brief   Creates service item and initializes data from given stream.
     * \param   stream      The streaming object, which contains service item information.
     **/
    ServiceItem( const IEInStream & stream );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    ServiceItem( const ServiceItem & source );

    /**
     * \brief   Destructor
     **/
    virtual ~ServiceItem( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   source      The source of data to copy
     **/
    const ServiceItem & operator = ( const ServiceItem & source );

    /**
     * \brief   Checks equality of 2 service item and returns true if they are equal.
     * \param   other       The service item to check
     **/
    bool operator == ( const ServiceItem & other ) const;

    /**
     * \brief   Checks inequality of 2 service item and returns true if they are not equal.
     * \param   other       The service item to check
     **/
    bool operator != (const ServiceItem & other ) const;

    /**
     * \brief   Converts service item to 32-bit unsigned integer value.
     **/
    operator unsigned int( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/
    /**
     * \brief   Streaming operator.
     *          De-serialize Service Item object
     * \param   stream  The streaming object to read out data
     * \param   input   The Service Item to initialize data from stream.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, ServiceItem & input);

    /**
     * \brief   Streaming operator.
     *          Serialize Service Item data to streaming object.
     * \param   stream  The streaming object to write data
     * \param   output  The Service Item containing data for streaming
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const ServiceItem & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if service item is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns service name.
     **/
    inline const String & getServiceName( void ) const;

    /**
     * \brief   Sets the service name
     **/
    inline void setServiceName( const char * serviceName );

    /**
     * \brief   Returns service version
     **/
    inline const Version & getServiceVersion( void ) const;

    /**
     * \brief   Sets service version
     **/
    inline void setServiceVersion( const Version & serviceVersion );

    /**
     * \brief   Returns service type
     **/
    inline NEService::eServiceType getServiceType( void ) const;

    /**
     * \brief   Sets the service type
     **/
    inline void setServiceType( NEService::eServiceType serviceType );

    /**
     * \brief   Returns true if service is remote
     **/
    inline bool isServiceRemote( void ) const;

    /**
     * \brief   Checks whether given service item is compatible.
     **/
    bool isServiceCompatible( const ServiceItem & other ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Service name
     **/
    String                  mServiceName;
    /**
     * \brief   Service Version
     **/
    Version                 mServiceVersion;
    /**
     * \brief   Service type
     **/
    NEService::eServiceType mServiceType;
};

//////////////////////////////////////////////////////////////////////////
// ServiceItem class inline functions
//////////////////////////////////////////////////////////////////////////

inline const String & ServiceItem::getServiceName( void ) const
{
    return mServiceName;
}

inline void ServiceItem::setServiceName( const char * serviceName )
{
    mServiceName = serviceName;
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

inline const Version & ServiceItem::getServiceVersion( void ) const
{
    return mServiceVersion;
}

inline void ServiceItem::setServiceVersion( const Version & serviceVersion )
{
    mServiceVersion = serviceVersion;
}

inline NEService::eServiceType ServiceItem::getServiceType( void ) const
{
    return mServiceType;
}

inline void ServiceItem::setServiceType( NEService::eServiceType serviceType )
{
    mServiceType = serviceType;
}

inline bool ServiceItem::isServiceRemote(void) const
{
    return (mServiceType == NEService::ServiceRemote);
}

inline bool ServiceItem::isValid( void ) const
{
    return ( (mServiceName != ServiceItem::INVALID_SERVICE) && mServiceVersion.isValid() );
}

#endif  // AREG_COMPONENT_SERVICEITEM_HPP
