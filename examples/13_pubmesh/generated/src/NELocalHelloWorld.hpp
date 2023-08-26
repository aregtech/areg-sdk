//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NELocalHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NELOCALHELLOWORLD_HPP
#define  GENERATED_SRC_NELOCALHELLOWORLD_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:26 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/NELocalHelloWorld.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
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
// NELocalHelloWorld namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   LocalHelloWorld Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          A Local Service Interface to demonstrate working features of AREG SDK.
 *
 **/
namespace   NELocalHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NELocalHelloWorld::ServiceName
     **/
    constexpr   char    ServiceName[]   { "LocalHelloWorld" };
    /**
     * \brief   Implemented Version.
     *          NELocalHelloWorld::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The dummy structure describing connected client.
     *          Every field of structure should have default constructor.
     *          NELocalHelloWorld::sConnectedClient
     **/
    struct sConnectedClient
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sConnectedClient( void );

        /**
         * \brief   Initialize sConnectedClient data by setting parameters.
         **/
        inline sConnectedClient( const unsigned int & _ccID, const String & _ccName );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NELocalHelloWorld::sConnectedClient structure to copy data.
         **/
        inline sConnectedClient( const NELocalHelloWorld::sConnectedClient & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NELocalHelloWorld::sConnectedClient structure to move data.
         **/
        inline sConnectedClient( NELocalHelloWorld::sConnectedClient && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sConnectedClient( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sConnectedClient structure to copy data.
         **/
        inline NELocalHelloWorld::sConnectedClient & operator = ( const NELocalHelloWorld::sConnectedClient & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sConnectedClient structure to move data.
         **/
        inline NELocalHelloWorld::sConnectedClient & operator = ( NELocalHelloWorld::sConnectedClient && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sConnectedClient are equal
         * \param   other   The instance of sConnectedClient to compare
         **/ 
        inline bool operator == ( const NELocalHelloWorld::sConnectedClient & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnectedClient are not equal
         * \param   other   The instance of sConnectedClient to compare
         **/ 
        inline bool operator != ( const NELocalHelloWorld::sConnectedClient & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sConnectedClient fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The ID of client, given by servicing component
         **/
         unsigned int    ccID    /* = 0 */;
        /**
         * \brief   The role name of client component. Parameter received by client.
         **/
         String          ccName;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NELocalHelloWorld::eMessageIDs
     *          LocalHelloWorld Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestHelloWorld
         *          Request to print hello world
         **/
          MsgId_requestHelloWorld     = NEService::REQUEST_ID_FIRST   // void requestHelloWorld( const String & roleName );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseHelloWorld
         *          The response to hello world request.
         **/
        , MsgId_responseHelloWorld    = NEService::RESPONSE_ID_FIRST  // void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo );
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
     * \brief   Returns string value of NELocalHelloWorld::eMessageIDs
     **/
    inline const char * getString( NELocalHelloWorld::eMessageIDs msgId );

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
    NELocalHelloWorld::eMessageIDs getResponseId( NELocalHelloWorld::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NELocalHelloWorld::eMessageIDs getRequestId( NELocalHelloWorld::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NELocalHelloWorld inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NELocalHelloWorld methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NELocalHelloWorld::eMessageIDs );

/************************************************************************
 * NELocalHelloWorld::sConnectedClient structure inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NELocalHelloWorld::sConnectedClient::sConnectedClient( void )
    : ccID    ( 0 )
    , ccName  (  )
{   /* do nothing */    }

inline NELocalHelloWorld::sConnectedClient::sConnectedClient( const unsigned int & _ccID, const String & _ccName )
    : ccID    ( _ccID )
    , ccName  ( _ccName )
{   /* do nothing */    }

inline NELocalHelloWorld::sConnectedClient::sConnectedClient( const NELocalHelloWorld::sConnectedClient & src )
    : ccID    ( src.ccID )
    , ccName  ( src.ccName )
{   /* do nothing */    }

inline NELocalHelloWorld::sConnectedClient::sConnectedClient( NELocalHelloWorld::sConnectedClient && src ) noexcept
    : ccID    ( std::move(src.ccID) )
    , ccName  ( std::move(src.ccName) )
{   /* do nothing */    }

inline NELocalHelloWorld::sConnectedClient & NELocalHelloWorld::sConnectedClient::operator = ( const NELocalHelloWorld::sConnectedClient & src )
{
    if ( static_cast<const NELocalHelloWorld::sConnectedClient *>(this) != &src )
    {
            this->ccID    = src.ccID;
            this->ccName  = src.ccName;
    }

    return (*this);
}

inline NELocalHelloWorld::sConnectedClient & NELocalHelloWorld::sConnectedClient::operator = ( NELocalHelloWorld::sConnectedClient && src ) noexcept
{
    if ( static_cast<const NELocalHelloWorld::sConnectedClient *>(this) != &src )
    {
            this->ccID    = std::move(src.ccID);
            this->ccName  = std::move(src.ccName);
    }
    return (*this);
}

inline bool NELocalHelloWorld::sConnectedClient::operator == ( const NELocalHelloWorld::sConnectedClient & other ) const
{
    if ( static_cast<const NELocalHelloWorld::sConnectedClient *>(this) != &other )
    {
        return (
                   (this->ccID    == other.ccID)
                && (this->ccName  == other.ccName)
                );
    }
    else
    {
        return true;
    }
}

inline bool NELocalHelloWorld::sConnectedClient::operator != ( const NELocalHelloWorld::sConnectedClient & other ) const
{
    if ( static_cast<const NELocalHelloWorld::sConnectedClient *>(this) != &other )
    {
        return (
                   (this->ccID    != other.ccID)
                || (this->ccName  != other.ccName)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NELocalHelloWorld::sConnectedClient structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NELocalHelloWorld::sConnectedClient & input )
{
    stream >> input.ccID;
    stream >> input.ccName;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NELocalHelloWorld::sConnectedClient & output )
{
    stream << output.ccID;
    stream << output.ccName;
    return stream;
}

/**
 * Returns string value of NELocalHelloWorld::eMessageIDs
 **/
inline const char * NELocalHelloWorld::getString( NELocalHelloWorld::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        return "NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld";
    case    NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        return "NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld";

    case NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed:
        return "NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed";
    case NELocalHelloWorld::eMessageIDs::MsgId_Invalid:
        return "NELocalHelloWorld::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NELocalHelloWorld::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NELocalHelloWorld namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NELOCALHELLOWORLD_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NELocalHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
