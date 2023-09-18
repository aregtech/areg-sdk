#ifndef AREG_BASE_REMOTEMESSAGE_HPP
#define AREG_BASE_REMOTEMESSAGE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/RemoteMessage.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between connected nodes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/SharedBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote message is used for IPC communication, which is an
 *          instance of shared buffer. Unlike internal communication message,
 *          the remote message contains additional information to deliver
 *          messages to target application for further processing.
 **/
class AREG_API RemoteMessage  : public    SharedBuffer
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
    friend class Socket;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialize object by default values.
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    explicit RemoteMessage(unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Constructor to reserve space for byte buffer object.
     * \param   reserveSize Size in bytes to reserve
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    RemoteMessage( unsigned int reserveSize, unsigned int blockSize );

    /**
     * \brief	Initializes and writes given data into byte buffer.
     * \param	buffer      The data to initialize byte buffer.
     * \param	size        The length in bytes of data.
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size).
     **/
    RemoteMessage(const unsigned char * buffer, unsigned int size, unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief	It does not make hard copy of data from given source, it will refer to the same shared
     *          byte buffer object and will increase reference counter by one to prevent any other
     *          shared buffer object deleting used data
     * \param	src     The source of shared buffer object instance.
     **/
    RemoteMessage( const RemoteMessage & src ) = default;

    /**
     * \brief	Move the data from given source.
     * \param	src     The source of data.
     **/
    RemoteMessage( RemoteMessage && src ) noexcept = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteMessage( void ) = default;

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Assigning operator, it does not copy source data, it increases
     *          byte buffer reference counter by one to prevent other shared
     *          object instance to delete buffer
     * \param	src	    Reference to source remote buffer data.
     **/
    RemoteMessage & operator = ( const RemoteMessage & src ) = default;

    /**
     * \brief	Move operator. Moves data from given source.
     * \param	src	    The source of data.
     **/
    RemoteMessage & operator = ( RemoteMessage && src ) noexcept = default;

    /************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief	Friend operator to make Remote Message streamable.
     *          Writes data from streaming object to remote message
     * \param	stream	The streaming object to read data
     * \param	input	The Remote Buffer object to write data
     * \return	Reference to Streaming object.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, RemoteMessage & input );

    /**
     * \brief	Friend operator to make Remote Message streamable.
     *          Writes data from remote message to streaming object.
     * \param	stream	The streaming object to write data
     * \param	output	The Remote Message object to read data
     * \return	Reference to Streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const RemoteMessage & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns remote message structure data
     **/
    inline const NEMemory::sRemoteMessage * getRemoteMessage( void ) const;

    /**
     * \brief   Returns checksum value of Remote Buffer.
     *          The checksum value cannot be set. It is calculated by call ChecksumMark().
     **/
    inline unsigned int getChecksum( void ) const;

    /**
     * \brief   Returns the ID of remote source set in Remote Buffer header.
     **/
    inline const ITEM_ID & getSource( void ) const;

    /**
     * \brief   Sets the ID of source in Remote Buffer.
     * \param   idSource    The ID of source to set in Remote Buffer
     **/
    inline void setSource(const ITEM_ID & idSource);

    /**
     * \brief   Returns the ID of remote target set in Remote Buffer header.
     **/
    inline const ITEM_ID & getTarget( void ) const;

    /**
     * \brief   Sets the ID of target in Remote Buffer.
     * \param   idTarget    The ID of target to set in Remote Buffer
     **/
    inline void setTarget(const ITEM_ID & idTarget);

    /**
     * \brief   Returns the message ID value set in remote buffer
     **/
    inline unsigned int getMessageId( void ) const;

    /**
     * \brief   Sets new Message ID value in Remote Buffer.
     * \param   newMessageId    New Message ID value set in Remote Buffer
     **/
    inline void setMessageId( unsigned int newMessageId );

    /**
     * \brief   Returns result of processed message
     **/
    inline unsigned int getResult( void ) const;

    /**
     * \brief   Sets result or processed message
     **/
    inline void setResult( unsigned int newResult );

    /**
     * \brief   Returns Sequence number value set in Remote Buffer.
     **/
    inline const SequenceNumber & getSequenceNr( void ) const;

    /**
     * \brief   Sets new Sequence number value in Remote Buffer.
     * \param   newSequenceNr   New Sequence number value set in Remote Buffer
     **/
    inline void setSequenceNr(const SequenceNumber & newSequenceNr );

    /**
     * \brief   Returns true if marked checksum value is valid. Otherwise, it returns false
     **/
    bool isChecksumValid( void ) const;

    /**
     * \brief   Call when completed modifying buffer. Completion will fix such values as
     *          length of Remote Buffer and checksum. When changing length of buffer,
     *          it will no resize the buffer and re-copy data, but it will use the value
     *          of used space, which is set in header. The checksum will be calculated
     *          based on available information in the buffer and it will set value.
     *          It is strongly recommended to call method again if the buffer was changed
     *          or before transferring buffer to remote target.
     **/
    void bufferCompletionFix( void ) const;

    /**
     * \brief   Initializes new buffer based on given Byte Buffer Header data.
     *          If succeeds to allocate new buffer, sets reference counter to 1,
     *          sets data used size to the value specified in header.
     *          The method expects that allocated data will be manually filled.
     * \param   rmHeader    Instance of Remote Buffer Header containing buffer information
     * \return  Returns pointer to allocated data buffer to copy data.
     **/
    unsigned char * initMessage( const NEMemory::sRemoteMessageHeader & rmHeader );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IEByteBuffer Attributes and operations
/************************************************************************/
    /**
     * \brief	Initialize passed buffer making it as byte buffer and
     *          returns the current writing position in initialized byte buffer.
     *          If needed, it will copy existing binary data to passed buffer.
     *          The function is called from child classes when new byte buffer
     *          is created and the existing data should be passed to new buffer.
     * \param	newBuffer	The buffer to initialize.
     * \param	bufLength	The length in bytes of the entire buffer object.
     * \param	makeCopy	If 'true' it will make copy of existing buffer
     * \return	Returns the current writing position in initialized buffer.
     *          If buffer is invalid, it will return INVALID_CURSOR_POSITION.
     *          If no data is copied, it will return position at the beginning of buffer.
     *          If data is copied, will return the position of written data.
     **/
    virtual unsigned int initBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const override;

    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual unsigned int getDataOffset( void ) const override;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    virtual unsigned int getHeaderSize( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns converted instance of Remote Message header.
     **/
    inline const NEMemory::sRemoteMessageHeader & _getHeader( void ) const;
    inline NEMemory::sRemoteMessageHeader & _getHeader( void );

    /**
     * \brief   Returns converted instance of Remote Buffer
     **/
    inline const NEMemory::sRemoteMessage & _getRemoteMessage( void ) const;
    inline NEMemory::sRemoteMessage & _getRemoteMessage( void );

    /**
     * \brief   Calculates and returns the checksum value of given remote message
     **/
    static unsigned int _checksumCalculate( const NEMemory::sRemoteMessage & remoteMessage );
};

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class inline functions
//////////////////////////////////////////////////////////////////////////
inline const NEMemory::sRemoteMessageHeader & RemoteMessage::_getHeader( void ) const
{
    return reinterpret_cast<const NEMemory::sRemoteMessageHeader &>(*getByteBuffer());
}

inline NEMemory::sRemoteMessageHeader & RemoteMessage::_getHeader( void )
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<NEMemory::sRemoteMessageHeader &>(*(mByteBuffer.get()));
}

inline const NEMemory::sRemoteMessage & RemoteMessage::_getRemoteMessage( void ) const
{
    return reinterpret_cast<const NEMemory::sRemoteMessage &>(*getByteBuffer());
}

inline NEMemory::sRemoteMessage & RemoteMessage::_getRemoteMessage( void )
{
    ASSERT( mByteBuffer.get( ) != nullptr );
    return reinterpret_cast<NEMemory::sRemoteMessage &>(*mByteBuffer.get());
}

inline const NEMemory::sRemoteMessage * RemoteMessage::getRemoteMessage( void ) const
{
    return reinterpret_cast<const NEMemory::sRemoteMessage *>(getByteBuffer());
}

inline unsigned int RemoteMessage::getChecksum( void ) const
{
    return _getHeader().rbhChecksum;
}

inline const ITEM_ID & RemoteMessage::getSource( void ) const
{
    return _getHeader().rbhSource;
}

inline void RemoteMessage::setSource(const ITEM_ID & idSource )
{
    if (isValid())
    {
        _getHeader().rbhSource = idSource;
    }
}

inline const ITEM_ID & RemoteMessage::getTarget( void ) const
{
    return _getHeader().rbhTarget;
}

inline void RemoteMessage::setTarget(const ITEM_ID & idTarget )
{
    if (isValid())
    {
        _getHeader().rbhTarget = idTarget;
    }
}

inline unsigned int RemoteMessage::getMessageId( void ) const
{
    return _getHeader().rbhMessageId;
}

inline void RemoteMessage::setMessageId( unsigned int newMessageId )
{
    if (isValid())
    {
        _getHeader().rbhMessageId = newMessageId;
    }
}

inline unsigned int RemoteMessage::getResult( void ) const
{
    return _getHeader().rbhResult;
}

inline void RemoteMessage::setResult( unsigned int newResult )
{
    if (isValid())
    {
        _getHeader().rbhResult = newResult;
    }
}

inline const SequenceNumber & RemoteMessage::getSequenceNr(void) const
{
    return _getHeader().rbhSequenceNr;
}

inline void RemoteMessage::setSequenceNr(const SequenceNumber & newSequenceNr )
{
    if ( isValid() )
    {
        _getHeader().rbhSequenceNr = newSequenceNr;
    }
}

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const IEInStream & operator >> (const IEInStream & stream, RemoteMessage & input)
{
    if ( static_cast<const IEInStream *>(&stream) != static_cast<const IEInStream *>(&input) )
    {
        stream.read(input);
        input.moveToBegin();
    }

    return stream;
}

inline IEOutStream & operator << (IEOutStream & stream, const RemoteMessage & output)
{
    if ( (static_cast<const IEOutStream *>(&stream)) != (static_cast<const IEOutStream *>(&output)) )
    {
        stream.write( output );
    }
    
    return stream;
}

#endif  // AREG_BASE_REMOTEMESSAGE_HPP
