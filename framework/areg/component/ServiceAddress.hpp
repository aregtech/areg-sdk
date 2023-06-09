#ifndef AREG_COMPONENT_SERVICEADDRESS_HPP
#define AREG_COMPONENT_SERVICEADDRESS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Generic Service Address object
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceItem.hpp"

#include <utility>

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
     * \param   out_nextPart    If not nullptr, on output this parameter points to next part of part after service address.
     * \return  Returns generated service address object.
     **/
    static ServiceAddress convPathToAddress( const char * pathService, const char** out_nextPart = nullptr );

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
    ServiceAddress( const String & serviceName
                  , const Version & serviceVersion
                  , NEService::eServiceType serviceType
                  , const String & roleName );

    /**
     * \brief   Creates service address. Sets given service item information and role name of owner component.
     * \param   serviceItem     The service item object, which contains name, version and type information.
     * \param   roleName        The role name of owner component.
     **/
    ServiceAddress( const ServiceItem  & serviceItem, const String & roleName );

    /**
     * \brief   Creates service address, which is contained in stub address object.
     * \param   addrStub        The address of stub object with contains information of service address.
     **/
    explicit ServiceAddress( const StubAddress & addrStub );

    /**
     * \brief   Creates service address, which is contained in proxy address object.
     * \param   addrProxy       The address of proxy object with contains information of service address.
     **/
    explicit ServiceAddress( const ProxyAddress & addrProxy );

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
     * \brief   Moves data from given source.
     * \param   source      The source of data to move.
     **/
    ServiceAddress( ServiceAddress && source ) noexcept;

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceAddress( void ) = default;


//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies service address from given source.
     * \param   source      The source of service address to copy data.
     **/
    inline ServiceAddress & operator = ( const ServiceAddress & source );

    /**
     * \brief   Copies service address from given source.
     * \param   source      The source of service address to copy data.
     **/
    inline ServiceAddress & operator = ( ServiceAddress && source ) noexcept;

    /**
     * \brief   Checks equality of 2 service addresses and returns true if they are equal.
     * \param   other       The service address to check
     **/
    inline bool operator == (const ServiceAddress & other ) const;

    /**
     * \brief   Checks inequality of 2 service addresses and returns true if they are not equal.
     * \param   other       The service address to check
     **/
    inline bool operator != (const ServiceAddress & other ) const;

    /**
     * \brief   Converts service address to 32-bit unsigned integer value.
     **/
    inline explicit operator unsigned int ( void ) const;

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
    inline void setRoleName( const String & roleName );

    /**
     * \brief   Returns service item information, which contains service name, version and type.
     **/
    inline const ServiceItem & getService( void ) const;

    /**
     * \brief   Creates Service address path as a string.
     * /return  Returns service address as a string.
     **/
    String convToString( void ) const;

    /**
     * \brief   Converts given service address path as a string to service address object.
     * \param   pathService     The path of service address as a string.
     * \param   out_nextPart    If not nullptr, on output this parameter points to next part of part after service address.
     **/
    void convFromString( const char * pathService, const char** out_nextPart = nullptr );

protected:
    /**
     * \brief   Returns true if service address has validated data.
     **/
    inline bool isValidated( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Calculates the number of specified service address object.
     **/
    static unsigned int _magicNumber( const ServiceAddress addrService );

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
    unsigned int    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ServiceAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServiceAddress.
 */
namespace std
{
    template<>
    struct hash<ServiceAddress>
    {
        //! A function to convert ServiceAddress object to unsigned int.
        inline unsigned int operator()(const ServiceAddress& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
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

inline ServiceAddress::operator unsigned int ( void ) const
{
    return mMagicNum;
}

inline const String & ServiceAddress::getRoleName(void) const
{
    return mRoleName;
}

inline void ServiceAddress::setRoleName(const String & roleName)
{
    mRoleName = roleName;
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

inline const ServiceItem & ServiceAddress::getService( void ) const
{
    return static_cast<const ServiceItem &>(*this);
}

inline bool ServiceAddress::isValidated(void) const
{
    return ServiceItem::isValidated() && (mRoleName.isEmpty() == false);
}

//////////////////////////////////////////////////////////////////////////
// Global serialization operators
//////////////////////////////////////////////////////////////////////////

inline const IEInStream & operator >> ( const IEInStream & stream, ServiceAddress & input )
{
    stream >> static_cast<ServiceItem &>(input); 
    stream >> input.mRoleName; 
    input.mMagicNum = ServiceAddress::_magicNumber(input);

    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const ServiceAddress & output)
{
    stream << static_cast<const ServiceItem &>(output);
    stream << output.mRoleName;
    return stream;
}

#endif  // AREG_COMPONENT_SERVICEADDRESS_HPP
