//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NETrafficController.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficController.
 * Generated at     15.08.2021  00:03:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NETrafficController.cpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NETrafficController.hpp"

//////////////////////////////////////////////////////////////////////////
// NETrafficController functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NETrafficController::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    const unsigned int * _RequestList       = static_cast<const unsigned int *>(NULL); /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NETrafficController::MSG_ID_broadcastSouthNorth  ) // broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
        , static_cast<unsigned int>( NETrafficController::MSG_ID_broadcastEastWest   ) // broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NETrafficController::MSG_ID_TrafficSouthNorth   ) // NETrafficController::sTrafficLight mTrafficSouthNorth;
        , static_cast<unsigned int>( NETrafficController::MSG_ID_TrafficEastWest     ) // NETrafficController::sTrafficLight mTrafficEastWest;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    const unsigned int * _RequestToResponseMap  = static_cast<const unsigned int *>(NULL); // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        2, // void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
        2, // void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )

    };
    
    /************************************************************************
     * TrafficController Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NETrafficController::ServiceName
        , NETrafficController::InterfaceVersion
        , NEService::ServiceRemote
        , 0
        , 2
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NETrafficController::eMessageIDs NETrafficController::getResponseId( NETrafficController::eMessageIDs reqId )
{
    return NETrafficController::MSG_ID_INVALID;
}

NETrafficController::eMessageIDs NETrafficController::getRequestId( NETrafficController::eMessageIDs respId )
{
    return NETrafficController::MSG_ID_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NETrafficController.cpp file
//////////////////////////////////////////////////////////////////////////
