#ifndef AREG_COMPONENT_SERVICEITEM_HPP
#define AREG_COMPONENT_SERVICEITEM_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceItem.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Item object
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class InStream; }

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
    static areg::String convAddressToPath( const ServiceItem & service );
   
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
    ServiceItem();

    /**
     * \brief   Creates service item, sets service name.
     * \param   serviceName     The service name to set.
     **/
    explicit ServiceItem( const areg::String & serviceName );

    /**
     * \brief   Creates service item, sets service name, version and type.
     * \param   serviceName     The service name to set.
     * \param   serviceVersion  The service version to set.
     * \param   serviceType     The type of service
     **/
    ServiceItem( const areg::String & serviceName, const areg::Version & serviceVersion, areg::ServiceType serviceType );

    /**
     * \brief   Creates service item and initializes data from given stream.
     * \param   stream      The streaming object, which contains service item information.
     **/
    ServiceItem( const areg::InStream & stream );

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
    virtual ~ServiceItem() = default;

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
    inline explicit operator uint32_t () const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/
    /**
     * \brief   Streaming operator.
     *          De-serialize Service Item object
     * \param   stream  The streaming object to read out data
     * \param   input   The Service Item to initialize data from stream.
     **/
    friend inline const areg::InStream & operator >> ( const areg::InStream & stream, ServiceItem & input);

    /**
     * \brief   Streaming operator.
     *          Serialize Service Item data to streaming object.
     * \param   stream  The streaming object to write data
     * \param   output  The Service Item containing data for streaming
     **/
    friend inline areg::OutStream & operator << ( areg::OutStream & stream, const ServiceItem & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if service item is valid.
     **/
    inline bool isValid() const;

    /**
     * \brief   Returns service name.
     **/
    inline const areg::String & getServiceName() const;

    /**
     * \brief   Sets the service name
     **/
    inline void setServiceName( const areg::String & serviceName );

    /**
     * \brief   Returns service version
     **/
    inline const areg::Version & getServiceVersion() const;

    /**
     * \brief   Sets service version
     **/
    inline void setServiceVersion( const areg::Version & serviceVersion );

    /**
     * \brief   Returns service type
     **/
    inline areg::ServiceType getServiceType() const;

    /**
     * \brief   Sets the service type
     **/
    inline void setServiceType( areg::ServiceType serviceType );

    /**
     * \brief   Returns true if service is remote
     **/
    inline bool isServicePublic() const;

    /**
     * \brief   Checks whether given service item is compatible.
     **/
    inline bool isServiceCompatible( const ServiceItem & other ) const;

    /**
     * \brief   Creates Service Item path as a string.
     * /return  Returns service item as a string.
     **/
    areg::String convToString() const;

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
    inline bool isValidated() const;
   
private:

    /**
     * \brief   Returns the calculated hash-key value of specified service item.
     **/
    static uint32_t _magicNumber( const ServiceItem svcItem );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Service name
     **/
    areg::String                  mServiceName;
    /**
     * \brief   Service Version
     **/
    areg::Version           mServiceVersion;
    /**
     * \brief   Service type
     **/
    areg::ServiceType mServiceType;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The calculated number of service item
     **/
    uint32_t            mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ServiceItem class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServiceItem.
 */
namespace std
{
    template<>
    struct hash<ServiceItem>
    {
        //! A function to convert ServiceItem object to uint32_t.
        inline uint32_t operator()(const ServiceItem& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ServiceItem class inline functions
//////////////////////////////////////////////////////////////////////////

inline const areg::String & ServiceItem::getServiceName() const
{
    return mServiceName;
}

inline void ServiceItem::setServiceName( const areg::String & serviceName )
{
    mServiceName = serviceName;
    mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum    = ServiceItem::_magicNumber(*this);
}

inline const areg::Version & ServiceItem::getServiceVersion() const
{
    return mServiceVersion;
}

inline void ServiceItem::setServiceVersion( const areg::Version & serviceVersion )
{
    mServiceVersion = serviceVersion;
}

inline areg::ServiceType ServiceItem::getServiceType() const
{
    return mServiceType;
}

inline void ServiceItem::setServiceType( areg::ServiceType serviceType )
{
    mServiceType = serviceType;
    mMagicNum    = serviceType != areg::ServiceType::Invalid ? ServiceItem::_magicNumber(*this) : areg::CHECKSUM_IGNORE;
}

inline bool ServiceItem::isServicePublic() const
{
    return (mServiceType == areg::ServiceType::Public);
}

inline bool ServiceItem::isValid() const
{
    return ( mMagicNum != areg::CHECKSUM_IGNORE );
}

inline bool ServiceItem::isValidated() const
{
    return (mServiceName.isEmpty()  == false                                    ) && 
           (mServiceName            != ServiceItem::INVALID_SERVICE.data()      ) && 
           (mServiceVersion         != areg::Version::getInvalidVersion()       ) &&
           (mServiceType            != areg::ServiceType::Invalid  );
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

inline ServiceItem::operator uint32_t () const
{
    return mMagicNum;
}

inline bool ServiceItem::isServiceCompatible( const ServiceItem & other ) const
{
    return ((mMagicNum == other.mMagicNum) && mServiceVersion.isCompatible(other.mServiceVersion));
}

inline const areg::InStream & operator >> ( const areg::InStream & stream, ServiceItem & input )
{
    stream >> input.mServiceName;
    stream >> input.mServiceVersion;
    stream >> input.mServiceType;
    
    input.mMagicNum = ServiceItem::_magicNumber(input);

    return stream;
}

inline areg::OutStream & operator << ( areg::OutStream & stream, const ServiceItem & output )
{
    stream << output.mServiceName;
    stream << output.mServiceVersion;
    stream << output.mServiceType;
    return stream;
}

#endif  // AREG_COMPONENT_SERVICEITEM_HPP
