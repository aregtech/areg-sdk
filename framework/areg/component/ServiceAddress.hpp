#ifndef AREG_COMPONENT_SERVICEADDRESS_HPP
#define AREG_COMPONENT_SERVICEADDRESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Service Address object
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceItem.hpp"

#include <utility>

/************************************************************************
 * Child objects
 ************************************************************************/
namespace areg {
    class StubAddress;
    class ProxyAddress;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceRole class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Contains the role name (service name) and the service unique number.
 **/
class AREG_API ServiceRole
{
public:
    ServiceRole();
    explicit ServiceRole(const String& roleName);
    explicit ServiceRole(const UniqueNumber roleNum);
    explicit ServiceRole(const String& roleName, const UniqueNumber roleNum);
    ServiceRole(const ServiceRole& src);
    ServiceRole(ServiceRole&& src) noexcept;
    ~ServiceRole() = default;

public:
    inline ServiceRole& operator = (const ServiceRole& src);
    inline ServiceRole& operator = (ServiceRole&& src) noexcept;

    [[nodiscard]]
    inline bool operator == (const ServiceRole& other) const noexcept;

    [[nodiscard]]
    inline bool operator != (const ServiceRole& other) const noexcept;

    [[nodiscard]]
    inline explicit operator uint32_t() const noexcept;

    inline explicit operator const areg::String& () const noexcept;

public:

    [[nodiscard]]
    inline const String& role_name() const noexcept;

    inline void set_role_name(const String& roleName);

    inline UniqueNumber number() const noexcept;

    inline void set_number(const UniqueNumber roleNum) noexcept;

    [[nodiscard]]
    inline const char* as_string() const noexcept;

    inline void clear();

    [[nodiscard]]
    inline bool is_valid() const noexcept;

    [[nodiscard]]
    inline bool is_empty() const noexcept;

    [[nodiscard]]
    inline bool is_numeric() const noexcept;

private:

    mutable String  mRoleName;

    UniqueNumber    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// ServiceAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Represents the address of a service (stub or proxy) within a component. Combines service
 *          identification (name, version, type) with the component role name.
 **/
class AREG_API ServiceAddress : public    ServiceItem
{
//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts a service address to its string path representation.
     *
     * \param   addService      The service address to convert.
     * \return  Returns the service address as a string path.
     **/
    [[nodiscard]]
    static String to_path( const ServiceAddress & addService );

    /**
     * \brief   Converts a service address string path to a ServiceAddress object.
     *
     * \param       pathService     The string path representing the service address.
     * \param[out]  nextPart        Optional output parameter that points to the next part of the string
     *                              after the service address.
     * \return  Returns the constructed ServiceAddress object.
     **/
    [[nodiscard]]
    static ServiceAddress from_path( const char * pathService, const char** nextPart = nullptr );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceAddress();

    /**
     * \brief   Creates a service address with the specified name, version, type, and component role name.
     *
     * \param   serviceName         The service name.
     * \param   serviceVersion      The service version.
     * \param   serviceType         The service type.
     * \param   roleName            The role name of the component owning the service.
     **/
    ServiceAddress( const String & serviceName , const Version & serviceVersion , areg::ServiceType serviceType , const String & roleName );

    /**
     * \brief   Creates a service address with the specified service number, version, type, and component role name.
     *
     * \param   serviceNum          The service number.
     * \param   serviceVersion      The service version.
     * \param   serviceType         The service type.
     * \param   roleNum             The role name of the component owning the service.
     **/
    ServiceAddress(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType, const UniqueNumber roleNum);

    /**
     * \brief   Creates a service address from a service item and component role name.
     *
     * \param   serviceItem     The service item containing name, version, and type.
     * \param   roleName        The role name of the component owning the service.
     **/
    ServiceAddress( const ServiceItem  & serviceItem, const String & roleName );

    /**
     * \brief   Creates a service address from a service item and component role name.
     *
     * \param   serviceItem     The service item containing name, version, and type.
     * \param   roleNum         The role name of the component owning the service.
     **/
    ServiceAddress(const ServiceItem& serviceItem, const UniqueNumber roleNum);

    /**
     * \brief   Creates a service address from a stub address.
     *
     * \param   addrStub    The stub address containing the service address information.
     **/
    explicit ServiceAddress( const StubAddress & addrStub );

    /**
     * \brief   Creates a service address from a proxy address.
     *
     * \param   addrProxy       The proxy address containing the service address information.
     **/
    explicit ServiceAddress( const ProxyAddress & addrProxy );

    /**
     * \brief   Deserializes service address from shared service identity and endpoint fields.
     *
     * \param   rawService  Shared service interface identity (role/service hash, type).
     * \param   endPoint    Endpoint carrying version and routing fields.
     **/
    ServiceAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint);

    ServiceAddress( ServiceAddress && source ) noexcept;

    ServiceAddress(const ServiceAddress& source) = default;

    virtual ~ServiceAddress() = default;


//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline ServiceAddress & operator = ( const ServiceAddress & source );

    inline ServiceAddress & operator = ( ServiceAddress && source ) noexcept;

    [[nodiscard]]
    inline bool operator == (const ServiceAddress & other ) const noexcept;

    [[nodiscard]]
    inline bool operator != (const ServiceAddress & other ) const noexcept;

    /**
     * \brief   Converts the service address to a 32-bit unsigned integer (hash value).
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const noexcept;

    /**
     * \brief   Deserializes date and time value from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     * \param[out] input    ServiceAddress object initialized from deserialized stream data.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ServiceAddress & input );

    /**
     * \brief   Serializes date and time value to stream.
     *
     * \param[out] stream   Streaming object where date and time will be serialized.
     * \param   output      ServiceAddress object to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ServiceAddress& output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if service address is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the role name of the component owning the service.
     **/
    [[nodiscard]]
    inline const String & role_name() const noexcept;

    /**
     * \brief   Sets the role name of the component owning the service.
     *
     * \param   roleName    The role name to set.
     **/
    inline void set_role_name( const String & roleName );

    /**
     * \brief   Returns the service item containing the service name, version, and type.
     **/
    [[nodiscard]]
    inline const ServiceItem & service() const noexcept;

    /**
     * \brief   Converts the service address to its string path representation.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Converts a service address string path to initialize this object.
     *
     * \param       pathService The string path representing the service address.
     * \param[out]  nextPart    Optional output parameter that points to the next part of the string
     *                          after the service address, if not nullptr.
     **/
    void from_string( const char * pathService, const char** nextPart = nullptr );

    /**
     * \brief   Initialize service address data from shared service identity and endpoint fields.
     *
     * \param   rawService  Shared service interface identity (role/service hash, type).
     * \param   endPoint    Endpoint carrying version fields.
     **/
    inline void from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept;

    /**
     * \brief   Write service address data into shared service identity and endpoint fields.
     *
     * \param   rawService  Receives role/service hash and type.
     * \param   endPoint    Receives version fields.
     **/
    inline void to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept;

    /**
     * \brief   Reads ServiceAddress data from an input stream.
     *
     * \param   stream      The input stream to read from.
     **/
    inline const InStream& from_stream(const InStream& stream);

    /**
     * \brief   Writes ServiceAddress data to an output stream.
     *
     * \param   stream      The output stream to write to.
     **/
    inline OutStream& to_stream(OutStream& stream) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Calculates a hash value for the service address.
     *
     * \param   addrService     The service address to hash.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( const ServiceAddress addrService ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The role name of service address.
     **/
    String  mRoleName;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The calculated number of service address
     **/
    uint32_t    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// ServiceRole class inline methods
//////////////////////////////////////////////////////////////////////////

inline ServiceRole& ServiceRole::operator = (const ServiceRole& src)
{
    if (this != &src)
    {
        mRoleName = src.mRoleName;
        mMagicNum = src.mMagicNum;
    }

    return (*this);
}

inline ServiceRole& ServiceRole::operator = (ServiceRole&& src) noexcept
{
    if (this != &src)
    {
        mRoleName = std::move(src.mRoleName);
        mMagicNum = src.mMagicNum;
        src.mMagicNum = areg::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline bool ServiceRole::operator == (const ServiceRole& other) const noexcept
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ServiceRole::operator != (const ServiceRole& other) const noexcept
{
    return (mMagicNum != other.mMagicNum);
}

inline ServiceRole::operator uint32_t() const noexcept
{
    return mMagicNum;
}

inline ServiceRole::operator const areg::String& () const noexcept
{
    return mRoleName;
}

inline const String& ServiceRole::role_name() const noexcept
{
    return mRoleName;
}

inline void ServiceRole::set_role_name(const String& roleName)
{
    mRoleName = roleName;
    mMagicNum = roleName.is_numeric() ? roleName.to_uint32() : areg::crc32_calculate(roleName.as_string());
}

inline const char* ServiceRole::as_string() const noexcept
{
    return mRoleName.as_string();
}

inline void ServiceRole::clear()
{
    mRoleName.clear();
    mMagicNum = areg::CHECKSUM_IGNORE;
}

inline bool ServiceRole::is_valid() const noexcept
{
    return (mMagicNum != areg::CHECKSUM_IGNORE);
}

inline bool ServiceRole::is_empty() const noexcept
{
    if (mRoleName.is_empty())
        mRoleName = String::make_string(mMagicNum);
    return mRoleName.is_empty();
}

inline bool ServiceRole::is_numeric() const noexcept
{
    return mRoleName.is_numeric();
}

inline UniqueNumber ServiceRole::number() const noexcept
{
    return mMagicNum;
}

inline void ServiceRole::set_number(const UniqueNumber roleNum) noexcept
{
    mMagicNum = roleNum;
}

//////////////////////////////////////////////////////////////////////////
// ServiceAddress class inline methods
//////////////////////////////////////////////////////////////////////////

inline ServiceAddress & ServiceAddress::operator = ( const ServiceAddress & source )
{
    if ( static_cast<const ServiceAddress *>(this) != &source )
    {
        static_cast<ServiceItem &>(*this) = static_cast<const ServiceItem &>(source);
        mRoleName   = source.mRoleName;
        mMagicNum   = source.mMagicNum;
    }

    return (*this);
}

inline ServiceAddress & ServiceAddress::operator = ( ServiceAddress && source ) noexcept
{
    if ( static_cast<ServiceAddress *>(this) != &source )
    {
        static_cast<ServiceItem &>(*this) = static_cast<ServiceItem &&>(source);
        mRoleName   = std::move(source.mRoleName);
        mMagicNum   = source.mMagicNum;
        source.mMagicNum = areg::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline bool ServiceAddress::operator == ( const ServiceAddress & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ServiceAddress::operator != (const ServiceAddress & other) const noexcept
{
    return (mMagicNum != other.mMagicNum);
}

inline ServiceAddress::operator uint32_t () const noexcept
{
    return mMagicNum;
}

inline bool ServiceAddress::is_valid() const noexcept
{
    return areg::crc32_valid(mMagicNum);
}

inline const String & ServiceAddress::role_name() const noexcept
{
    return mRoleName;
}

inline void ServiceAddress::set_role_name(const String & roleName)
{
    mRoleName = roleName;
    mMagicNum = roleName.is_numeric() ? roleName.to_uint32() : ServiceAddress::_magic_number(*this);
}

inline const ServiceItem & ServiceAddress::service() const noexcept
{
    return static_cast<const ServiceItem &>(*this);
}

inline void ServiceAddress::to_endpoint(areg::RawService& rawService, areg::Endpoint& endPoint) const noexcept
{
    ServiceItem::to_endpoint(rawService, endPoint);
    rawService.role = mMagicNum;
}

inline void ServiceAddress::from_endpoint(const areg::RawService& rawService, const areg::Endpoint& endPoint) noexcept
{
    ServiceItem::from_endpoint(rawService, endPoint);
    mMagicNum = rawService.role;
}

inline const InStream& ServiceAddress::from_stream(const InStream& stream)
{
    ServiceItem::from_stream(stream);
    stream >> mMagicNum;
    return stream;
}

inline OutStream& ServiceAddress::to_stream(OutStream& stream) const
{
    ServiceItem::to_stream(stream);
    stream << mMagicNum;
    return stream;
}

inline const InStream& operator >> (const InStream& stream, ServiceAddress& input)
{
    return input.from_stream(stream);
}

inline OutStream& operator << (OutStream& stream, const ServiceAddress& output)
{
    return output.to_stream(stream);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ServiceAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServiceAddress.
 */
namespace std {
    template<>
    struct hash<areg::ServiceAddress>
    {
        //! A function to convert ServiceAddress object to uint32_t.
        inline uint32_t operator()(const areg::ServiceAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_SERVICEADDRESS_HPP
