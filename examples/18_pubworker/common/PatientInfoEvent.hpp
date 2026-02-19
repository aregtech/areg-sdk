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
#include "areg/component/EventTemplate.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "examples/18_pubworker/services/PatientInformation.hpp"

/**
 * \brief   The patient information event data, event and the event consumer object declaration.
 **/
class PatientInfoEventData
{
public:
    /**
     * \brief   Defines the data type contained in buffer.
     **/
    enum class UpdateCommands : uint8_t
    {
          CMD_Undefined = 0     //!< No information is in buffer or it is undefined.
        , CMD_PatientInfo       //!< The buffer contains patient information data.
    };

//////////////////////////////////////////////////////////////////////////
// Constructors, destructor, operators and the attributes.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor.
     **/
    inline PatientInfoEventData();

    /**
     * \brief   Initializes the patient information 
     **/
    inline PatientInfoEventData( const PatientInformation::PatientInfo & infoPatient );

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
    inline PatientInfoEventData( PatientInfoEventData && src ) noexcept;

    /**
     * \brief   Copies patient information data from given source.
     **/
    inline void operator = ( const PatientInfoEventData & src );

    /**
     * \brief   Moves patient information data from given source.
     **/
    inline void operator = ( PatientInfoEventData && src ) noexcept;

    /**
     * \brief   Gets buffer that contains patient information.
     **/
    inline const SharedBuffer & getData() const;

    /**
     * \brief   Declare PatientInfoEventData::UpdateCommands as streamable.
     **/
    AREG_DECLARE_STREAMABLE( PatientInfoEventData::UpdateCommands );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    SharedBuffer    mData;  //!< The shared buffer that contains patient information.
};

//////////////////////////////////////////////////////////////////////////
// Implement PatientInfoEventData::UpdateCommands streamable.
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE( PatientInfoEventData::UpdateCommands );

//////////////////////////////////////////////////////////////////////////
// Define custom event and the event consumer.
//////////////////////////////////////////////////////////////////////////
AREG_DECLARE_EVENT( PatientInfoEventData, PatientInfoEvent, IEPatientInfoEventConsumer );

//////////////////////////////////////////////////////////////////////////
// inline methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInfoEventData class inline methods
//////////////////////////////////////////////////////////////////////////
inline PatientInfoEventData::PatientInfoEventData()
    : mData ( )
{
}

inline PatientInfoEventData::PatientInfoEventData( const PatientInformation::PatientInfo & infoPatient )
    : mData ( )
{
    mData << PatientInfoEventData::UpdateCommands::CMD_PatientInfo;
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

inline PatientInfoEventData::PatientInfoEventData( PatientInfoEventData && src ) noexcept
    : mData ( static_cast<SharedBuffer &&>(src.mData) )
{
}

inline const SharedBuffer & PatientInfoEventData::getData() const
{
    return mData;
}

inline void PatientInfoEventData::operator = ( const PatientInfoEventData & src )
{
    mData   = src.mData;
}

inline void PatientInfoEventData::operator = ( PatientInfoEventData && src ) noexcept
{
    mData   = std::move(src.mData);
}
