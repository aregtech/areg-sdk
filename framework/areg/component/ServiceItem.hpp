#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceItem.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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

#include <string_view>

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
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //!< The name of invalid service.
    static constexpr std::string_view   INVALID_SERVICE         { "INVALID_SERVICE" };

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

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
     * \param   out_nextPart    If not nullptr, on output this parameter points to next part of part after service item.
     * \return  Returns generated service address object.
     **/
    static ServiceItem convPathToAddress(  const char* pathService, const char** out_nextPart = nullptr );

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
    explicit ServiceItem( const char * serviceName );

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
     * \brief   Move constructor.
     * \param   source  The source of data to move.
     **/
    ServiceItem( ServiceItem && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ServiceItem( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   source      The source of data to copy
     **/
    inline ServiceItem & operator = ( const ServiceItem & source );

    /**
     * \brief   Moves data from given source.
     * \param   source      The source of data to move.
     **/
    inline ServiceItem & operator = ( ServiceItem && source ) noexcept;

    /**
     * \brief   Checks equality of 2 service item and returns true if they are equal.
     * \param   other       The service item to check
     **/
    inline bool operator == ( const ServiceItem & other ) const;

    /**
     * \brief   Checks inequality of 2 service item and returns true if they are not equal.
     * \param   other       The service item to check
     **/
    inline bool operator != (const ServiceItem & other ) const;

    /**
     * \brief   Converts service item to 32-bit unsigned integer value.
     **/
    inline explicit operator unsigned int ( void ) const;

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
    inline bool isServicePublic( void ) const;

    /**
     * \brief   Checks whether given service item is compatible.
     **/
    inline bool isServiceCompatible( const ServiceItem & other ) const;

    /**
     * \brief   Creates Service Item path as a string.
     * /return  Returns service item as a string.
     **/
    String convToString( void ) const;

    /**
     * \brief   Converts given service item path as a string to service item object.
     * \param   pathService     The path of service item as a string.
     * \param   out_nextPart    If not nullptr, on output this parameter points to next part of part after service item.
     **/
    void convFromString(  const char* pathService, const char** out_nextPart = nullptr );

protected:
    /**
     * \brief   Returns true if service item has valid data.
     **/
    inline bool isValidated( void ) const;
   
private:

    /**
     * \brief   Returns the calculated hash-key value of specified service item.
     **/
    static unsigned int _magicNumber( const ServiceItem svcItem );

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

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The calculated number of service item
     **/
    unsigned int            mMagicNum;
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
    mMagicNum    = ServiceItem::_magicNumber(*this);
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
    mMagicNum    = serviceType != NEService::eServiceType::ServiceInvalid ? ServiceItem::_magicNumber(*this) : NEMath::CHECKSUM_IGNORE;
}

inline bool ServiceItem::isServicePublic(void) const
{
    return (mServiceType == NEService::eServiceType::ServicePublic);
}

inline bool ServiceItem::isValid( void ) const
{
    return ( mMagicNum != NEMath::CHECKSUM_IGNORE );
}

inline bool ServiceItem::isValidated(void) const
{
    return (mServiceName.isEmpty()  == false                                    ) && 
           (mServiceName            != ServiceItem::INVALID_SERVICE.data()      ) && 
           (mServiceVersion         != Version::INVALID_VERSION                 ) && 
           (mServiceType            != NEService::eServiceType::ServiceInvalid  );
}

inline ServiceItem & ServiceItem::operator = ( const ServiceItem & source )
{
    if ( this != &source )
    {
        mServiceName    = source.mServiceName;
        mServiceVersion = source.mServiceVersion;
        mServiceType    = source.mServiceType;
        mMagicNum       = source.mMagicNum;
    }

    return (*this);
}

inline ServiceItem & ServiceItem::operator = ( ServiceItem && source ) noexcept
{
    if ( this != &source )
    {
        mServiceName    = std::move(source.mServiceName);
        mServiceVersion = std::move(source.mServiceVersion);
        mServiceType    = source.mServiceType;
        mMagicNum       = source.mMagicNum;
    }

    return (*this);
}

inline bool ServiceItem::operator == ( const ServiceItem & other ) const
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ServiceItem::operator != (const ServiceItem & other ) const
{
    return (mMagicNum != other.mMagicNum);
}

inline ServiceItem::operator unsigned int ( void ) const
{
    return mMagicNum;
}

inline bool ServiceItem::isServiceCompatible( const ServiceItem & other ) const
{
    return ((mMagicNum == other.mMagicNum) && mServiceVersion.isCompatible(other.mServiceVersion));
}
