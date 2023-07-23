//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPatientInformation.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPATIENTINFORMATION_HPP
#define  GENERATED_SRC_NEPATIENTINFORMATION_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:51 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/NEPatientInformation.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
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
// NEPatientInformation namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   PatientInformation Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This service sends the patient information to apply connected devices.
 *
 **/
namespace   NEPatientInformation
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEPatientInformation::ServiceName
     **/
    constexpr   char    ServiceName[]   { "PatientInformation" };
    /**
     * \brief   Implemented Version.
     *          NEPatientInformation::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The timeout in milliseconds to apply the patient information.
     *          NEPatientInformation::Timeout
     **/
    constexpr unsigned int    Timeout             = 500;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The patien information
     *          Every field of structure should have default constructor.
     *          NEPatientInformation::PatientInfo
     **/
    struct PatientInfo
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline PatientInfo( void );

        /**
         * \brief   Initialize PatientInfo data by setting parameters.
         **/
        inline PatientInfo( const String & _firstName, const String & _lastName, const double & _weight, const unsigned char & _age );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEPatientInformation::PatientInfo structure to copy data.
         **/
        inline PatientInfo( const NEPatientInformation::PatientInfo & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEPatientInformation::PatientInfo structure to move data.
         **/
        inline PatientInfo( NEPatientInformation::PatientInfo && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~PatientInfo( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of PatientInfo structure to copy data.
         **/
        inline NEPatientInformation::PatientInfo & operator = ( const NEPatientInformation::PatientInfo & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of PatientInfo structure to move data.
         **/
        inline NEPatientInformation::PatientInfo & operator = ( NEPatientInformation::PatientInfo && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of PatientInfo are equal
         * \param   other   The instance of PatientInfo to compare
         **/ 
        inline bool operator == ( const NEPatientInformation::PatientInfo & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of PatientInfo are not equal
         * \param   other   The instance of PatientInfo to compare
         **/ 
        inline bool operator != ( const NEPatientInformation::PatientInfo & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // PatientInfo fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The first name of patient.
         **/
         String          firstName;
        /**
         * \brief   The last name of patient.
         **/
         String          lastName;
        /**
         * \brief   The weight of patient in kilogramms.
         **/
         double          weight      /* = 0.0 */;
        /**
         * \brief   The age of patient.
         **/
         unsigned char   age         /* = 0 */;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEPatientInformation::eMessageIDs
     *          PatientInformation Service interface defined message IDs
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
         * \brief   Attribute ID: MsgId_Patient
         *          The patient information to apply to hardware.
         **/
          MsgId_Patient       = NEService::ATTRIBUTE_ID_FIRST // NEPatientInformation::PatientInfo mPatient;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed  = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid       = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEPatientInformation::eMessageIDs
     **/
    inline const char * getString( NEPatientInformation::eMessageIDs msgId );

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
    NEPatientInformation::eMessageIDs getResponseId( NEPatientInformation::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEPatientInformation::eMessageIDs getRequestId( NEPatientInformation::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEPatientInformation inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEPatientInformation methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEPatientInformation::eMessageIDs );

/************************************************************************
 * NEPatientInformation::PatientInfo strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEPatientInformation::PatientInfo::PatientInfo( void )
    : firstName   (  )
    , lastName    (  )
    , weight      ( 0.0 )
    , age         ( 0 )
{   /* do nothing */    }

inline NEPatientInformation::PatientInfo::PatientInfo( const String & _firstName, const String & _lastName, const double & _weight, const unsigned char & _age )
    : firstName   ( _firstName )
    , lastName    ( _lastName )
    , weight      ( _weight )
    , age         ( _age )
{   /* do nothing */    }

inline NEPatientInformation::PatientInfo::PatientInfo( const NEPatientInformation::PatientInfo & src )
    : firstName   ( src.firstName )
    , lastName    ( src.lastName )
    , weight      ( src.weight )
    , age         ( src.age )
{   /* do nothing */    }

inline NEPatientInformation::PatientInfo::PatientInfo( NEPatientInformation::PatientInfo && src ) noexcept
    : firstName   ( std::move(src.firstName) )
    , lastName    ( std::move(src.lastName) )
    , weight      ( std::move(src.weight) )
    , age         ( std::move(src.age) )
{   /* do nothing */    }

inline NEPatientInformation::PatientInfo & NEPatientInformation::PatientInfo::operator = ( const NEPatientInformation::PatientInfo & src )
{
    if ( static_cast<const NEPatientInformation::PatientInfo *>(this) != &src )
    {
            this->firstName   = src.firstName;
            this->lastName    = src.lastName;
            this->weight      = src.weight;
            this->age         = src.age;
    }

    return (*this);
}

inline NEPatientInformation::PatientInfo & NEPatientInformation::PatientInfo::operator = ( NEPatientInformation::PatientInfo && src ) noexcept
{
    if ( static_cast<const NEPatientInformation::PatientInfo *>(this) != &src )
    {
            this->firstName   = std::move(src.firstName);
            this->lastName    = std::move(src.lastName);
            this->weight      = std::move(src.weight);
            this->age         = std::move(src.age);
    }
    return (*this);
}

inline bool NEPatientInformation::PatientInfo::operator == ( const NEPatientInformation::PatientInfo & other ) const
{
    if ( static_cast<const NEPatientInformation::PatientInfo *>(this) != &other )
    {
        return (
                   (this->firstName   == other.firstName)
                && (this->lastName    == other.lastName)
                && (this->weight      == other.weight)
                && (this->age         == other.age)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEPatientInformation::PatientInfo::operator != ( const NEPatientInformation::PatientInfo & other ) const
{
    if ( static_cast<const NEPatientInformation::PatientInfo *>(this) != &other )
    {
        return (
                   (this->firstName   != other.firstName)
                || (this->lastName    != other.lastName)
                || (this->weight      != other.weight)
                || (this->age         != other.age)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEPatientInformation::PatientInfo structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEPatientInformation::PatientInfo & input )
{
    stream >> input.firstName;
    stream >> input.lastName;
    stream >> input.weight;
    stream >> input.age;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEPatientInformation::PatientInfo & output )
{
    stream << output.firstName;
    stream << output.lastName;
    stream << output.weight;
    stream << output.age;
    return stream;
}

/**
 * Returns string value of NEPatientInformation::eMessageIDs
 **/
inline const char * NEPatientInformation::getString( NEPatientInformation::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEPatientInformation::eMessageIDs::MsgId_Patient:
        return "NEPatientInformation::eMessageIDs::MsgId_Patient";

    case NEPatientInformation::eMessageIDs::MsgId_NotProcessed:
        return "NEPatientInformation::eMessageIDs::MsgId_NotProcessed";
    case NEPatientInformation::eMessageIDs::MsgId_Invalid:
        return "NEPatientInformation::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPatientInformation::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPatientInformation namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEPATIENTINFORMATION_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPatientInformation.hpp file
//////////////////////////////////////////////////////////////////////////
