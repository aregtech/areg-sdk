#pragma once

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
#include "examples/15_pubworker/services/NEPatientInformation.hpp"

/**
 * \brief   The patient information event data, event and the event consumer object declaration.
 **/
class PatientInfoEventData
{
public:
    /**
     * \brief   Defines the data type contained in buffer.
     **/
    typedef enum E_UpdateCommands
    {
          CMD_Undefined     = 0 //!< No information is in buffer or it is undefined.
        , CMD_PatientInfo       //!< The buffer contains patient information data.
    } eUpdateCommands;

//////////////////////////////////////////////////////////////////////////
// Constructors, destructor, operators and the attributes.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor.
     **/
    inline PatientInfoEventData( void );

    /**
     * \brief   Initializes the patient information 
     **/
    inline PatientInfoEventData( const NEPatientInformation::PatientInfo & infoPatient );

    /**
     * \brief   Sets the shared buffer that already contains patient information data.
     **/
    inline PatientInfoEventData( const SharedBuffer & data );

    /**
     * \brief   Copies patient information data from given source.
     **/
    inline PatientInfoEventData( const PatientInfoEventData & src );

    /**
     * \brief   Moves patient information data from given source.
     **/
    inline PatientInfoEventData( PatientInfoEventData && src );

    /**
     * \brief   Copies patient information data from given source.
     **/
    inline void operator = ( const PatientInfoEventData & src );

    /**
     * \brief   Moves patient information data from given source.
     **/
    inline void operator = ( PatientInfoEventData && src );

    /**
     * \brief   Gets buffer that contains patient information.
     **/
    inline const SharedBuffer & getData( void ) const;

    /**
     * \brief   Declare PatientInfoEventData::eUpdateCommands as streamable.
     **/
    DECLARE_STREAMABLE( PatientInfoEventData::eUpdateCommands );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    SharedBuffer    mData;  //!< The shared buffer that contains patient information.
};

//////////////////////////////////////////////////////////////////////////
// Implement PatientInfoEventData::eUpdateCommands streamable.
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE( PatientInfoEventData::eUpdateCommands );

//////////////////////////////////////////////////////////////////////////
// Define custom event and the event consumer.
//////////////////////////////////////////////////////////////////////////
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

inline PatientInfoEventData::PatientInfoEventData( PatientInfoEventData && src )
    : mData ( static_cast<SharedBuffer &&>(src.mData) )
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

inline void PatientInfoEventData::operator = ( PatientInfoEventData && src )
{
    mData   = static_cast<SharedBuffer &&>(src.mData);
}
