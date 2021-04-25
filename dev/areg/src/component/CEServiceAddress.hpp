#ifndef AREG_COMPONENT_CESERVICEADDRESS_HPP
#define AREG_COMPONENT_CESERVICEADDRESS_HPP
/************************************************************************
 * \file        areg/src/component/CEServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Service Address object
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEServiceItem.hpp"

/************************************************************************
 * Child objects
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;

//////////////////////////////////////////////////////////////////////////
// CEServiceAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The service address is a base of Proxy and Stub addresses created in the system.
 *          Since Service are not stand-alone module, but part of components, besides of
 *          service information it contains role name, which is given to component.
 **/
class AREG_API CEServiceAddress : public    CEServiceItem
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
    static CEString ConvertAddressToPath( const CEServiceAddress & addService );

    /**
     * \brief   Converts given service address path as a string to service address object.
     * \param   pathService     The path of service address as a string.
     * \param   out_nextPart    If not NULL, on output this parameter points to next part of part after service address.
     * \return  Returns generated service address object.
     **/
    static CEServiceAddress ConvertPathToAddress( const char * pathService, const char** out_nextPart = NULL );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty service address.
     **/
    CEServiceAddress( void );

    /**
     * \brief   Creates service address. Sets given service name, version, service type and role name of owner component.
     * \param   servieName      The name of service.
     * \param   serviceVersion  The version of service.
     * \param   serviceType     The type of service.
     * \param   roleName        The role name of owner component.
     **/
    CEServiceAddress( const char * serviceName
                    , const CEVersion & serviceVersion
                    , NEService::eServiceType serviceType
                    , const char * roleName );

    /**
     * \brief   Creates service address. Sets given service item information and role name of owner component.
     * \param   serviceItem     The service item object, which contains name, version and type information.
     * \param   roleName        The role name of owner component.
     **/
    CEServiceAddress( const CEServiceItem  & serviceItem, const char * roleName );

    /**
     * \brief   Creates service address, which is contained in stub address object.
     * \param   addrStub        The address of stub object with contains information of service address.
     **/
    CEServiceAddress( const CEStubAddress & addrStub );

    /**
     * \brief   Creates service address, which is contained in proxy address object.
     * \param   addrProxy       The address of proxy object with contains information of service address.
     **/
    CEServiceAddress( const CEProxyAddress & addrProxy );

    /**
     * \brief   Initializes service address data from given streaming object.
     * \param   stream      The streaming object, which contain service address information
     **/
    CEServiceAddress( const IEInStream & stream );

    /**
     * \brief   Copies data from given source.
     * \param   source      The source of data to copy
     **/
    CEServiceAddress( const CEServiceAddress & source );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEServiceAddress( void );


//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies service address from given source.
     * \param   source      The source of service address to copy data.
     **/
    const CEServiceAddress & operator = ( const CEServiceAddress & source );

    /**
     * \brief   Checks equality of 2 service addresses and returns true if they are equal.
     * \param   other       The service address to check
     **/
    bool operator == (const CEServiceAddress & other ) const;

    /**
     * \brief   Checks inequality of 2 service addresses and returns true if they are not equal.
     * \param   other       The service address to check
     **/
    bool operator != (const CEServiceAddress & other ) const;

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CEServiceAddress & input );

    /**
     * \brief   Streaming operator. Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CEServiceAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns role name of service address.
     **/
    inline const CEString & GetRoleName( void ) const;

    /**
     * \brief   Sets the role name of service address
     * \param   roleName        New role name to set.
     **/
    inline void SetRoleName( const char * roleName );

    /**
     * \brief   Returns service item information, which contains service name, version and type.
     **/
    inline const CEServiceItem & GetService( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The role name of service address.
     **/
    CEString        mRoleName;
};

//////////////////////////////////////////////////////////////////////////
// CEServiceAddress class inline methods
//////////////////////////////////////////////////////////////////////////

inline const CEString & CEServiceAddress::GetRoleName(void) const
{   return mRoleName;                                                                                   }

inline void CEServiceAddress::SetRoleName(const char * roleName)
{   mRoleName = roleName; mRoleName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);                      }

inline const CEServiceItem & CEServiceAddress::GetService( void ) const
{   return static_cast<const CEServiceItem &>(*this);                                                   }

inline const IEInStream & operator >> ( const IEInStream & stream, CEServiceAddress & input )
{   stream >> static_cast<CEServiceItem &>(input); stream >> input.mRoleName; return stream;            }

inline IEOutStream & operator << ( IEOutStream & stream, const CEServiceAddress & output)
{   stream << static_cast<const CEServiceItem &>(output); stream << output.mRoleName; return stream;    }

#endif  // AREG_COMPONENT_CESERVICEADDRESS_HPP
