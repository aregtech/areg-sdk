//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#pragma once

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:11 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/NEHelloWorld.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
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
// NEHelloWorld namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   HelloWorld Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *
 **/
namespace   NEHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NEHelloWorld::ServiceName
     **/
    constexpr char        ServiceName[]       { "HelloWorld" };
    /**
     * \brief   Implemented Version
     *          NEHelloWorld::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The maximum number of messages to output.
     *          NEHelloWorld::MaxMessages
     **/
    constexpr short           MaxMessages         = 37;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The dummy structure describing connected client.
     *          Every field of structure should have default constructor.
     *          NEHelloWorld::sConnectedClient
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
         * \param   src     The source of NEHelloWorld::sConnectedClient structure to copy data.
         **/
        inline sConnectedClient( const NEHelloWorld::sConnectedClient & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEHelloWorld::sConnectedClient structure to move data.
         **/
        inline sConnectedClient( NEHelloWorld::sConnectedClient && src ) noexcept;

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
        inline NEHelloWorld::sConnectedClient & operator = ( const NEHelloWorld::sConnectedClient & src );
        
        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sConnectedClient structure to move data.
         **/
        inline NEHelloWorld::sConnectedClient & operator = ( NEHelloWorld::sConnectedClient && src ) noexcept;
        
        /**
         * \brief   Checks equality and returns true if 2 instances of sConnectedClient are equal
         * \param   other   The instance of sConnectedClient to compare
         * \return  Returns true if 2 instances are equal
         **/ 
        inline bool operator == ( const NEHelloWorld::sConnectedClient & other ) const;
        
        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnectedClient are not equal
         * \param   other   The instance of sConnectedClient to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
        inline bool operator != ( const NEHelloWorld::sConnectedClient & other ) const;

        /**
         * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
         **/
         inline explicit operator unsigned int ( void ) const;

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

    /**
     * \brief   The list of connected clients.
     *          NEHelloWorld::ConnectionList
     **/
     typedef    TELinkedList< sConnectedClient >   ConnectionList;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEHelloWorld::eMessageIDs
     *          HelloWorld Service interface defined message IDs
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
          MsgId_requestHelloWorld             = NEService::REQUEST_ID_FIRST     // void requestHelloWorld( const String & roleName, const String & addMessage = "" );
        /**
         * \brief   Request ID: MsgId_requestClientShutdown
         *          Sent by client to notify the shutdown. This removes client from the list. This request has no response.
         **/
        , MsgId_requestClientShutdown                                           // void requestClientShutdown( unsigned int clientID, const String & roleName );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseHelloWorld
         *          The response to hello world request.
         **/
        , MsgId_responseHelloWorld            = NEService::RESPONSE_ID_FIRST    // void responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastHelloClients
         *          Broadcast to notify all clients about connection
         **/
        , MsgId_broadcastHelloClients                                           // void broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList );
        /**
         * \brief   Broadcast ID: MsgId_broadcastServiceUnavailable
         *          DESCRIPTION MISSED
         **/
        , MsgId_broadcastServiceUnavailable                                     // void broadcastServiceUnavailable( void );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ConnectedClients
         *          The list of connected clients. Updated each time when new client requests to output Hello World message.
         **/
        , MsgId_ConnectedClients              = NEService::ATTRIBUTE_ID_FIRST   // NEHelloWorld::ConnectionList mConnectedClients;
        /**
         * \brief   Attribute ID: MsgId_RemainOutput
         *          Remaining number of outputs to print Hello World.
         **/
        , MsgId_RemainOutput                                                    // short mRemainOutput;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed                  = NEService::INVALID_MESSAGE_ID  
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                       = NEService::RESPONSE_ID_NONE    
     
    };
    /**
     * \brief   Returns string value of NEHelloWorld::eMessageIDs
     **/
    inline const char * getString( NEHelloWorld::eMessageIDs msgId );
     
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
    NEHelloWorld::eMessageIDs getResponseId( NEHelloWorld::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEHelloWorld::eMessageIDs getRequestId( NEHelloWorld::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEHelloWorld methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEHelloWorld::eMessageIDs );

/************************************************************************
 * NEHelloWorld::sConnectedClient strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEHelloWorld::sConnectedClient::sConnectedClient( void )
    : ccID    ( 0 )
    , ccName  (  )
{   /* do nothing */    }

inline NEHelloWorld::sConnectedClient::sConnectedClient( const unsigned int & _ccID, const String & _ccName )
    : ccID    ( _ccID )
    , ccName  ( _ccName )
{   /* do nothing */    }

inline NEHelloWorld::sConnectedClient::sConnectedClient( const NEHelloWorld::sConnectedClient & src )
    : ccID    ( src.ccID )
    , ccName  ( src.ccName )
{   /* do nothing */    }

inline NEHelloWorld::sConnectedClient::sConnectedClient( NEHelloWorld::sConnectedClient && src ) noexcept
    : ccID    ( std::move(src.ccID) )
    , ccName  ( std::move(src.ccName) )
{   /* do nothing */    }

inline NEHelloWorld::sConnectedClient & NEHelloWorld::sConnectedClient::operator = ( const NEHelloWorld::sConnectedClient & src )
{
    if ( static_cast<const NEHelloWorld::sConnectedClient *>(this) != &src )
    {
            this->ccID    = src.ccID;
            this->ccName  = src.ccName;
    }

    return (*this);
}

inline NEHelloWorld::sConnectedClient & NEHelloWorld::sConnectedClient::operator = ( NEHelloWorld::sConnectedClient && src ) noexcept
{
    if ( static_cast<const NEHelloWorld::sConnectedClient *>(this) != &src )
    {
            this->ccID    = std::move(src.ccID);
            this->ccName  = std::move(src.ccName);
    }
    return (*this);
}

inline bool NEHelloWorld::sConnectedClient::operator == ( const NEHelloWorld::sConnectedClient & other ) const
{
    if ( static_cast<const NEHelloWorld::sConnectedClient *>(this) != &other )
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

inline bool NEHelloWorld::sConnectedClient::operator != ( const NEHelloWorld::sConnectedClient & other ) const
{
    if ( static_cast<const NEHelloWorld::sConnectedClient *>(this) != &other )
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

inline NEHelloWorld::sConnectedClient::operator unsigned int ( void ) const
{
    return (
              static_cast<unsigned int>( ccID     )
            + static_cast<unsigned int>( ccName   )
            );
}

/**
 * Encode and decode NEHelloWorld::sConnectedClient structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEHelloWorld::sConnectedClient & input )
{
    stream >> input.ccID;
    stream >> input.ccName;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEHelloWorld::sConnectedClient & output )
{
    stream << output.ccID;
    stream << output.ccName;
    return stream;
}

/**
 * Returns string value of NEHelloWorld::eMessageIDs
 **/
inline const char * NEHelloWorld::getString( NEHelloWorld::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        return "NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld";
    case    NEHelloWorld::eMessageIDs::MsgId_requestClientShutdown:
        return "NEHelloWorld::eMessageIDs::MsgId_requestClientShutdown";
    case    NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        return "NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld";
    case    NEHelloWorld::eMessageIDs::MsgId_broadcastHelloClients:
        return "NEHelloWorld::eMessageIDs::MsgId_broadcastHelloClients";
    case    NEHelloWorld::eMessageIDs::MsgId_broadcastServiceUnavailable:
        return "NEHelloWorld::eMessageIDs::MsgId_broadcastServiceUnavailable";
    case    NEHelloWorld::eMessageIDs::MsgId_ConnectedClients:
        return "NEHelloWorld::eMessageIDs::MsgId_ConnectedClients";
    case    NEHelloWorld::eMessageIDs::MsgId_RemainOutput:
        return "NEHelloWorld::eMessageIDs::MsgId_RemainOutput";

    case NEHelloWorld::eMessageIDs::MsgId_NotProcessed:
        return "NEHelloWorld::eMessageIDs::MsgId_NotProcessed";
        
    case NEHelloWorld::eMessageIDs::MsgId_Invalid:
        return "NEHelloWorld::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEHelloWorld::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld namespace declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
