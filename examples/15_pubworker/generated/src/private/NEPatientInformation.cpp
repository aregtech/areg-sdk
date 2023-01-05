//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPatientInformation.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:24 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/NEPatientInformation.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEPatientInformation.hpp"

//////////////////////////////////////////////////////////////////////////
// NEPatientInformation functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEPatientInformation::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseList  { nullptr }; /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEPatientInformation::eMessageIDs::MsgId_Patient  ) // NEPatientInformation::PatientInfo mPatient;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseParamStateMap { nullptr }; // EMPTY RESPONSE AND BROADCAST LIST

    /************************************************************************
     * PatientInformation Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEPatientInformation::ServiceName
        , NEPatientInformation::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 0
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEPatientInformation::eMessageIDs NEPatientInformation::getResponseId( NEPatientInformation::eMessageIDs /* reqId */ )
{
    return NEPatientInformation::eMessageIDs::MsgId_Invalid;
}

NEPatientInformation::eMessageIDs NEPatientInformation::getRequestId( NEPatientInformation::eMessageIDs /* respId */ )
{
    return NEPatientInformation::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPatientInformation.cpp file
//////////////////////////////////////////////////////////////////////////
