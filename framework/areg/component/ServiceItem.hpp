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
#include "areg/base/areg_global.h"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <string_view>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceItem class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all service addresses used in the system. Contains service name, version,
 *          and type; used by Proxy and Stub objects to identify service endpoints.
 **/
class AREG_API ServiceItem
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
protected:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //!< The name of invalid service.
    static constexpr std::string_view   INVALID_SERVICE         { "INVALID_SERVICE" };

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts service item to a path string.
     *
     * \param   service     The service item to convert.
     * \return  Returns the service item as a string path.
     **/
    [[nodiscard]]
    static String to_path( const ServiceItem & service );
   
    /**
     * \brief   Converts service item path string to service item object.
     *
     * \param       pathService The path of service item as a string.
     * \param[out]  nextPart    If not nullptr, on output points to the next part after the
     *                          service item in the path.
     * \return  Returns the parsed service address object.
     **/
    [[nodiscard]]
    static ServiceItem from_path(  const char* pathService, const char** nextPart = nullptr );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceItem();

    ServiceItem(const ServiceItem& source) = default;

    ServiceItem(ServiceItem&& source) noexcept;

    virtual ~ServiceItem() = default;

    /**
     * \brief   Initializes with service name.
     **/
    explicit ServiceItem( const String & serviceName );

    /**
     * \brief   Initializes with service number.
     **/
    explicit ServiceItem(const UniqueNumber serviceNum);

    /**
     * \brief   Initializes with service name, version, and type.
     *
     * \param   serviceName         The service name.
     * \param   serviceVersion      The service version.
     * \param   serviceType         The service type.
     **/
    ServiceItem( const String & serviceName, const Version & serviceVersion, areg::ServiceType serviceType );

    /**
     * \brief   Initializes with service unique number, version, and type.
     *
     * \param   serviceNum          The service name.
     * \param   serviceVersion      The service version.
     * \param   serviceType         The service type.
     **/
    ServiceItem(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType);

    /**
     * \brief   Deserializes service item from event envelope element.
     **/
    ServiceItem( const areg::RawService& rawService, const areg::Endpoint & endPoint );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline ServiceItem & operator = ( const ServiceItem & source );

    inline ServiceItem & operator = ( ServiceItem && source ) noexcept;

    [[nodiscard]]
    inline bool operator == ( const ServiceItem & other ) const noexcept;

    [[nodiscard]]
    inline bool operator != (const ServiceItem & other ) const noexcept;

    /**
     * \brief   Converts service item to 32-bit unsigned integer.
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const noexcept;

    /**
     * \brief   Deserializes date and time value from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     * \param[out] input    ServiceItem object initialized from deserialized stream data.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ServiceItem & input );

    /**
     * \brief   Serializes date and time value to stream.
     *
     * \param[out] stream   Streaming object where date and time will be serialized.
     * \param   output      ServiceItem object to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ServiceItem & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if service item is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the service name.
     **/
    [[nodiscard]]
    inline const String & service_name() const noexcept;

    /**
     * \brief   Sets the service name.
     *
     * \param   serviceName     The service name to set.
     **/
    inline void set_service_name( const String & serviceName );

    /**
     * \brief   Returns the service version.
     **/
    [[nodiscard]]
    inline const Version & service_version() const noexcept;

    /**
     * \brief   Sets the service version.
     *
     * \param   serviceVersion      The service version to set.
     **/
    inline void set_service_version( const Version & serviceVersion ) noexcept;

    /**
     * \brief   Returns the service type.
     **/
    [[nodiscard]]
    inline areg::ServiceType service_type() const noexcept;

    /**
     * \brief   Sets the service type.
     *
     * \param   serviceType     The service type to set.
     **/
    inline void set_service_type( areg::ServiceType serviceType ) noexcept;

    /**
     * \brief   Returns true if service is public (remote).
     **/
    [[nodiscard]]
    inline bool is_service_public() const noexcept;

    /**
     * \brief   Returns true if service is compatible with the given service.
     *
     * \param   other       The service to check compatibility against.
     **/
    [[nodiscard]]
    inline bool is_service_compatible( const ServiceItem & other ) const noexcept;

    /**
     * \brief   Converts service item to path string.
     *
     * \return  Returns the service item as a string path.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Parses service item path string and initializes from path.
     *
     * \param       pathService The service path as a string.
     * \param[out]  nextPart    If not nullptr, on output points to the next part after the service item.
     **/
    void from_string(  const char* pathService, const char** nextPart = nullptr );


    /**
     * \brief   Initialize service item data from shared service identity and endpoint fields.
     *
     * \param   rawService  Shared service interface identity (service hash, type).
     * \param   endPoint    Endpoint carrying version fields.
     **/
    inline void from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept;

    /**
     * \brief   Write service item data into shared service identity and endpoint fields.
     *
     * \param   rawService  Receives service hash and type.
     * \param   endPoint    Receives version fields.
     **/
    inline void to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept;

    /**
     * \brief   Reads ServiceItem data from an input stream.
     *
     * \param   stream      The input stream to read from.
     **/
    inline const InStream& from_stream(const InStream& stream);

    /**
     * \brief   Writes ServiceItem data to an output stream.
     *
     * \param   stream      The output stream to write to.
     **/
    inline OutStream& to_stream(OutStream& stream) const;

private:

    /**
     * \brief   Returns the hash value of the given service item.
     *
     * \param   svcItem     The service item to hash.
     * \return  Returns the calculated hash value.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( const ServiceItem svcItem ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Service name
     **/
    String              mServiceName;
    /**
     * \brief   Service Version
     **/
    Version             mServiceVersion;
    /**
     * \brief   Service type
     **/
    areg::ServiceType   mServiceType;

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
// ServiceItem class inline functions
//////////////////////////////////////////////////////////////////////////

inline const String & ServiceItem::service_name() const noexcept
{
    return mServiceName;
}

inline void ServiceItem::set_service_name( const String & serviceName )
{
    mServiceName = serviceName;
    mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum    = ServiceItem::_magic_number(*this);
}

inline const Version & ServiceItem::service_version() const noexcept
{
    return mServiceVersion;
}

inline void ServiceItem::set_service_version( const Version & serviceVersion ) noexcept
{
    mServiceVersion = serviceVersion;
}

inline areg::ServiceType ServiceItem::service_type() const noexcept
{
    return mServiceType;
}

inline void ServiceItem::set_service_type( areg::ServiceType serviceType ) noexcept
{
    mServiceType = serviceType;
    mMagicNum    = serviceType != areg::ServiceType::Invalid ? ServiceItem::_magic_number(*this) : areg::CHECKSUM_IGNORE;
}

inline bool ServiceItem::is_service_public() const noexcept
{
    return (mServiceType == areg::ServiceType::Public);
}

inline bool ServiceItem::is_valid() const noexcept
{
    return areg::crc32_valid(mMagicNum);
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

inline bool ServiceItem::operator == ( const ServiceItem & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ServiceItem::operator != (const ServiceItem & other ) const noexcept
{
    return (mMagicNum != other.mMagicNum);
}

inline ServiceItem::operator uint32_t () const noexcept
{
    return mMagicNum;
}

inline bool ServiceItem::is_service_compatible( const ServiceItem & other ) const noexcept
{
    return ((mMagicNum == other.mMagicNum) && mServiceVersion.is_compatible(other.mServiceVersion));
}

inline void ServiceItem::to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept
{
    endPoint.major      = static_cast<uint16_t>(mServiceVersion.major());
    endPoint.minor      = static_cast<uint16_t>(mServiceVersion.minor());
    endPoint.patch      = static_cast<uint16_t>(mServiceVersion.patch());
    endPoint.type       = static_cast<uint16_t>(mServiceType);
    rawService.service  = mMagicNum;
}

inline void ServiceItem::from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept
{
    mServiceVersion.set_version(endPoint.major, endPoint.minor, endPoint.patch);
    mMagicNum    = rawService.service;
    mServiceType = static_cast<areg::ServiceType>(endPoint.type);
    mServiceName.clear();
}

inline const InStream& ServiceItem::from_stream(const InStream& stream)
{
    stream >> mMagicNum;
    stream >> mServiceVersion;
    stream >> mServiceType;
    return stream;
}

inline OutStream& ServiceItem::to_stream(OutStream& stream) const
{
    stream << mMagicNum;
    stream << mServiceVersion;
    stream << mServiceType;
    return stream;
}

inline const InStream& operator >> (const InStream& stream, ServiceItem& input)
{
    return input.from_stream(stream);
}

inline OutStream& operator << (OutStream& stream, const ServiceItem& output)
{
    return output.to_stream(stream);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ServiceItem class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServiceItem.
 */
namespace std {
    template<>
    struct hash<areg::ServiceItem>
    {
        //! A function to convert ServiceItem object to uint32_t.
        inline uint32_t operator()(const areg::ServiceItem& key) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_SERVICEITEM_HPP
