//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEPatientInformation.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 * Generated at     11.08.2021  15:53:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NEPatientInformation.cpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation Service Interface Namespace implementation.
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
    const unsigned int * _RequestList       = static_cast<const unsigned int *>(NULL); /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    const unsigned int * _ResponseList      = static_cast<const unsigned int *>(NULL); /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEPatientInformation::MSG_ID_Patient  ) // NEPatientInformation::PatientInfo mPatient;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    const unsigned int * _RequestToResponseMap  = static_cast<const unsigned int *>(NULL); // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    const unsigned int * _ResponseParamStateMap = static_cast<const unsigned int *>(NULL); // EMPTY RESPONSE AND BROADCAST LIST
    
    /************************************************************************
     * PatientInformation Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEPatientInformation::ServiceName
        , NEPatientInformation::InterfaceVersion
        , NEService::ServiceRemote
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

NEPatientInformation::eMessageIDs NEPatientInformation::getResponseId( NEPatientInformation::eMessageIDs reqId )
{
    return NEPatientInformation::MSG_ID_INVALID;
}

NEPatientInformation::eMessageIDs NEPatientInformation::getRequestId( NEPatientInformation::eMessageIDs respId )
{
    return NEPatientInformation::MSG_ID_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEPatientInformation.cpp file
//////////////////////////////////////////////////////////////////////////
