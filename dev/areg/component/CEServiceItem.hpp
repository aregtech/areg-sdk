#ifndef AREG_COMPONENT_CESERVICEITEM_HPP
#define AREG_COMPONENT_CESERVICEITEM_HPP
/************************************************************************
 * \file        areg/component/CEServiceItem.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Item object
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/CEString.hpp"
#include "areg/base/CEVersion.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// CEServiceItem class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Item is a base class of all service based address used in the system.
 *          It contains service name, version and the type of service. It is used by Proxy and Stub
 *          objects to identify service target and source.
 **/
class AREG_API CEServiceItem
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
    static CEString ConvertAddressToPath( const CEServiceItem & service );
   
    /**
     * \brief   Converts given service item path as a string to service item object.
     * \param   pathService     The path of service item as a string.
     * \param   out_nextPart    If not NULL, on output this parameter points to next part of part after service item.
     * \return  Returns generated service address object.
     **/
    static CEServiceItem ConvertPathToAddress(  const char* pathService, const char** out_nextPart = NULL );

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates empty service item.
     **/
    CEServiceItem( void );

    /**
     * \brief   Creates service item, sets service name.
     * \param   serviceName     The service name to set.
     **/
    CEServiceItem( const char * serviceName );

    /**
     * \brief   Creates service item, sets service name, version and type.
     * \param   serviceName     The service name to set.
     * \param   serviceVersion  The service version to set.
     * \param   serviceType     The type of service
     **/
    CEServiceItem( const char * serviceName, const CEVersion & serviceVersion, NEService::eServiceType serviceType );

    /**
     * \brief   Creates service item and initializes data from given stream.
     * \param   stream      The streaming object, which contains service item information.
     **/
    CEServiceItem( const IEInStream & stream );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    CEServiceItem( const CEServiceItem & source );

    /**
     * \brief   Destructor
     **/
    virtual ~CEServiceItem( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   source      The source of data to copy
     **/
    const CEServiceItem & operator = ( const CEServiceItem & source );

    /**
     * \brief   Checks equality of 2 service item and returns true if they are equal.
     * \param   other       The service item to check
     **/
    bool operator == ( const CEServiceItem & other ) const;

    /**
     * \brief   Checks inequality of 2 service item and returns true if they are not equal.
     * \param   other       The service item to check
     **/
    bool operator != (const CEServiceItem & other ) const;

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
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEServiceItem & input);

    /**
     * \brief   Streaming operator.
     *          Serialize Service Item data to streaming object.
     * \param   stream  The streaming object to write data
     * \param   output  The Service Item containing data for streaming
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEServiceItem & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if service item is valid.
     **/
    inline bool IsValid( void ) const;

    /**
     * \brief   Returns service name.
     **/
    inline const CEString & GetServiceName( void ) const;

    /**
     * \brief   Sets the service name
     **/
    inline void SetServiceName( const char * serviceName );

    /**
     * \brief   Returns service version
     **/
    inline const CEVersion & GetServiceVersion( void ) const;

    /**
     * \brief   Sets service version
     **/
    inline void SetServiceVersion( const CEVersion & serviceVersion );

    /**
     * \brief   Returns service type
     **/
    inline NEService::eServiceType GetServiceType( void ) const;

    /**
     * \brief   Sets the service type
     **/
    inline void SetServiceType( NEService::eServiceType serviceType );

    /**
     * \brief   Returns true if service is remote
     **/
    inline bool IsServiceRemote( void ) const;

    /**
     * \brief   Checks whether given service item is compatible.
     **/
    bool IsServiceCompatible( const CEServiceItem & other ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Service name
     **/
    CEString                    mServiceName;
    /**
     * \brief   Service Version
     **/
    CEVersion                   mServiceVersion;
    /**
     * \brief   Service type
     **/
    NEService::eServiceType     mServiceType;
};

//////////////////////////////////////////////////////////////////////////
// CEServiceItem class inline functions
//////////////////////////////////////////////////////////////////////////

inline const CEString & CEServiceItem::GetServiceName( void ) const
{   return mServiceName;                                                                        }

inline void CEServiceItem::SetServiceName( const char * serviceName )
{   mServiceName = serviceName; mServiceName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);      }

inline const CEVersion & CEServiceItem::GetServiceVersion( void ) const
{   return mServiceVersion;                                                                     }

inline void CEServiceItem::SetServiceVersion( const CEVersion & serviceVersion )
{   mServiceVersion = serviceVersion;                                                           }

inline NEService::eServiceType CEServiceItem::GetServiceType( void ) const
{   return mServiceType;                                                                        }

inline void CEServiceItem::SetServiceType( NEService::eServiceType serviceType )
{   mServiceType = serviceType;                                                                 }

inline bool CEServiceItem::IsServiceRemote(void) const
{   return (mServiceType == NEService::ServiceRemote);                                          }

inline bool CEServiceItem::IsValid( void ) const
{   return ( mServiceName != CEServiceItem::INVALID_SERVICE && mServiceVersion.IsValid() );     }

#endif  // AREG_COMPONENT_CESERVICEITEM_HPP
