#ifndef AREG_BASE_CEREMOTEMESSAGE_HPP
#define AREG_BASE_CEREMOTEMESSAGE_HPP
/************************************************************************
 * \file        areg/base/CERemoteMessage.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between different processes running either on same or 
 *              different hosts. The Data in buffer within same process
 *              remains valid until  the number of references to the 
 *              buffer is more than zero.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/CESharedBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// CERemoteMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote message is used for IPC communication, which is an
 *          instance of shared buffer. Unlike internal communication message,
 *          the remote message contains additional information to deliver
 *          messages to target application for further processing.
 **/
class AREG_API CERemoteMessage  : public    CESharedBuffer
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
    friend class CESocket;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CERemoteMessage(unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Constructor to reserve space for byte buffer object
     * \param   reserveSize Size in bytes to reserve
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CERemoteMessage( unsigned int reserveSize, unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief	Initialization constructor, writes given data into byte buffer
     * \param	buffer      The data to initialize byte buffer
     * \param	size        The length in bytes of data
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CERemoteMessage(const unsigned char*    buffer, 
                          unsigned int      size, 
                          unsigned int      blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief	Initialization constructor. 
     *          It will not copy data, it will increase the reference counter by 1
     *          to prevent any other object deleting shared buffer object.
     *          The shared buffer should not be invalid. Otherwise it will be ignored.
     * \param	remoteMessage   The reference to remote message object to share
     **/
    CERemoteMessage(NEMemory::sRemoteMessage & remoteMessage );

    /**
     * \brief	Copy constructor. It does not copy data from src, it will refer to the same shared
     *          byte buffer object and will increase reference counter by one to prevent any other
     *          shared buffer object deleting used data
     * \param	src     The source of shared buffer object instance.
     **/
    CERemoteMessage( const CERemoteMessage & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CERemoteMessage( void );

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Assigning operator, it does not copy source data, it increases
     *          byte buffer reference counter by one to prevent other shared
     *          object instance to delete buffer
     * \param	src	    Reference to source object
     **/
    const CERemoteMessage & operator = ( const CERemoteMessage & src );

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CERemoteMessage & input );

    /**
     * \brief	Friend operator to make Remote Message streamable.
     *          Writes data from remote message to streaming object.
     * \param	stream	The streaming object to write data
     * \param	output	The Remote Message object to read data
     * \return	Reference to Streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CERemoteMessage & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Returns checksum value of Remote Buffer.
     *          The checksum value cannot be set. It is calculated by call ChecksumMark().
     **/
    inline unsigned int GetChecksum( void ) const;

    /**
     * \brief   Returns the ID of remote source set in Remote Buffer header.
     **/
    inline ITEM_ID GetSource( void ) const;
    
    /**
     * \brief   Sets the ID of source in Remote Buffer.
     * \param   idSource    The ID of source to set in Remote Buffer
     **/
    inline void SetSource( ITEM_ID idSource );

    /**
     * \brief   Returns the ID of remote target set in Remote Buffer header.
     **/
    inline ITEM_ID GetTarget( void ) const;
    
    /**
     * \brief   Sets the ID of target in Remote Buffer.
     * \param   idTarget    The ID of target to set in Remote Buffer
     **/
    inline void SetTarget( ITEM_ID idTarget );

    /**
     * \brief   Returns the message ID value set in remote buffer
     **/
    inline unsigned int GetMessageId( void ) const;

    /**
     * \brief   Sets new Message ID value in Remote Buffer.
     * \param   newMessageId    New Message ID value set in Remote Buffer
     **/
    inline void SetMessageId( unsigned int newMessageId );

    /**
     * \brief   Returns result of processed message
     **/
    inline unsigned int GetResult( void ) const;

    /**
     * \brief   Sets result or processed message
     **/
    inline void SetResult( unsigned int newResult );

    /**
     * \brief   Returns Sequence number value set in Remote Buffer.
     **/
    inline unsigned int GetSequenceNr( void ) const;

    /**
     * \brief   Sets new Sequence number value in Remote Buffer.
     * \param   newSequenceNr   New Sequence number value set in Remote Buffer
     **/
    inline void SetSequenceNr( unsigned int newSequenceNr );

    /**
     * \brief   Returns true if marked checksum value is valid. Otherwise, it returns false
     **/
    bool IsChecksumValid( void ) const;

    /**
     * \brief   Call when completed modifying buffer. Completion will fix such values as
     *          length of Remote Buffer and checksum. When changing length of buffer,
     *          it will no resize the buffer and re-copy data, but it will use the value
     *          of used space, which is set in header. The checksum will be calculated
     *          based on available information in the buffer and it will set value.
     *          It is strongly recommended to call method again if the buffer was changed
     *          or before transferring buffer to remote target.
     **/
    void BufferCompletionFix( void );

    /**
     * \brief   Initializes new buffer based on given Byte Buffer Header data.
     *          If succeeds to allocate new buffer, sets reference counter to 1,
     *          sets data used size to the value specified in header.
     *          The method expects that allocated data will be manually filled.
     * \param   rmHeader    Instance of Remote Buffer Header containing buffer information
     * \return  Returns pointer to allocated data buffer to copy data.
     **/
    unsigned char * InitializeMessage( const NEMemory::sRemoteMessageHeader & rmHeader );

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
     * \param	newBuffer	The buffer to initialize. If NULL, the buffer object. 
     * \param	bufLength	The length of entire buffer, i.e. length of complete byte buffer.
     * \param	makeCopy	If 'true' it will make copy of existing buffer
     * \return	Returns the current writing position in initialized buffer.
     *          If buffer is invalid, it will return INVALID_CURSOR_POSITION.
     *          If no data is copied, it will return position at the beginning of buffer.
     *          If data is copied, will return the position of written data.
     **/
    virtual unsigned int InitBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const;

    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual int SizeDataOffset( void ) const;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    virtual int SizeBufferStruct( void ) const;

private:
    /**
     * \brief   Returns converted instance of Remote Message header.
     **/
    inline const NEMemory::sRemoteMessageHeader & getHeader( void ) const;
    inline NEMemory::sRemoteMessageHeader & getHeader( void );

    /**
     * \brief   Returns converted instance of Remote Buffer
     **/
    inline const NEMemory::sRemoteMessage & getRemoteMessage( void ) const;
    inline NEMemory::sRemoteMessage & getRemoteMessage( void );

    /**
     * \brief   Calculates and returns the checksum value of given remote message
     **/
    static unsigned int _checksumCalculate( const NEMemory::sRemoteMessage & remoteMessage );
};

//////////////////////////////////////////////////////////////////////////
// CERemoteMessage class inline functions
//////////////////////////////////////////////////////////////////////////
inline const NEMemory::sRemoteMessageHeader & CERemoteMessage::getHeader( void ) const
{   return reinterpret_cast<const NEMemory::sRemoteMessageHeader &>(GetByteBuffer());  }

inline NEMemory::sRemoteMessageHeader & CERemoteMessage::getHeader( void )
{   return reinterpret_cast<NEMemory::sRemoteMessageHeader &>(GetByteBuffer());        }

inline const NEMemory::sRemoteMessage & CERemoteMessage::getRemoteMessage( void ) const
{   return reinterpret_cast<const NEMemory::sRemoteMessage &>(GetByteBuffer());        }

inline NEMemory::sRemoteMessage & CERemoteMessage::getRemoteMessage( void )
{   return reinterpret_cast<NEMemory::sRemoteMessage &>(GetByteBuffer());              }

inline unsigned int CERemoteMessage::GetChecksum( void ) const
{   return getHeader().rbhChecksum;                                                     }

inline ITEM_ID CERemoteMessage::GetSource( void ) const
{   return getHeader().rbhSource;                                                       }

inline void CERemoteMessage::SetSource( ITEM_ID idSource )
{   if ( IsValid() ) getHeader().rbhSource       = idSource;                            }

inline ITEM_ID CERemoteMessage::GetTarget( void ) const
{   return getHeader().rbhTarget;                                                       }

inline void CERemoteMessage::SetTarget( ITEM_ID idTarget )
{   if ( IsValid() ) getHeader().rbhTarget       = idTarget;                            }

inline unsigned int CERemoteMessage::GetMessageId( void ) const
{   return getHeader().rbhMessageId;                                                    }

inline void CERemoteMessage::SetMessageId( unsigned int newMessageId )
{   if ( IsValid() ) getHeader().rbhMessageId   = newMessageId;                         }

inline unsigned int CERemoteMessage::GetResult( void ) const
{   return getHeader().rbhResult;                                                       }

inline void CERemoteMessage::SetResult( unsigned int newResult )
{   if ( IsValid() ) getHeader().rbhResult      = newResult;                            }

inline unsigned int CERemoteMessage::GetSequenceNr(void) const
{   return getHeader().rbhSequenceNr;                                                   }

inline void CERemoteMessage::SetSequenceNr( unsigned int newSequenceNr )
{   if ( IsValid() ) getHeader().rbhSequenceNr  = newSequenceNr;                        }

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const IEInStream & operator >> (const IEInStream & stream, CERemoteMessage & input)
{
    if ( static_cast<const IEInStream *>(&stream) != static_cast<const IEInStream *>(&input) )
    {
        stream.Read(input);
        input.MoveToBeginOfData();
    }
    return stream;
}

inline IEOutStream & operator << (IEOutStream & stream, const CERemoteMessage & output)
{
    if ( (static_cast<const IEOutStream *>(&stream)) != (static_cast<const IEOutStream *>(&output)) )
        stream.Write(output);
    return stream;
}

#endif  // AREG_BASE_CEREMOTEMESSAGE_HPP
