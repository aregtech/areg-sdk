//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NERemoteRegistry.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEREMOTEREGISTRY_HPP
#define  GENERATED_SRC_NEREMOTEREGISTRY_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.08.2021  13:17:32 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/NERemoteRegistry.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "areg/component/ServiceAddress.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TELinkedList.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NERemoteRegistry namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   RemoteRegistry Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          This interface demonstrates how to use multiple remote services in the system.
 *          Each service is unique in the network.
 *          This service is used by each remote servicing component to register clients.
 *          Before using services, the clients should be registered.
 *
 **/
namespace   NERemoteRegistry
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NERemoteRegistry::ServiceName
     **/
    const char * const    ServiceName         = "RemoteRegistry";
    /**
     * \brief   Implemented Version
     *          NERemoteRegistry::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   DESCRIPTION MISSED
     *          NERemoteRegistry::MaximumOutputs
     **/
    const unsigned int    MaximumOutputs      = 50;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Describes the client registery entry.
     *          Every field of structure should have default constructor.
     *          NERemoteRegistry::sClientRegister
     **/
    struct sClientRegister
    {
    
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////
        
        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sClientRegister( void );

        /**
         * \brief   Initialize sClientRegister data by setting parameters.
         **/
        inline sClientRegister( const unsigned int & _crID, const String & _crName, const ServiceAddress & _crService, const String & _crThread, const String & _crProcess );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NERemoteRegistry::sClientRegister structure to copy data
         **/
        inline sClientRegister( const NERemoteRegistry::sClientRegister & src );

        /**
         * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
         **/
        inline ~sClientRegister( void );
    
    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////
        
        /**
         * \brief   Copies data from given source
         * \param   src     The source of sClientRegister structure to copy data.
         **/
        inline const NERemoteRegistry::sClientRegister & operator = ( const NERemoteRegistry::sClientRegister & src );
        
        /**
         * \brief   Checks equality and returns true if 2 instances of sClientRegister are equal
         * \param   other   The instance of sClientRegister to compare
         * \return  Returns true if 2 instances are equal
         **/ 
        inline bool operator == ( const NERemoteRegistry::sClientRegister & other ) const;
        
        /**
         * \brief   Checks inequality and returns true if 2 instances of sClientRegister are not equal
         * \param   other   The instance of sClientRegister to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
        inline bool operator != ( const NERemoteRegistry::sClientRegister & other ) const;

        /**
         * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
         **/
         inline operator unsigned int ( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // sClientRegister fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The generated ID of the client.
         **/
         unsigned int    crID        /* = 0 */;
        /**
         * \brief   The name of client
         **/
         String          crName      /* = "" */;
        /**
         * \brief   The service address of the client. Required when the client looses connection, so that the system needs to unregister client.
         **/
         ServiceAddress  crService;
        /**
         * \brief   The thread name where the client is running. Required to provide uniqueness.
         **/
         String          crThread    /* = "" */;
        /**
         * \brief   The name of process. Optional parameter, used to make outputs in the log.
         **/
         String          crProcess;
    };

    /**
     * \brief   Describes the list of clients.
     *          NERemoteRegistry::ListRegistry
     **/
     typedef    TELinkedList< sClientRegister >   ListRegistry;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NERemoteRegistry::eMessageIDs
     *          RemoteRegistry Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestRegister
         *          Call to register client. Each client should be registered before starting communication.
         **/
          MSG_ID_requestRegister      = NEService::REQUEST_ID_FIRST     // void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );
        /**
         * \brief   Request ID: MSG_ID_requestUnregister
         *          Sent to unregister connected client.
         **/
        , MSG_ID_requestUnregister                                      // void requestUnregister( const NERemoteRegistry::sClientRegister & client );
        /**
         * \brief   Request ID: MSG_ID_requestHelloWorld
         *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
         **/
        , MSG_ID_requestHelloWorld                                      // void requestHelloWorld( unsigned int clientID, const String & addMessage );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseRegister
         *          Sent when the client is registered. Each registered client has unique ID.
         **/
        , MSG_ID_responseRegister     = NEService::RESPONSE_ID_FIRST    // void responseRegister( const NERemoteRegistry::sClientRegister & client );
        /**
         * \brief   Response ID: MSG_ID_responseHelloWorld
         *          Called as reply to the request to output message.
         **/
        , MSG_ID_responseHelloWorld                                     // void responseHelloWorld( unsigned int clientID );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_RegistryList
         *          List of registered clients.
         **/
        , MSG_ID_RegistryList         = NEService::ATTRIBUTE_ID_FIRST   // NERemoteRegistry::ListRegistry mRegistryList;
        /**
         * \brief   Attribute ID: MSG_ID_RemainOutputs
         *          Remaining outputs before reaching maximum.
         **/
        , MSG_ID_RemainOutputs                                          // unsigned int mRemainOutputs;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED           = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID              = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
    /**
     * \brief   Returns string value of NERemoteRegistry::eMessageIDs
     **/
    inline const char * getString( NERemoteRegistry::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & getInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NERemoteRegistry::eMessageIDs getResponseId( NERemoteRegistry::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NERemoteRegistry::eMessageIDs getRequestId( NERemoteRegistry::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NERemoteRegistry inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NERemoteRegistry methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NERemoteRegistry::eMessageIDs );

/************************************************************************
 * NERemoteRegistry::sClientRegister strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NERemoteRegistry::sClientRegister::sClientRegister( void )
    : crID        ( 0 )
    , crName      ( "" )
    , crService   (  )
    , crThread    ( "" )
    , crProcess   (  )
{   /* do nothing */    }

inline NERemoteRegistry::sClientRegister::sClientRegister( const unsigned int & _crID, const String & _crName, const ServiceAddress & _crService, const String & _crThread, const String & _crProcess )
    : crID        ( _crID )
    , crName      ( _crName )
    , crService   ( _crService )
    , crThread    ( _crThread )
    , crProcess   ( _crProcess )
{   /* do nothing */    }

inline NERemoteRegistry::sClientRegister::sClientRegister( const NERemoteRegistry::sClientRegister & src )
    : crID        ( src.crID )
    , crName      ( src.crName )
    , crService   ( src.crService )
    , crThread    ( src.crThread )
    , crProcess   ( src.crProcess )
{   /* do nothing */    }

inline NERemoteRegistry::sClientRegister::~sClientRegister( void )
{   /* do nothing */    }

inline const NERemoteRegistry::sClientRegister & NERemoteRegistry::sClientRegister::operator = ( const NERemoteRegistry::sClientRegister & src )
{
    if ( static_cast<const NERemoteRegistry::sClientRegister *>(this) != &src )
    {
            this->crID        = src.crID;
            this->crName      = src.crName;
            this->crService   = src.crService;
            this->crThread    = src.crThread;
            this->crProcess   = src.crProcess;
    }
    return (*this);
}

inline bool NERemoteRegistry::sClientRegister::operator == ( const NERemoteRegistry::sClientRegister & other ) const
{
    if ( static_cast<const NERemoteRegistry::sClientRegister *>(this) != &other )
    {
        return (
                   (this->crID        == other.crID)
                && (this->crName      == other.crName)
                && (this->crService   == other.crService)
                && (this->crThread    == other.crThread)
                && (this->crProcess   == other.crProcess)
                );
    }
    else
    {
        return true;
    }
}

inline bool NERemoteRegistry::sClientRegister::operator != ( const NERemoteRegistry::sClientRegister & other ) const
{
    if ( static_cast<const NERemoteRegistry::sClientRegister *>(this) != &other )
    {
        return (
                   (this->crID        != other.crID)
                || (this->crName      != other.crName)
                || (this->crService   != other.crService)
                || (this->crThread    != other.crThread)
                || (this->crProcess   != other.crProcess)
                );
    }
    else
    {
        return false;
    }
}

inline NERemoteRegistry::sClientRegister::operator unsigned int ( void ) const
{
    return (
              static_cast<unsigned int>( crID         )
            + static_cast<unsigned int>( crName       )
            + static_cast<unsigned int>( crService    )
            + static_cast<unsigned int>( crThread     )
            + static_cast<unsigned int>( crProcess    )
            );
}

/**
 * Encode and decode NERemoteRegistry::sClientRegister structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NERemoteRegistry::sClientRegister & input )
{
    stream >> input.crID;
    stream >> input.crName;
    stream >> input.crService;
    stream >> input.crThread;
    stream >> input.crProcess;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NERemoteRegistry::sClientRegister & output )
{
    stream << output.crID;
    stream << output.crName;
    stream << output.crService;
    stream << output.crThread;
    stream << output.crProcess;
    return stream;
}

/**
 * Returns string value of NERemoteRegistry::eMessageIDs
 **/
inline const char * NERemoteRegistry::getString( NERemoteRegistry::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NERemoteRegistry::MSG_ID_requestRegister:
        return "NERemoteRegistry::MSG_ID_requestRegister";
    case    NERemoteRegistry::MSG_ID_requestUnregister:
        return "NERemoteRegistry::MSG_ID_requestUnregister";
    case    NERemoteRegistry::MSG_ID_requestHelloWorld:
        return "NERemoteRegistry::MSG_ID_requestHelloWorld";
    case    NERemoteRegistry::MSG_ID_responseRegister:
        return "NERemoteRegistry::MSG_ID_responseRegister";
    case    NERemoteRegistry::MSG_ID_responseHelloWorld:
        return "NERemoteRegistry::MSG_ID_responseHelloWorld";
    case    NERemoteRegistry::MSG_ID_RegistryList:
        return "NERemoteRegistry::MSG_ID_RegistryList";
    case    NERemoteRegistry::MSG_ID_RemainOutputs:
        return "NERemoteRegistry::MSG_ID_RemainOutputs";

    case NERemoteRegistry::MSG_ID_NO_PROCEED:
        return "NERemoteRegistry::MSG_ID_NO_PROCEED";
        
    case NERemoteRegistry::MSG_ID_INVALID:
        return "NERemoteRegistry::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NERemoteRegistry::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NERemoteRegistry namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_NEREMOTEREGISTRY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NERemoteRegistry.hpp file
//////////////////////////////////////////////////////////////////////////
