//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/NELocalHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_NELOCALHELLOWORLD_HPP
#define  SHAREIPCMIX_SRC_NELOCALHELLOWORLD_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     29.05.2021  12:42:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/NELocalHelloWorld.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Namespace declaration.
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
 *          
 *          		Simple Service Interface to demonstrate working features of AREG SDK.
 *          		This interface serves only local components and can be duplicated in other processes.
 *          		
 *
 **/
namespace   NELocalHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NELocalHelloWorld::ServiceName
     **/
    const char * const    ServiceName         = "LocalHelloWorld";
    /**
     * \brief   Implemented Version
     *          NELocalHelloWorld::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The maximum number of messages to output.
     *          NELocalHelloWorld::MaxMessages
     **/
    const short           MaxMessages         = 37;

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
         * \param   src     The source of NELocalHelloWorld::sConnectedClient structure to copy data
         **/
        inline sConnectedClient( const NELocalHelloWorld::sConnectedClient & src );

        /**
         * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
         **/
        inline ~sConnectedClient( void );
    
    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////
        
        /**
         * \brief   Copies data from given source
         * \param   src     The source of sConnectedClient structure to copy data.
         **/
        inline const NELocalHelloWorld::sConnectedClient & operator = ( const NELocalHelloWorld::sConnectedClient & src );
        
        /**
         * \brief   Checks equality and returns true if 2 instances of sConnectedClient are equal
         * \param   other   The instance of sConnectedClient to compare
         * \return  Returns true if 2 instances are equal
         **/ 
        inline bool operator == ( const NELocalHelloWorld::sConnectedClient & other ) const;
        
        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnectedClient are not equal
         * \param   other   The instance of sConnectedClient to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
        inline bool operator != ( const NELocalHelloWorld::sConnectedClient & other ) const;

        /**
         * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
         **/
         inline operator unsigned int ( void ) const;

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
     *          NELocalHelloWorld::ConnectionList
     **/
     typedef    TELinkedList< sConnectedClient >   ConnectionList;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NELocalHelloWorld::eMessageIDs
     *          LocalHelloWorld Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestHelloWorld
         *          Request to print hello world
         **/
          MSG_ID_requestHelloWorld            = NEService::REQUEST_ID_FIRST     // void requestHelloWorld( const String & roleName, const String & addMessage = "" );
        /**
         * \brief   Request ID: MSG_ID_requestClientShutdown
         *          Sent by client to notify the shutdown. This removes client from the list. This request has no response.
         **/
        , MSG_ID_requestClientShutdown                                          // void requestClientShutdown( unsigned int clientID, const String & roleName );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseHelloWorld
         *          The response to hello world request.
         **/
        , MSG_ID_responseHelloWorld           = NEService::RESPONSE_ID_FIRST    // void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastHelloClients
         *          Broadcast to notify all clients about connection
         **/
        , MSG_ID_broadcastHelloClients                                          // void broadcastHelloClients( const NELocalHelloWorld::ConnectionList & clientList );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastServiceUnavailable
         *          DESCRIPTION MISSED
         **/
        , MSG_ID_broadcastServiceUnavailable                                    // void broadcastServiceUnavailable( void );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_ConnectedClients
         *          The list of connected clients. Updated each time when new client requests to output Hello World message.
         **/
        , MSG_ID_ConnectedClients             = NEService::ATTRIBUTE_ID_FIRST   // NELocalHelloWorld::ConnectionList mConnectedClients;
        /**
         * \brief   Attribute ID: MSG_ID_RemainOutput
         *          Remaining number of outputs to print Hello World.
         **/
        , MSG_ID_RemainOutput                                                   // short mRemainOutput;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED                   = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID                      = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
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
 * NELocalHelloWorld::sConnectedClient strcuture inline implementation
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

inline NELocalHelloWorld::sConnectedClient::~sConnectedClient( void )
{   /* do nothing */    }

inline const NELocalHelloWorld::sConnectedClient & NELocalHelloWorld::sConnectedClient::operator = ( const NELocalHelloWorld::sConnectedClient & src )
{
    if ( static_cast<const NELocalHelloWorld::sConnectedClient *>(this) != &src )
    {
            this->ccID    = src.ccID;
            this->ccName  = src.ccName;
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

inline NELocalHelloWorld::sConnectedClient::operator unsigned int ( void ) const
{
    return (
              static_cast<unsigned int>( ccID     )
            + static_cast<unsigned int>( ccName   )
            );
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
    case    NELocalHelloWorld::MSG_ID_requestHelloWorld:
        return "NELocalHelloWorld::MSG_ID_requestHelloWorld";
    case    NELocalHelloWorld::MSG_ID_requestClientShutdown:
        return "NELocalHelloWorld::MSG_ID_requestClientShutdown";
    case    NELocalHelloWorld::MSG_ID_responseHelloWorld:
        return "NELocalHelloWorld::MSG_ID_responseHelloWorld";
    case    NELocalHelloWorld::MSG_ID_broadcastHelloClients:
        return "NELocalHelloWorld::MSG_ID_broadcastHelloClients";
    case    NELocalHelloWorld::MSG_ID_broadcastServiceUnavailable:
        return "NELocalHelloWorld::MSG_ID_broadcastServiceUnavailable";
    case    NELocalHelloWorld::MSG_ID_ConnectedClients:
        return "NELocalHelloWorld::MSG_ID_ConnectedClients";
    case    NELocalHelloWorld::MSG_ID_RemainOutput:
        return "NELocalHelloWorld::MSG_ID_RemainOutput";

    case NELocalHelloWorld::MSG_ID_NO_PROCEED:
        return "NELocalHelloWorld::MSG_ID_NO_PROCEED";
        
    case NELocalHelloWorld::MSG_ID_INVALID:
        return "NELocalHelloWorld::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NELocalHelloWorld::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NELocalHelloWorld namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHAREIPCMIX_SRC_NELOCALHELLOWORLD_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/NELocalHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
