//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NETrafficController.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NETRAFFICCONTROLLER_HPP
#define  GENERATED_SRC_NETRAFFICCONTROLLER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficController.
 * Generated at     15.08.2021  00:03:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/NETrafficController.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController Service Interface Namespace declaration.
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
#include "areg/component/NEService.hpp"

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
     * \brief   Service Interface Name
     *          NETrafficController::ServiceName
     **/
    const char * const    ServiceName         = "TrafficController";
    /**
     * \brief   Implemented Version
     *          NETrafficController::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The direction of traffic.
     *          NETrafficController::eTrafficDirection
     **/
    enum eTrafficDirection
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
    enum eVehicleTrafficLight
    {
        /**
         * \brief   The vehicle lights are OFF.
         **/
          VhicleLightOFF      
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
    enum ePedestrianTrafficLight
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
         * \param   src     The source of NETrafficController::sTrafficLight structure to copy data
         **/
        inline sTrafficLight( const NETrafficController::sTrafficLight & src );

        /**
         * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
         **/
        inline ~sTrafficLight( void );
    
    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////
        
        /**
         * \brief   Copies data from given source
         * \param   src     The source of sTrafficLight structure to copy data.
         **/
        inline const NETrafficController::sTrafficLight & operator = ( const NETrafficController::sTrafficLight & src );
        
        /**
         * \brief   Checks equality and returns true if 2 instances of sTrafficLight are equal
         * \param   other   The instance of sTrafficLight to compare
         * \return  Returns true if 2 instances are equal
         **/ 
        inline bool operator == ( const NETrafficController::sTrafficLight & other ) const;
        
        /**
         * \brief   Checks inequality and returns true if 2 instances of sTrafficLight are not equal
         * \param   other   The instance of sTrafficLight to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
        inline bool operator != ( const NETrafficController::sTrafficLight & other ) const;

        /**
         * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
         **/
         inline operator unsigned int ( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // sTrafficLight fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   DESCRIPTION MISSED
         **/
         NETrafficController::eTrafficDirection          trafficDirection    /* = DirectionUndefiend */;
        /**
         * \brief   The state of vehicle lights.
         **/
         NETrafficController::eVehicleTrafficLight       lightVehicle        /* = VhicleLightOFF */;
        /**
         * \brief   Describes the state of pedestrian lights.
         **/
         NETrafficController::ePedestrianTrafficLight    lightPedestrian     /* = PedestrianLightOFF */;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NETrafficController::eMessageIDs
     *          TrafficController Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
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
         * \brief   Broadcast ID: MSG_ID_broadcastSouthNorth
         *          Broadcasts the light states of south-north direction.
         **/
          MSG_ID_broadcastSouthNorth   = NEService::RESPONSE_ID_FIRST    // void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastEastWest
         *          Broadcasts the light states of east-west direction.
         **/
        , MSG_ID_broadcastEastWest                                      // void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_TrafficSouthNorth
         *          The south-north traffic.
         **/
        , MSG_ID_TrafficSouthNorth    = NEService::ATTRIBUTE_ID_FIRST   // NETrafficController::sTrafficLight mTrafficSouthNorth;
        /**
         * \brief   Attribute ID: MSG_ID_TrafficEastWest
         *          The east-west traffic
         **/
        , MSG_ID_TrafficEastWest                                        // NETrafficController::sTrafficLight mTrafficEastWest;
     
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
    case    NETrafficController::DirectionUndefiend:
        return "NETrafficController::DirectionUndefiend";
    case    NETrafficController::DirectionSouthNorth:
        return "NETrafficController::DirectionSouthNorth";
    case    NETrafficController::DirectionEastWest:
        return "NETrafficController::DirectionEastWest";
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
    case    NETrafficController::VhicleLightOFF:
        return "NETrafficController::VhicleLightOFF";
    case    NETrafficController::VehicleLightsInit:
        return "NETrafficController::VehicleLightsInit";
    case    NETrafficController::VehicleLightRed:
        return "NETrafficController::VehicleLightRed";
    case    NETrafficController::VehicleLightYellow:
        return "NETrafficController::VehicleLightYellow";
    case    NETrafficController::VehicleLightGreen:
        return "NETrafficController::VehicleLightGreen";
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
    case    NETrafficController::PedestrianLightOFF:
        return "NETrafficController::PedestrianLightOFF";
    case    NETrafficController::PedestrianLightRed:
        return "NETrafficController::PedestrianLightRed";
    case    NETrafficController::PedestrianLightGreen:
        return "NETrafficController::PedestrianLightGreen";
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
    : trafficDirection    ( DirectionUndefiend )
    , lightVehicle        ( VhicleLightOFF )
    , lightPedestrian     ( PedestrianLightOFF )
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

inline NETrafficController::sTrafficLight::~sTrafficLight( void )
{   /* do nothing */    }

inline const NETrafficController::sTrafficLight & NETrafficController::sTrafficLight::operator = ( const NETrafficController::sTrafficLight & src )
{
    if ( static_cast<const NETrafficController::sTrafficLight *>(this) != &src )
    {
            this->trafficDirection    = src.trafficDirection;
            this->lightVehicle        = src.lightVehicle;
            this->lightPedestrian     = src.lightPedestrian;
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

inline NETrafficController::sTrafficLight::operator unsigned int ( void ) const
{
    return (
              static_cast<unsigned int>( trafficDirection     )
            + static_cast<unsigned int>( lightVehicle         )
            + static_cast<unsigned int>( lightPedestrian      )
            );
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
    case    NETrafficController::MSG_ID_broadcastSouthNorth:
        return "NETrafficController::MSG_ID_broadcastSouthNorth";
    case    NETrafficController::MSG_ID_broadcastEastWest:
        return "NETrafficController::MSG_ID_broadcastEastWest";
    case    NETrafficController::MSG_ID_TrafficSouthNorth:
        return "NETrafficController::MSG_ID_TrafficSouthNorth";
    case    NETrafficController::MSG_ID_TrafficEastWest:
        return "NETrafficController::MSG_ID_TrafficEastWest";

    case NETrafficController::MSG_ID_NO_PROCEED:
        return "NETrafficController::MSG_ID_NO_PROCEED";
        
    case NETrafficController::MSG_ID_INVALID:
        return "NETrafficController::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NETrafficController::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NETrafficController namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_NETRAFFICCONTROLLER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NETrafficController.hpp file
//////////////////////////////////////////////////////////////////////////
