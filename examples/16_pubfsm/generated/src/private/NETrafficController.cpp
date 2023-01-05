//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NETrafficController.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/NETrafficController.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface namespace implementation.
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
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth  ) // broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
        , static_cast<unsigned int>( NETrafficController::eMessageIDs::MsgId_broadcastEastWest    ) // broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth    ) // NETrafficController::sTrafficLight mTrafficSouthNorth;
        , static_cast<unsigned int>( NETrafficController::eMessageIDs::MsgId_TrafficEastWest      ) // NETrafficController::sTrafficLight mTrafficEastWest;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
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
        , NEService::eServiceType::ServicePublic
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

NETrafficController::eMessageIDs NETrafficController::getResponseId( NETrafficController::eMessageIDs /* reqId */ )
{
    return NETrafficController::eMessageIDs::MsgId_Invalid;
}

NETrafficController::eMessageIDs NETrafficController::getRequestId( NETrafficController::eMessageIDs /* respId */ )
{
    return NETrafficController::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NETrafficController.cpp file
//////////////////////////////////////////////////////////////////////////
