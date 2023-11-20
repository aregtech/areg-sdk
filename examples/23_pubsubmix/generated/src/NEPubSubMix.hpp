//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPubSubMix.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPUBSUBMIX_HPP
#define  GENERATED_SRC_NEPUBSUBMIX_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:21 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/NEPubSubMix.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
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
// NEPubSubMix namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   PubSubMix Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This is an interface to test the PubSub features (data-driven communication) of the
 *          AREG framework, where each service provider is a Publisher and each service consumer is a Subscriber that dynamically
 *          subscribes on data update and get notified. The agenda of this example to demonstrate mixture of Publishers and Subscribers
 *          and testing the correct behavior of notifications when Subscribers run in the same or different processes.
 *
 **/
namespace   NEPubSubMix
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEPubSubMix::ServiceName
     **/
    constexpr   char    ServiceName[]   { "PubSubMix" };
    /**
     * \brief   Implemented Version.
     *          NEPubSubMix::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The timeout to make updates on change
     *          NEPubSubMix::TimeoutOnChange
     **/
    constexpr unsigned int    TimeoutOnChange     = 150;

    /**
     * \brief   The timeout to update on always.
     *          NEPubSubMix::TimeoutAlways
     **/
    constexpr unsigned int    TimeoutAlways       = 300;

    /**
     * \brief   The number of cycles to change the value when updating values.
     *          NEPubSubMix::CycleAlways
     **/
    constexpr unsigned short  CycleAlways         = 3;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The state of the Publisher (service provider)
     *          NEPubSubMix::eServiceState
     **/
    enum class eServiceState
    {
        /**
         * \brief   The service provider state is initialized. The values are in invalid states.
         **/
          Uninitialized
        /**
         * \brief   The service provider state is stopped, values are initialized and valid.
         **/
        , Stopped
        /**
         * \brief   The service provider state is running, the values should change values.
         **/
        , Running
        /**
         * \brief   The service provider shuts down and quits.
         **/
        , Shutdown
    };
    /**
     * \brief   Returns string value of NEPubSubMix::eServiceState
     **/
    inline const char * getString( NEPubSubMix::eServiceState enumValue );

    /**
     * \brief   The named structure for integer, 
     *          which contains integer as a value and name of a service, which updates the value.
     *          Every field of structure should have default constructor.
     *          NEPubSubMix::sInteger
     **/
    struct sInteger
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sInteger( void );

        /**
         * \brief   Initialize sInteger data by setting parameters.
         **/
        inline sInteger( const unsigned int & _value, const String & _name );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEPubSubMix::sInteger structure to copy data.
         **/
        inline sInteger( const NEPubSubMix::sInteger & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEPubSubMix::sInteger structure to move data.
         **/
        inline sInteger( NEPubSubMix::sInteger && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sInteger( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sInteger structure to copy data.
         **/
        inline NEPubSubMix::sInteger & operator = ( const NEPubSubMix::sInteger & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sInteger structure to move data.
         **/
        inline NEPubSubMix::sInteger & operator = ( NEPubSubMix::sInteger && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sInteger are equal
         * \param   other   The instance of sInteger to compare
         **/ 
        inline bool operator == ( const NEPubSubMix::sInteger & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sInteger are not equal
         * \param   other   The instance of sInteger to compare
         **/ 
        inline bool operator != ( const NEPubSubMix::sInteger & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sInteger fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The integer value.
         **/
         unsigned int    value   /* = 0 */;
        /**
         * \brief   The name of the service that updated value.
         **/
         String          name;
    };

    /**
     * \brief   The named structure for string, 
     *          which contains string as a value and name of a service, which updates the value.
     *          Every field of structure should have default constructor.
     *          NEPubSubMix::sString
     **/
    struct sString
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sString( void );

        /**
         * \brief   Initialize sString data by setting parameters.
         **/
        inline sString( const String & _value, const String & _name );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEPubSubMix::sString structure to copy data.
         **/
        inline sString( const NEPubSubMix::sString & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEPubSubMix::sString structure to move data.
         **/
        inline sString( NEPubSubMix::sString && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sString( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sString structure to copy data.
         **/
        inline NEPubSubMix::sString & operator = ( const NEPubSubMix::sString & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sString structure to move data.
         **/
        inline NEPubSubMix::sString & operator = ( NEPubSubMix::sString && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sString are equal
         * \param   other   The instance of sString to compare
         **/ 
        inline bool operator == ( const NEPubSubMix::sString & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sString are not equal
         * \param   other   The instance of sString to compare
         **/ 
        inline bool operator != ( const NEPubSubMix::sString & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sString fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The value
         **/
         String  value;
        /**
         * \brief   The name of the service that updated value.
         **/
         String  name;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEPubSubMix::eMessageIDs
     *          PubSubMix Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/

    /************************************************************************
     * Response IDs
     ************************************************************************/
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_StringOnChange
         *          This string should send update notification only on change.
         **/
          MsgId_StringOnChange        = NEService::ATTRIBUTE_ID_FIRST // NEPubSubMix::sString mStringOnChange;
        /**
         * \brief   Attribute ID: MsgId_IntegerAlways
         *          This  integer should send update notification always when set.
         **/
        , MsgId_IntegerAlways                                           // NEPubSubMix::sInteger mIntegerAlways;
        /**
         * \brief   Attribute ID: MsgId_ServiceProviderState
         *          The state of service provider. The value is always valid, updated on change.
         **/
        , MsgId_ServiceProviderState                                    // NEPubSubMix::eServiceState mServiceProviderState;

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
     * \brief   Returns string value of NEPubSubMix::eMessageIDs
     **/
    inline const char * getString( NEPubSubMix::eMessageIDs msgId );

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
    NEPubSubMix::eMessageIDs getResponseId( NEPubSubMix::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEPubSubMix::eMessageIDs getRequestId( NEPubSubMix::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEPubSubMix inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEPubSubMix methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEPubSubMix::eMessageIDs );

/************************************************************************
 * NEPubSubMix::eServiceState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEPubSubMix::eServiceState )

/**
 * Return string value of NEPubSubMix::eServiceState
 **/
inline const char * NEPubSubMix::getString( NEPubSubMix::eServiceState enumValue )
{
    switch ( enumValue )
    {
    case    NEPubSubMix::eServiceState::Uninitialized:
        return "NEPubSubMix::eServiceState::Uninitialized";
    case    NEPubSubMix::eServiceState::Stopped:
        return "NEPubSubMix::eServiceState::Stopped";
    case    NEPubSubMix::eServiceState::Running:
        return "NEPubSubMix::eServiceState::Running";
    case    NEPubSubMix::eServiceState::Shutdown:
        return "NEPubSubMix::eServiceState::Shutdown";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEPubSubMix::eServiceState value!";
    }
}

/************************************************************************
 * NEPubSubMix::sInteger strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEPubSubMix::sInteger::sInteger( void )
    : value   ( 0 )
    , name    (  )
{   /* do nothing */    }

inline NEPubSubMix::sInteger::sInteger( const unsigned int & _value, const String & _name )
    : value   ( _value )
    , name    ( _name )
{   /* do nothing */    }

inline NEPubSubMix::sInteger::sInteger( const NEPubSubMix::sInteger & src )
    : value   ( src.value )
    , name    ( src.name )
{   /* do nothing */    }

inline NEPubSubMix::sInteger::sInteger( NEPubSubMix::sInteger && src ) noexcept
    : value   ( std::move(src.value) )
    , name    ( std::move(src.name) )
{   /* do nothing */    }

inline NEPubSubMix::sInteger & NEPubSubMix::sInteger::operator = ( const NEPubSubMix::sInteger & src )
{
    if ( static_cast<const NEPubSubMix::sInteger *>(this) != &src )
    {
            this->value   = src.value;
            this->name    = src.name;
    }

    return (*this);
}

inline NEPubSubMix::sInteger & NEPubSubMix::sInteger::operator = ( NEPubSubMix::sInteger && src ) noexcept
{
    if ( static_cast<const NEPubSubMix::sInteger *>(this) != &src )
    {
            this->value   = std::move(src.value);
            this->name    = std::move(src.name);
    }
    return (*this);
}

inline bool NEPubSubMix::sInteger::operator == ( const NEPubSubMix::sInteger & other ) const
{
    if ( static_cast<const NEPubSubMix::sInteger *>(this) != &other )
    {
        return (
                   (this->value   == other.value)
                && (this->name    == other.name)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEPubSubMix::sInteger::operator != ( const NEPubSubMix::sInteger & other ) const
{
    if ( static_cast<const NEPubSubMix::sInteger *>(this) != &other )
    {
        return (
                   (this->value   != other.value)
                || (this->name    != other.name)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEPubSubMix::sInteger structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEPubSubMix::sInteger & input )
{
    stream >> input.value;
    stream >> input.name;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEPubSubMix::sInteger & output )
{
    stream << output.value;
    stream << output.name;
    return stream;
}

/************************************************************************
 * NEPubSubMix::sString strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEPubSubMix::sString::sString( void )
    : value   (  )
    , name    (  )
{   /* do nothing */    }

inline NEPubSubMix::sString::sString( const String & _value, const String & _name )
    : value   ( _value )
    , name    ( _name )
{   /* do nothing */    }

inline NEPubSubMix::sString::sString( const NEPubSubMix::sString & src )
    : value   ( src.value )
    , name    ( src.name )
{   /* do nothing */    }

inline NEPubSubMix::sString::sString( NEPubSubMix::sString && src ) noexcept
    : value   ( std::move(src.value) )
    , name    ( std::move(src.name) )
{   /* do nothing */    }

inline NEPubSubMix::sString & NEPubSubMix::sString::operator = ( const NEPubSubMix::sString & src )
{
    if ( static_cast<const NEPubSubMix::sString *>(this) != &src )
    {
            this->value   = src.value;
            this->name    = src.name;
    }

    return (*this);
}

inline NEPubSubMix::sString & NEPubSubMix::sString::operator = ( NEPubSubMix::sString && src ) noexcept
{
    if ( static_cast<const NEPubSubMix::sString *>(this) != &src )
    {
            this->value   = std::move(src.value);
            this->name    = std::move(src.name);
    }
    return (*this);
}

inline bool NEPubSubMix::sString::operator == ( const NEPubSubMix::sString & other ) const
{
    if ( static_cast<const NEPubSubMix::sString *>(this) != &other )
    {
        return (
                   (this->value   == other.value)
                && (this->name    == other.name)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEPubSubMix::sString::operator != ( const NEPubSubMix::sString & other ) const
{
    if ( static_cast<const NEPubSubMix::sString *>(this) != &other )
    {
        return (
                   (this->value   != other.value)
                || (this->name    != other.name)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEPubSubMix::sString structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEPubSubMix::sString & input )
{
    stream >> input.value;
    stream >> input.name;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEPubSubMix::sString & output )
{
    stream << output.value;
    stream << output.name;
    return stream;
}

/**
 * Returns string value of NEPubSubMix::eMessageIDs
 **/
inline const char * NEPubSubMix::getString( NEPubSubMix::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
        return "NEPubSubMix::eMessageIDs::MsgId_StringOnChange";
    case    NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
        return "NEPubSubMix::eMessageIDs::MsgId_IntegerAlways";
    case    NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
        return "NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState";

    case NEPubSubMix::eMessageIDs::MsgId_NotProcessed:
        return "NEPubSubMix::eMessageIDs::MsgId_NotProcessed";
    case NEPubSubMix::eMessageIDs::MsgId_Invalid:
        return "NEPubSubMix::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPubSubMix::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPubSubMix namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEPUBSUBMIX_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPubSubMix.hpp file
//////////////////////////////////////////////////////////////////////////
