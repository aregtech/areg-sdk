#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_GENERATED_SRC_PATIENTINFOEVENT_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_GENERATED_SRC_PATIENTINFOEVENT_HPP

/************************************************************************
 * \file            generated/src/PatientInfoEvent.hpp
 * \ingroup         Patient monitoring Service
 * \brief           Common event object used to communication with worker threads.
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "generated/src/NEPatientInformation.hpp"

class PatientInfoEventData
{
public:
    typedef enum E_UpdateCommands
    {
          CMD_Undefined     = 0
        , CMD_PatientInfo
    } eUpdateCommands;

public:
    inline PatientInfoEventData( void );

    inline PatientInfoEventData( const NEPatientInformation::PatientInfo & infoPatient );

    inline PatientInfoEventData( const SharedBuffer & data );

    inline PatientInfoEventData( const PatientInfoEventData & src );

    inline void operator = ( const PatientInfoEventData & src );

    inline const SharedBuffer & getData( void ) const;

    DECLARE_STREAMABLE( PatientInfoEventData::eUpdateCommands );

private:

    SharedBuffer    mData;
};

IMPLEMENT_STREAMABLE( PatientInfoEventData::eUpdateCommands );
DECLARE_EVENT( PatientInfoEventData, PatientInfoEvent, IEPatientInfoEventConsumer );

//////////////////////////////////////////////////////////////////////////
// inline methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInfoEventData class inline methods
//////////////////////////////////////////////////////////////////////////
inline PatientInfoEventData::PatientInfoEventData( void )
    : mData ( )
{
}

inline PatientInfoEventData::PatientInfoEventData( const NEPatientInformation::PatientInfo & infoPatient )
    : mData ( )
{
    mData << PatientInfoEventData::CMD_PatientInfo;
    mData << infoPatient;
}

inline PatientInfoEventData::PatientInfoEventData( const SharedBuffer & data )
    : mData ( data )
{
}

inline PatientInfoEventData::PatientInfoEventData( const PatientInfoEventData & src )
    : mData ( src.mData )
{
}

inline const SharedBuffer & PatientInfoEventData::getData( void ) const
{
    return mData;
}

inline void PatientInfoEventData::operator = ( const PatientInfoEventData & src )
{
    mData   = src.mData;
}

#endif  // AREG_EXAMPLES_15_PUBSVCWORKER_GENERATED_SRC_PATIENTINFOEVENT_HPP
