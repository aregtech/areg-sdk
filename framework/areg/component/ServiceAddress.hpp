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
namespace areg {

/************************************************************************
 * Child objects
 ************************************************************************/
class StubAddress;
class ProxyAddress;

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
    static String to_path( const ServiceAddress & addService );

    /**
     * \brief   Converts a service address string path to a ServiceAddress object.
     *
     * \param   pathService     The string path representing the service address.
     * \param   out_nextPart    Optional output parameter that points to the next part of the string
     *                          after the service address.
     * \return  Returns the constructed ServiceAddress object.
     **/
    static ServiceAddress from_path( const char * pathService, const char** out_nextPart = nullptr );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty service address.
     **/
    ServiceAddress();

    /**
     * \brief   Creates a service address with the specified name, version, type, and component role
     *          name.
     *
     * \param   serviceName         The service name.
     * \param   serviceVersion      The service version.
     * \param   serviceType         The service type.
     * \param   roleName            The role name of the component owning the service.
     **/
    ServiceAddress( const String & serviceName
                  , const Version & serviceVersion
                  , areg::ServiceType serviceType
                  , const String & roleName );

    /**
     * \brief   Creates a service address from a service item and component role name.
     *
     * \param   serviceItem     The service item containing name, version, and type.
     * \param   roleName        The role name of the component owning the service.
     **/
    ServiceAddress( const ServiceItem  & serviceItem, const String & roleName );

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
     * \brief   Initializes a service address by reading from a stream.
     *
     * \param   stream      The stream containing serialized service address data.
     **/
    ServiceAddress( const InStream & stream );

    /**
     * \brief   Copies a service address from the source.
     *
     * \param   source      The source service address to copy.
     **/
    ServiceAddress( const ServiceAddress & source );

    /**
     * \brief   Moves a service address from the source.
     *
     * \param   source      The source service address to move.
     **/
    ServiceAddress( ServiceAddress && source ) noexcept;

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceAddress() = default;


//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies a service address from the source.
     *
     * \param   source      The source service address.
     **/
    inline ServiceAddress & operator = ( const ServiceAddress & source );

    /**
     * \brief   Moves a service address from the source.
     *
     * \param   source      The source service address.
     **/
    inline ServiceAddress & operator = ( ServiceAddress && source ) noexcept;

    /**
     * \brief   Returns true if two service addresses are equal.
     *
     * \param   other       The service address to compare.
     **/
    inline bool operator == (const ServiceAddress & other ) const;

    /**
     * \brief   Returns true if two service addresses are not equal.
     *
     * \param   other       The service address to compare.
     **/
    inline bool operator != (const ServiceAddress & other ) const;

    /**
     * \brief   Converts the service address to a 32-bit unsigned integer (hash value).
     **/
    inline explicit operator uint32_t () const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads and initializes a service address from a stream.
     *
     * \param   stream      The stream to read data from.
     * \param   input       The service address object to initialize.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ServiceAddress & input );

    /**
     * \brief   Writes a service address into a stream.
     *
     * \param   stream      The stream to write data to.
     * \param   output      The service address to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ServiceAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the role name of the component owning the service.
     **/
    inline const String & role_name() const;

    /**
     * \brief   Sets the role name of the component owning the service.
     *
     * \param   roleName    The role name to set.
     **/
    inline void set_role_name( const String & roleName );

    /**
     * \brief   Returns the service item containing the service name, version, and type.
     **/
    inline const ServiceItem & service() const;

    /**
     * \brief   Converts the service address to its string path representation.
     **/
    String to_string() const;

    /**
     * \brief   Converts a service address string path to initialize this object.
     *
     * \param   pathService     The string path representing the service address.
     * \param   out_nextPart    Optional output parameter that points to the next part of the string
     *                          after the service address.
     **/
    void conv_from_string( const char * pathService, const char** out_nextPart = nullptr );

protected:
    /**
     * \brief   Returns true if the service address contains valid data.
     **/
    inline bool is_validated() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Calculates a hash value for the service address.
     *
     * \param   addrService     The service address to hash.
     **/
    static uint32_t _magic_number( const ServiceAddress addrService );

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The role name of service address.
     **/
    String          mRoleName;

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
    }

    return (*this);
}

inline bool ServiceAddress::operator == ( const ServiceAddress & other ) const
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ServiceAddress::operator != (const ServiceAddress & other) const
{
    return (mMagicNum != other.mMagicNum);
}

inline ServiceAddress::operator uint32_t () const
{
    return mMagicNum;
}

inline const String & ServiceAddress::role_name() const
{
    return mRoleName;
}

inline void ServiceAddress::set_role_name(const String & roleName)
{
    mRoleName = roleName;
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceAddress::_magic_number(*this);
}

inline const ServiceItem & ServiceAddress::service() const
{
    return static_cast<const ServiceItem &>(*this);
}

inline bool ServiceAddress::is_validated() const
{
    return ServiceItem::is_validated() && (mRoleName.is_empty() == false);
}

//////////////////////////////////////////////////////////////////////////
// Global serialization operators
//////////////////////////////////////////////////////////////////////////

inline const InStream & operator >> ( const InStream & stream, ServiceAddress & input )
{
    stream >> static_cast<ServiceItem &>(input); 
    stream >> input.mRoleName; 
    input.mMagicNum = ServiceAddress::_magic_number(input);

    return stream;
}

inline OutStream & operator << ( OutStream & stream, const ServiceAddress & output)
{
    stream << static_cast<const ServiceItem &>(output);
    stream << output.mRoleName;
    return stream;
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
