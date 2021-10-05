//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NERemoteRegistry.hpp file
//////////////////////////////////////////////////////////////////////////
#pragma once

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:13 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/NERemoteRegistry.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface namespace declaration.
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

#include <utility>

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
    constexpr char        ServiceName[]       { "RemoteRegistry" };
    /**
     * \brief   Implemented Version
     *          NERemoteRegistry::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   DESCRIPTION MISSED
     *          NERemoteRegistry::MaximumOutputs
     **/
    constexpr unsigned int    MaximumOutputs      = 50;

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
         * \param   src     The source of NERemoteRegistry::sClientRegister structure to copy data.
         **/
        inline sClientRegister( const NERemoteRegistry::sClientRegister & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NERemoteRegistry::sClientRegister structure to move data.
         **/
        inline sClientRegister( NERemoteRegistry::sClientRegister && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sClientRegister( void ) = default;
    
    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////
        
        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sClientRegister structure to copy data.
         **/
        inline NERemoteRegistry::sClientRegister & operator = ( const NERemoteRegistry::sClientRegister & src );
        
        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sClientRegister structure to move data.
         **/
        inline NERemoteRegistry::sClientRegister & operator = ( NERemoteRegistry::sClientRegister && src ) noexcept;
        
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
         inline explicit operator unsigned int ( void ) const;

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
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestRegister
         *          Call to register client. Each client should be registered before starting communication.
         **/
          MsgId_requestRegister       = NEService::REQUEST_ID_FIRST     // void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );
        /**
         * \brief   Request ID: MsgId_requestUnregister
         *          Sent to unregister connected client.
         **/
        , MsgId_requestUnregister                                       // void requestUnregister( const NERemoteRegistry::sClientRegister & client );
        /**
         * \brief   Request ID: MsgId_requestHelloWorld
         *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
         **/
        , MsgId_requestHelloWorld                                       // void requestHelloWorld( unsigned int clientID, const String & addMessage );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseRegister
         *          Sent when the client is registered. Each registered client has unique ID.
         **/
        , MsgId_responseRegister      = NEService::RESPONSE_ID_FIRST    // void responseRegister( const NERemoteRegistry::sClientRegister & client );
        /**
         * \brief   Response ID: MsgId_responseHelloWorld
         *          Called as reply to the request to output message.
         **/
        , MsgId_responseHelloWorld                                      // void responseHelloWorld( unsigned int clientID );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_RegistryList
         *          List of registered clients.
         **/
        , MsgId_RegistryList          = NEService::ATTRIBUTE_ID_FIRST   // NERemoteRegistry::ListRegistry mRegistryList;
        /**
         * \brief   Attribute ID: MsgId_RemainOutputs
         *          Remaining outputs before reaching maximum.
         **/
        , MsgId_RemainOutputs                                           // unsigned int mRemainOutputs;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed          = NEService::INVALID_MESSAGE_ID  
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid               = NEService::RESPONSE_ID_NONE    
     
    };
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

inline NERemoteRegistry::sClientRegister::sClientRegister( NERemoteRegistry::sClientRegister && src ) noexcept
    : crID        ( std::move(src.crID) )
    , crName      ( std::move(src.crName) )
    , crService   ( std::move(src.crService) )
    , crThread    ( std::move(src.crThread) )
    , crProcess   ( std::move(src.crProcess) )
{   /* do nothing */    }

inline NERemoteRegistry::sClientRegister & NERemoteRegistry::sClientRegister::operator = ( const NERemoteRegistry::sClientRegister & src )
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

inline NERemoteRegistry::sClientRegister & NERemoteRegistry::sClientRegister::operator = ( NERemoteRegistry::sClientRegister && src ) noexcept
{
    if ( static_cast<const NERemoteRegistry::sClientRegister *>(this) != &src )
    {
            this->crID        = std::move(src.crID);
            this->crName      = std::move(src.crName);
            this->crService   = std::move(src.crService);
            this->crThread    = std::move(src.crThread);
            this->crProcess   = std::move(src.crProcess);
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
    case    NERemoteRegistry::eMessageIDs::MsgId_requestRegister:
        return "NERemoteRegistry::eMessageIDs::MsgId_requestRegister";
    case    NERemoteRegistry::eMessageIDs::MsgId_requestUnregister:
        return "NERemoteRegistry::eMessageIDs::MsgId_requestUnregister";
    case    NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld:
        return "NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld";
    case    NERemoteRegistry::eMessageIDs::MsgId_responseRegister:
        return "NERemoteRegistry::eMessageIDs::MsgId_responseRegister";
    case    NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld:
        return "NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld";
    case    NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
        return "NERemoteRegistry::eMessageIDs::MsgId_RegistryList";
    case    NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
        return "NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs";

    case NERemoteRegistry::eMessageIDs::MsgId_NotProcessed:
        return "NERemoteRegistry::eMessageIDs::MsgId_NotProcessed";
        
    case NERemoteRegistry::eMessageIDs::MsgId_Invalid:
        return "NERemoteRegistry::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NERemoteRegistry::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NERemoteRegistry namespace declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NERemoteRegistry.hpp file
//////////////////////////////////////////////////////////////////////////
