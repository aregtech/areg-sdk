#ifndef AREG_COMPONENT_CEEVENTDATA_HPP
#define AREG_COMPONENT_CEEVENTDATA_HPP
/************************************************************************
 * \file        areg/src/component/CEEventData.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Data object containing data to
 *              support request and response communication.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEEventDataStream.hpp"
#include "areg/src/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventData class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CEEventData class, used in request and response events
 ************************************************************************/
/**
 * \brief       Event Data object is used in request and response events
 *              to transfer data. It contains minimum required information
 *              like message ID, data type and data serialized in 
 *              binary buffer.
 * 
 * \details     All Service Interface request and response message should
 *              contain event data to be able to transfer certain information.
 *              The transfered information depends on specific request
 *              and response. Minimum, Event Data should contain message
 *              ID and data type if they have no parameters. If message
 *              contains parameters, they are serialized in data buffer.
 *              De-serialization of parameters depends on parameter type.
 *              and specific for every call.
 *
 **/
class AREG_API CEEventData
{
//////////////////////////////////////////////////////////////////////////
// List of friend classes to access protected members
//////////////////////////////////////////////////////////////////////////
    friend class CELocalRequestEvent;
    friend class CERemoteRequestEvent;
    friend class CELocalResponseEvent;
    friend class CERemoteResponseEvent;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor.
     * \param	msgId	The ID of communication message. 
     *                  Data type will be set according of 
     *                  message ID type.
     * \param	name	Optional. Used to name data streaming object.
     *                  Can be NULL if there is no need to name streaming object.
     **/
    CEEventData(unsigned int msgId, CEEventDataStream::eEventData dataType, const char* name = NULL);

    /**
     * \brief	Constructor.
     * \param	msgId	The ID of communication message. 
     *                  Data type will be set according of 
     *                  message ID type.
     * \param	args	Streaming object containing serialized information
     *                  of parameters.
     * \param	name	Optional. Used to name data streaming object.
     *                  Can be NULL if there is no need to name streaming object.
     **/
    CEEventData(unsigned int msgId, const CEEventDataStream & args, const char* name = NULL);

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CEEventData(const CEEventData & src);
    
    /**
     * \brief   Initialization constructor.
     *          Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CEEventData( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~CEEventData( void );

public:
/************************************************************************/
// Friend global operators to stream Event Data Buffer
/************************************************************************/

    /**
     * \brief   Copies event data from given source.
     * \param   src     The source of data to copy.
     **/
    const CEEventData & operator = ( const CEEventData & src );

    /**
     * \brief	Friend global operator to initialize data from streaming.
     * \param	stream	The data streaming object to read data.
     * \param	input	The Event Data Buffer object to write data.
     * \return	Reference to Streaming object.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEEventData & input );

    /**
     * \brief	Friend global operator to write object into streaming buffer.
     * \param	stream	The data streaming object to write data.
     * \param	output	The Event Data Buffer object of data source.
     * \return	Reference to Streaming object.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEEventData & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Returns type of data. There are 2 types of data defined.
     *          It is either request or response. The update messages
     *          are classified as response.
     **/
    inline NEService::eMessageDataType GetMessageDataType( void ) const;
    
    /**
     * \brief   Returns reference of data input streaming object
     *          to de-serialize message parameters.
     **/
    inline const IEInStream & GetReadStream( void ) const;
    
    /**
     * \brief   Returns reference of data output streaming object
     *          to serialize message parameters
     **/
    inline IEOutStream & GetWriteStream( void );

    /**
     * \brief   Returns reference of data container object,
     *          which is a streaming object.
     **/
    inline const CEEventDataStream & GetDataStream( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The type of data
     **/
    NEService::eMessageDataType    mDataType;
    /**
     * \brief   Streaming object, containing data in binary format.
     **/
    CEEventDataStream               mData;
};

//////////////////////////////////////////////////////////////////////////
// CEEventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::eMessageDataType CEEventData::GetMessageDataType( void ) const
{   return mDataType;                   }

inline const IEInStream& CEEventData::GetReadStream( void ) const
{   return mData.GetStreamForRead();    }

inline IEOutStream & CEEventData::GetWriteStream( void )
{   return mData.GetStreamForWrite();   }

inline const CEEventDataStream & CEEventData::GetDataStream( void ) const
{   return mData;                       }

#endif  // AREG_COMPONENT_CEEVENTDATA_HPP
