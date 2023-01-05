//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NETrafficController.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NETRAFFICCONTROLLER_HPP
#define  GENERATED_SRC_NETRAFFICCONTROLLER_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/NETrafficController.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////

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
// NETrafficController namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   TrafficController Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This is a traffic light controlle service interface.
 *
 **/
namespace   NETrafficController
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NETrafficController::ServiceName
     **/
    constexpr   char    ServiceName[]   { "TrafficController" };
    /**
     * \brief   Implemented Version.
     *          NETrafficController::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The direction of traffic.
     *          NETrafficController::eTrafficDirection
     **/
    enum class eTrafficDirection
    {
        /**
         * \brief   Undefined direction.
         **/
          DirectionUndefiend
        /**
         * \brief   The traffic direction is south-north.
         **/
        , DirectionSouthNorth
        /**
         * \brief   The traffic direction is east-west.
         **/
        , DirectionEastWest
    };
    /**
     * \brief   Returns string value of NETrafficController::eTrafficDirection
     **/
    inline const char * getString( NETrafficController::eTrafficDirection enumValue );

    /**
     * \brief   The states of vehicle traffic states.
     *          NETrafficController::eVehicleTrafficLight
     **/
    enum class eVehicleTrafficLight
    {
        /**
         * \brief   The vehicle lights are OFF.
         **/
          VehicleLightOFF
        /**
         * \brief   The vehicle lights are initialized (blink yellow).
         **/
        , VehicleLightsInit
        /**
         * \brief   Vehicle light is red.
         **/
        , VehicleLightRed
        /**
         * \brief   Vehicle light is yellow.
         **/
        , VehicleLightYellow
        /**
         * \brief   The vehicle light is green.
         **/
        , VehicleLightGreen
    };
    /**
     * \brief   Returns string value of NETrafficController::eVehicleTrafficLight
     **/
    inline const char * getString( NETrafficController::eVehicleTrafficLight enumValue );

    /**
     * \brief   The traffic light state of pedestrians.
     *          NETrafficController::ePedestrianTrafficLight
     **/
    enum class ePedestrianTrafficLight
    {
        /**
         * \brief   The pedestrian light is OFF.
         **/
          PedestrianLightOFF
        /**
         * \brief   Pedestrian light is red.
         **/
        , PedestrianLightRed
        /**
         * \brief   The pedestrian light is green.
         **/
        , PedestrianLightGreen
    };
    /**
     * \brief   Returns string value of NETrafficController::ePedestrianTrafficLight
     **/
    inline const char * getString( NETrafficController::ePedestrianTrafficLight enumValue );

    /**
     * \brief   Describes the direction of traffic and the states of lights for vehicle and pedestrians.
     *          Every field of structure should have default constructor.
     *          NETrafficController::sTrafficLight
     **/
    struct sTrafficLight
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sTrafficLight( void );

        /**
         * \brief   Initialize sTrafficLight data by setting parameters.
         **/
        inline sTrafficLight( const NETrafficController::eTrafficDirection & _trafficDirection, const NETrafficController::eVehicleTrafficLight & _lightVehicle, const NETrafficController::ePedestrianTrafficLight & _lightPedestrian );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NETrafficController::sTrafficLight structure to copy data.
         **/
        inline sTrafficLight( const NETrafficController::sTrafficLight & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NETrafficController::sTrafficLight structure to move data.
         **/
        inline sTrafficLight( NETrafficController::sTrafficLight && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sTrafficLight( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sTrafficLight structure to copy data.
         **/
        inline NETrafficController::sTrafficLight & operator = ( const NETrafficController::sTrafficLight & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sTrafficLight structure to move data.
         **/
        inline NETrafficController::sTrafficLight & operator = ( NETrafficController::sTrafficLight && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sTrafficLight are equal
         * \param   other   The instance of sTrafficLight to compare
         **/ 
        inline bool operator == ( const NETrafficController::sTrafficLight & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sTrafficLight are not equal
         * \param   other   The instance of sTrafficLight to compare
         **/ 
        inline bool operator != ( const NETrafficController::sTrafficLight & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sTrafficLight fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The traffic direction
         **/
         NETrafficController::eTrafficDirection          trafficDirection    /* = eTrafficDirection::DirectionUndefiend */;
        /**
         * \brief   The state of vehicle lights.
         **/
         NETrafficController::eVehicleTrafficLight       lightVehicle        /* = eVehicleTrafficLight::VehicleLightOFF */;
        /**
         * \brief   Describes the state of pedestrian lights.
         **/
         NETrafficController::ePedestrianTrafficLight    lightPedestrian     /* = ePedestrianTrafficLight::PedestrianLightOFF */;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NETrafficController::eMessageIDs
     *          TrafficController Service interface defined message IDs
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
        /**
         * \brief   Broadcast ID: MsgId_broadcastSouthNorth
         *          Broadcasts the light states of south-north direction.
         **/
          MsgId_broadcastSouthNorth   = NEService::RESPONSE_ID_FIRST  // void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );
        /**
         * \brief   Broadcast ID: MsgId_broadcastEastWest
         *          Broadcasts the light states of east-west direction.
         **/
        , MsgId_broadcastEastWest                                       // void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_TrafficSouthNorth
         *          The south-north traffic.
         **/
        , MsgId_TrafficSouthNorth     = NEService::ATTRIBUTE_ID_FIRST // NETrafficController::sTrafficLight mTrafficSouthNorth;
        /**
         * \brief   Attribute ID: MsgId_TrafficEastWest
         *          The east-west traffic
         **/
        , MsgId_TrafficEastWest                                         // NETrafficController::sTrafficLight mTrafficEastWest;

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
     * \brief   Returns string value of NETrafficController::eMessageIDs
     **/
    inline const char * getString( NETrafficController::eMessageIDs msgId );

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
    NETrafficController::eMessageIDs getResponseId( NETrafficController::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NETrafficController::eMessageIDs getRequestId( NETrafficController::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NETrafficController inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NETrafficController methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NETrafficController::eMessageIDs );

/************************************************************************
 * NETrafficController::eTrafficDirection enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NETrafficController::eTrafficDirection )

/**
 * Return string value of NETrafficController::eTrafficDirection
 **/
inline const char * NETrafficController::getString( NETrafficController::eTrafficDirection enumValue )
{
    switch ( enumValue )
    {
    case    NETrafficController::eTrafficDirection::DirectionUndefiend:
        return "NETrafficController::eTrafficDirection::DirectionUndefiend";
    case    NETrafficController::eTrafficDirection::DirectionSouthNorth:
        return "NETrafficController::eTrafficDirection::DirectionSouthNorth";
    case    NETrafficController::eTrafficDirection::DirectionEastWest:
        return "NETrafficController::eTrafficDirection::DirectionEastWest";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NETrafficController::eTrafficDirection value!";
    }
}

/************************************************************************
 * NETrafficController::eVehicleTrafficLight enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NETrafficController::eVehicleTrafficLight )

/**
 * Return string value of NETrafficController::eVehicleTrafficLight
 **/
inline const char * NETrafficController::getString( NETrafficController::eVehicleTrafficLight enumValue )
{
    switch ( enumValue )
    {
    case    NETrafficController::eVehicleTrafficLight::VehicleLightOFF:
        return "NETrafficController::eVehicleTrafficLight::VehicleLightOFF";
    case    NETrafficController::eVehicleTrafficLight::VehicleLightsInit:
        return "NETrafficController::eVehicleTrafficLight::VehicleLightsInit";
    case    NETrafficController::eVehicleTrafficLight::VehicleLightRed:
        return "NETrafficController::eVehicleTrafficLight::VehicleLightRed";
    case    NETrafficController::eVehicleTrafficLight::VehicleLightYellow:
        return "NETrafficController::eVehicleTrafficLight::VehicleLightYellow";
    case    NETrafficController::eVehicleTrafficLight::VehicleLightGreen:
        return "NETrafficController::eVehicleTrafficLight::VehicleLightGreen";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NETrafficController::eVehicleTrafficLight value!";
    }
}

/************************************************************************
 * NETrafficController::ePedestrianTrafficLight enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NETrafficController::ePedestrianTrafficLight )

/**
 * Return string value of NETrafficController::ePedestrianTrafficLight
 **/
inline const char * NETrafficController::getString( NETrafficController::ePedestrianTrafficLight enumValue )
{
    switch ( enumValue )
    {
    case    NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF:
        return "NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF";
    case    NETrafficController::ePedestrianTrafficLight::PedestrianLightRed:
        return "NETrafficController::ePedestrianTrafficLight::PedestrianLightRed";
    case    NETrafficController::ePedestrianTrafficLight::PedestrianLightGreen:
        return "NETrafficController::ePedestrianTrafficLight::PedestrianLightGreen";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NETrafficController::ePedestrianTrafficLight value!";
    }
}

/************************************************************************
 * NETrafficController::sTrafficLight strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NETrafficController::sTrafficLight::sTrafficLight( void )
    : trafficDirection    ( eTrafficDirection::DirectionUndefiend )
    , lightVehicle        ( eVehicleTrafficLight::VehicleLightOFF )
    , lightPedestrian     ( ePedestrianTrafficLight::PedestrianLightOFF )
{   /* do nothing */    }

inline NETrafficController::sTrafficLight::sTrafficLight( const NETrafficController::eTrafficDirection & _trafficDirection, const NETrafficController::eVehicleTrafficLight & _lightVehicle, const NETrafficController::ePedestrianTrafficLight & _lightPedestrian )
    : trafficDirection    ( _trafficDirection )
    , lightVehicle        ( _lightVehicle )
    , lightPedestrian     ( _lightPedestrian )
{   /* do nothing */    }

inline NETrafficController::sTrafficLight::sTrafficLight( const NETrafficController::sTrafficLight & src )
    : trafficDirection    ( src.trafficDirection )
    , lightVehicle        ( src.lightVehicle )
    , lightPedestrian     ( src.lightPedestrian )
{   /* do nothing */    }

inline NETrafficController::sTrafficLight::sTrafficLight( NETrafficController::sTrafficLight && src ) noexcept
    : trafficDirection    ( std::move(src.trafficDirection) )
    , lightVehicle        ( std::move(src.lightVehicle) )
    , lightPedestrian     ( std::move(src.lightPedestrian) )
{   /* do nothing */    }

inline NETrafficController::sTrafficLight & NETrafficController::sTrafficLight::operator = ( const NETrafficController::sTrafficLight & src )
{
    if ( static_cast<const NETrafficController::sTrafficLight *>(this) != &src )
    {
            this->trafficDirection    = src.trafficDirection;
            this->lightVehicle        = src.lightVehicle;
            this->lightPedestrian     = src.lightPedestrian;
    }

    return (*this);
}

inline NETrafficController::sTrafficLight & NETrafficController::sTrafficLight::operator = ( NETrafficController::sTrafficLight && src ) noexcept
{
    if ( static_cast<const NETrafficController::sTrafficLight *>(this) != &src )
    {
            this->trafficDirection    = std::move(src.trafficDirection);
            this->lightVehicle        = std::move(src.lightVehicle);
            this->lightPedestrian     = std::move(src.lightPedestrian);
    }
    return (*this);
}

inline bool NETrafficController::sTrafficLight::operator == ( const NETrafficController::sTrafficLight & other ) const
{
    if ( static_cast<const NETrafficController::sTrafficLight *>(this) != &other )
    {
        return (
                   (this->trafficDirection    == other.trafficDirection)
                && (this->lightVehicle        == other.lightVehicle)
                && (this->lightPedestrian     == other.lightPedestrian)
                );
    }
    else
    {
        return true;
    }
}

inline bool NETrafficController::sTrafficLight::operator != ( const NETrafficController::sTrafficLight & other ) const
{
    if ( static_cast<const NETrafficController::sTrafficLight *>(this) != &other )
    {
        return (
                   (this->trafficDirection    != other.trafficDirection)
                || (this->lightVehicle        != other.lightVehicle)
                || (this->lightPedestrian     != other.lightPedestrian)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NETrafficController::sTrafficLight structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NETrafficController::sTrafficLight & input )
{
    stream >> input.trafficDirection;
    stream >> input.lightVehicle;
    stream >> input.lightPedestrian;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NETrafficController::sTrafficLight & output )
{
    stream << output.trafficDirection;
    stream << output.lightVehicle;
    stream << output.lightPedestrian;
    return stream;
}

/**
 * Returns string value of NETrafficController::eMessageIDs
 **/
inline const char * NETrafficController::getString( NETrafficController::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth:
        return "NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth";
    case    NETrafficController::eMessageIDs::MsgId_broadcastEastWest:
        return "NETrafficController::eMessageIDs::MsgId_broadcastEastWest";
    case    NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
        return "NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth";
    case    NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
        return "NETrafficController::eMessageIDs::MsgId_TrafficEastWest";

    case NETrafficController::eMessageIDs::MsgId_NotProcessed:
        return "NETrafficController::eMessageIDs::MsgId_NotProcessed";
    case NETrafficController::eMessageIDs::MsgId_Invalid:
        return "NETrafficController::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NETrafficController::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NETrafficController namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NETRAFFICCONTROLLER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NETrafficController.hpp file
//////////////////////////////////////////////////////////////////////////
