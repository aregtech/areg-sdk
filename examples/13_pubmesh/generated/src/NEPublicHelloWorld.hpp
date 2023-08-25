//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPublicHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPUBLICHELLOWORLD_HPP
#define  GENERATED_SRC_NEPUBLICHELLOWORLD_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:27 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/NEPublicHelloWorld.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
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

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg//component/NERegistry.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NEPublicHelloWorld namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   PublicHelloWorld Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          This interface demonstrates how to use multiple remote services in the system.
 *          Each service is unique in the network.
 *          This service is used by each remote servicing component to register clients.
 *          Before using services, the clients should be registered.
 *
 **/
namespace   NEPublicHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEPublicHelloWorld::ServiceName
     **/
    constexpr   char    ServiceName[]   { "PublicHelloWorld" };
    /**
     * \brief   Implemented Version.
     *          NEPublicHelloWorld::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   DESCRIPTION MISSED
     *          NEPublicHelloWorld::MaximumOutputs
     **/
    constexpr unsigned int    MaximumOutputs      = 50;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Describes the client registry entry.
     *          Every field of structure should have default constructor.
     *          NEPublicHelloWorld::sClientRegister
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
         * \param   src     The source of NEPublicHelloWorld::sClientRegister structure to copy data.
         **/
        inline sClientRegister( const NEPublicHelloWorld::sClientRegister & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEPublicHelloWorld::sClientRegister structure to move data.
         **/
        inline sClientRegister( NEPublicHelloWorld::sClientRegister && src ) noexcept;

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
        inline NEPublicHelloWorld::sClientRegister & operator = ( const NEPublicHelloWorld::sClientRegister & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sClientRegister structure to move data.
         **/
        inline NEPublicHelloWorld::sClientRegister & operator = ( NEPublicHelloWorld::sClientRegister && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sClientRegister are equal
         * \param   other   The instance of sClientRegister to compare
         **/ 
        inline bool operator == ( const NEPublicHelloWorld::sClientRegister & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sClientRegister are not equal
         * \param   other   The instance of sClientRegister to compare
         **/ 
        inline bool operator != ( const NEPublicHelloWorld::sClientRegister & other ) const;
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

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEPublicHelloWorld::eMessageIDs
     *          PublicHelloWorld Service interface defined message IDs
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
          MsgId_requestRegister       = NEService::REQUEST_ID_FIRST   // void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );
        /**
         * \brief   Request ID: MsgId_requestUnregister
         *          Sent to unregister connected client.
         **/
        , MsgId_requestUnregister                                       // void requestUnregister( const NEPublicHelloWorld::sClientRegister & client );
        /**
         * \brief   Request ID: MsgId_requestHelloWorld
         *          Outputs message on console.
         **/
        , MsgId_requestHelloWorld                                       // void requestHelloWorld( unsigned int clientID );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseRegister
         *          Sent when the client is registered. Each registered client has unique ID.
         **/
        , MsgId_responseRegister      = NEService::RESPONSE_ID_FIRST  // void responseRegister( const NEPublicHelloWorld::sClientRegister & client );
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
     * \brief   Returns string value of NEPublicHelloWorld::eMessageIDs
     **/
    inline const char * getString( NEPublicHelloWorld::eMessageIDs msgId );

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
    NEPublicHelloWorld::eMessageIDs getResponseId( NEPublicHelloWorld::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEPublicHelloWorld::eMessageIDs getRequestId( NEPublicHelloWorld::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEPublicHelloWorld inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEPublicHelloWorld methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEPublicHelloWorld::eMessageIDs );

/************************************************************************
 * NEPublicHelloWorld::sClientRegister strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEPublicHelloWorld::sClientRegister::sClientRegister( void )
    : crID        ( 0 )
    , crName      ( "" )
    , crService   (  )
    , crThread    ( "" )
    , crProcess   (  )
{   /* do nothing */    }

inline NEPublicHelloWorld::sClientRegister::sClientRegister( const unsigned int & _crID, const String & _crName, const ServiceAddress & _crService, const String & _crThread, const String & _crProcess )
    : crID        ( _crID )
    , crName      ( _crName )
    , crService   ( _crService )
    , crThread    ( _crThread )
    , crProcess   ( _crProcess )
{   /* do nothing */    }

inline NEPublicHelloWorld::sClientRegister::sClientRegister( const NEPublicHelloWorld::sClientRegister & src )
    : crID        ( src.crID )
    , crName      ( src.crName )
    , crService   ( src.crService )
    , crThread    ( src.crThread )
    , crProcess   ( src.crProcess )
{   /* do nothing */    }

inline NEPublicHelloWorld::sClientRegister::sClientRegister( NEPublicHelloWorld::sClientRegister && src ) noexcept
    : crID        ( std::move(src.crID) )
    , crName      ( std::move(src.crName) )
    , crService   ( std::move(src.crService) )
    , crThread    ( std::move(src.crThread) )
    , crProcess   ( std::move(src.crProcess) )
{   /* do nothing */    }

inline NEPublicHelloWorld::sClientRegister & NEPublicHelloWorld::sClientRegister::operator = ( const NEPublicHelloWorld::sClientRegister & src )
{
    if ( static_cast<const NEPublicHelloWorld::sClientRegister *>(this) != &src )
    {
            this->crID        = src.crID;
            this->crName      = src.crName;
            this->crService   = src.crService;
            this->crThread    = src.crThread;
            this->crProcess   = src.crProcess;
    }

    return (*this);
}

inline NEPublicHelloWorld::sClientRegister & NEPublicHelloWorld::sClientRegister::operator = ( NEPublicHelloWorld::sClientRegister && src ) noexcept
{
    if ( static_cast<const NEPublicHelloWorld::sClientRegister *>(this) != &src )
    {
            this->crID        = std::move(src.crID);
            this->crName      = std::move(src.crName);
            this->crService   = std::move(src.crService);
            this->crThread    = std::move(src.crThread);
            this->crProcess   = std::move(src.crProcess);
    }
    return (*this);
}

inline bool NEPublicHelloWorld::sClientRegister::operator == ( const NEPublicHelloWorld::sClientRegister & other ) const
{
    if ( static_cast<const NEPublicHelloWorld::sClientRegister *>(this) != &other )
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

inline bool NEPublicHelloWorld::sClientRegister::operator != ( const NEPublicHelloWorld::sClientRegister & other ) const
{
    if ( static_cast<const NEPublicHelloWorld::sClientRegister *>(this) != &other )
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


/**
 * Encode and decode NEPublicHelloWorld::sClientRegister structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEPublicHelloWorld::sClientRegister & input )
{
    stream >> input.crID;
    stream >> input.crName;
    stream >> input.crService;
    stream >> input.crThread;
    stream >> input.crProcess;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEPublicHelloWorld::sClientRegister & output )
{
    stream << output.crID;
    stream << output.crName;
    stream << output.crService;
    stream << output.crThread;
    stream << output.crProcess;
    return stream;
}

/**
 * Returns string value of NEPublicHelloWorld::eMessageIDs
 **/
inline const char * NEPublicHelloWorld::getString( NEPublicHelloWorld::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister";
    case    NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister";
    case    NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld";
    case    NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister";
    case    NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld";

    case NEPublicHelloWorld::eMessageIDs::MsgId_NotProcessed:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_NotProcessed";
    case NEPublicHelloWorld::eMessageIDs::MsgId_Invalid:
        return "NEPublicHelloWorld::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPublicHelloWorld::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPublicHelloWorld namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEPUBLICHELLOWORLD_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPublicHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
